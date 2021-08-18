//=============================================================================
// FILE: svrAndroidMain.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
 
#include <android/looper.h>
#include <unistd.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <android_native_app_glue.h>
#include <cstring>

#include "svrApi.h"
#include "svrApplication.h"
#include "svrUtil.h"
#include "../aio_application.h"

#if !defined( EGL_OPENGL_ES3_BIT_KHR )
#define EGL_OPENGL_ES3_BIT_KHR		0x0040
#endif

#if !defined( EGL_PROTECTED_CONTENT_EXT )
#define EGL_PROTECTED_CONTENT_EXT   0x32C0
#endif

using namespace Svr;

static bool gContextCreated = false;

static bool gIsPaused = true;
static bool gSvrInitialized = false;
static bool gNeedsReInitialization = false;
static bool gRecreateContext = false;

void PauseVR(AioApplication* pApp);
void StartVR();
void EndVR(AioApplication*, bool need_exit);

static void LogGLExtensions()
{
    char* pGlExtensions = (char*)glGetString(GL_EXTENSIONS);

    LOGI("Supported GL Extensions:");
    if (pGlExtensions == NULL)
    {
        LOGI("    None!");
        return;
    }

    char* pCurrent = pGlExtensions;
    char* pEnd = NULL;
    char oneExtensionBuf[256];
    while (strlen(pCurrent) > 0)
    {
        // Look for delimiter
        pEnd = strstr(pCurrent, " ");
        if (pEnd == NULL)
        {
            // No more delimiters, print what is left
            LOGI("    %s", pCurrent);
            return;
        }

        // Print this value
        memset(oneExtensionBuf, 0, sizeof(oneExtensionBuf));
        memcpy(oneExtensionBuf, pCurrent, pEnd - pCurrent + 1);
        LOGI("    %s", oneExtensionBuf);

        // Step forward
        pCurrent = pEnd + 1;
    }
}

static void LogEGLExtensions(EGLDisplay display)
{
    char* pGlExtensions = (char*)eglQueryString(display, EGL_EXTENSIONS);

    LOGI("Supported EGL Extensions:");
    if (pGlExtensions == NULL)
    {
        LOGI("    None!");
        return;
    }

    char* pCurrent = pGlExtensions;
    char* pEnd = NULL;
    char oneExtensionBuf[256];
    while (strlen(pCurrent) > 0)
    {
        // Look for delimiter
        pEnd = strstr(pCurrent, " ");
        if (pEnd == NULL)
        {
            // No more delimiters, print what is left
            LOGI("    %s", pCurrent);
            return;
        }

        // Print this value
        memset(oneExtensionBuf, 0, sizeof(oneExtensionBuf));
        memcpy(oneExtensionBuf, pCurrent, pEnd - pCurrent + 1);
        LOGI("    %s", oneExtensionBuf);

        // Step forward
        pCurrent = pEnd + 1;
    }
}

static void deleteContext(SvrApplicationContext& appContext)
{
    //Recreate context.
    gContextCreated = false;
    EGLDisplay eglDisplay = appContext.display;
    EGLSurface eglSurface = appContext.eyeRenderSurface;
    EGLContext eglContext = appContext.eyeRenderContext;
    
    eglDestroySurface(appContext.display, appContext.eyeRenderSurface);
    eglDestroyContext(appContext.display, appContext.eyeRenderContext);
    appContext.display = EGL_NO_DISPLAY;
    appContext.eyeRenderContext = EGL_NO_CONTEXT;
    appContext.eyeRenderSurface = EGL_NO_SURFACE;
}

static int CreateRenderContext(SvrApplicationContext& appContext)
{
    deleteContext(appContext);
    // initialize OpenGL ES and EGL
    const EGLint attribs[] =
    {
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 0,
        EGL_NONE
    };

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;
    eglInitialize(display, &major, &minor);
    LOGI("EGL version %d.%d", major, minor);

    EGLConfig configs[512];
    EGLint numConfigs = 0;
    eglGetConfigs(display, configs, 512, &numConfigs);

    EGLConfig config = 0;
    for (int i = 0; i < numConfigs; i++)
    {
        EGLint value = 0;

        eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &value);
        if ((value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
        {
            continue;
        }

        eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &value);
        if ((value & (EGL_WINDOW_BIT | EGL_PBUFFER_BIT)) != (EGL_WINDOW_BIT | EGL_PBUFFER_BIT))
        {
            continue;
        }

        int	j = 0;
        for (; attribs[j] != EGL_NONE; j += 2)
        {
            eglGetConfigAttrib(display, configs[i], attribs[j], &value);
            if (value != attribs[j + 1])
            {
                break;
            }
        }
        if (attribs[j] == EGL_NONE)
        {
            config = configs[i];
            LOGI("CreateRenderContext, found EGL config");
            break;
        }
    }

    if (config == 0)
    {
        LOGI("CreateRenderContext, failed to find EGL config");
        return -1;
    }

    //Reconfigure the ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    EGLint format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(appContext.nativeWindow, 0, 0, format);

    //Create a surface 
    EGLSurface surface;
    EGLint isProtectedContent = EGL_FALSE;
    if (appContext.isProtectedContent)
    {
        isProtectedContent = EGL_TRUE;
        LOGI("isProtectedContent is true");
    }

    EGLint surfaceAttribs[] = 
    {
         EGL_PROTECTED_CONTENT_EXT, isProtectedContent,
         EGL_NONE
    };

    surface = eglCreateWindowSurface(display, config, appContext.nativeWindow, surfaceAttribs);
    
    if (surface == EGL_NO_SURFACE)
    {
        LOGE("CreateRenderContext: eglCreateWindowSurface failed");
        return -1;
    }

    //Create a context
    EGLContext context;
    EGLint contextAttribs [] = 
    {
         EGL_CONTEXT_CLIENT_VERSION, 3,
         EGL_PROTECTED_CONTENT_EXT, isProtectedContent,
         EGL_NONE
    };

    context = eglCreateContext(display, config, NULL, contextAttribs);

    if (context == EGL_NO_CONTEXT)
    {
        LOGE("CreateRenderContext: eglCreateContext failed");
        return -1;
    }

    //Attach the surface to the context
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        LOGE("CreateRenderContext: eglMakeCurrent failed");
        return -1;
    }

    EGLint w, h;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    appContext.display = display;
    appContext.eyeRenderContext = context;
    appContext.eyeRenderSurface = surface;

    return 0;
}

bool createContext(android_app *pApp)
{
    AioApplication* pFrmApp = (AioApplication*)pApp->userData;
    if (pApp->window != NULL)
    {
        LOGI("APP_CMD_INIT_WINDOW, Thread ID : %d", gettid());

        SvrApplicationContext& appContext = pFrmApp->GetApplicationContext();

        svrDeviceInfo deviceInfo = svrGetDeviceInfo();
        appContext.targetEyeHeight = deviceInfo.targetEyeHeightPixels;
        appContext.targetEyeWidth = deviceInfo.targetEyeWidthPixels;
        appContext.physicalHeight = deviceInfo.displayHeightPixels;
        appContext.physicalWidth = deviceInfo.displayWidthPixels;
        appContext.targetEyeFovXDeg = deviceInfo.targetFovXRad * RAD_TO_DEG;
        appContext.targetEyeFovYDeg = deviceInfo.targetFovYRad * RAD_TO_DEG;

        appContext.nativeWindow = pApp->window;

        if (CreateRenderContext(appContext) < 0)
        {
            LOGE("Failed to create eye render context");
            return false;
        }
        
        gRecreateContext = false;
        gNeedsReInitialization = true;
        gContextCreated = true;
    }
    return gContextCreated;
}

void CommandCallback(android_app* pApp, int32_t cmd)
{
    AioApplication* pFrmApp = (AioApplication*)pApp->userData;
    SvrApplicationContext& appContext = pFrmApp->GetApplicationContext();
    switch (cmd)
    {
        // Command from main thread: the AInputQueue has changed.  Upon processing
        // this command, android_app->inputQueue will be updated to the new queue
        // (or NULL).
    case APP_CMD_INPUT_CHANGED:
        LOGI("APP_CMD_INPUT_CHANGED");
        break;

        // Command from main thread: a new ANativeWindow is ready for use.  Upon
        // receiving this command, android_app->window will contain the new window
        // surface.
    case APP_CMD_INIT_WINDOW:
        LOGI("APP_CMD_INIT_WINDOW");
        createContext(pApp);
        break;

        // Command from main thread: the existing ANativeWindow needs to be
        // terminated.  Upon receiving this command, android_app->window still
        // contains the existing window; after calling android_app_exec_cmd
        // it will be set to NULL.
    case APP_CMD_TERM_WINDOW:
        LOGI("APP_CMD_TERM_WINDOW");
        gContextCreated = false;
        pFrmApp->Shutdown();
        break;

        // Command from main thread: the current ANativeWindow has been resized.
        // Please redraw with its new size.
    case APP_CMD_WINDOW_RESIZED:
        LOGI("APP_CMD_WINDOW_RESIZED");
        break;

        // Command from main thread: the system needs that the current ANativeWindow
        // be redrawn.  You should redraw the window before handing this to
        // android_app_exec_cmd() in order to avoid transient drawing glitches.
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        LOGI("APP_CMD_WINDOW_REDRAW_NEEDED");
        break;

        // Command from main thread: the content area of the window has changed,
        // such as from the soft input window being shown or hidden.  You can
        // find the new content rect in android_app::contentRect.
    case APP_CMD_CONTENT_RECT_CHANGED:
        LOGI("APP_CMD_CONTENT_RECT_CHANGED");
        break;

        // Command from main thread: the app's activity window has gained
        // input focus.
    case APP_CMD_GAINED_FOCUS:
        LOGI("APP_CMD_GAINED_FOCUS");
        break;

        // Command from main thread: the app's activity window has lost
        // input focus.
    case APP_CMD_LOST_FOCUS:
        LOGI("APP_CMD_LOST_FOCUS");
        break;

        // Command from main thread: the current device configuration has changed.
    case APP_CMD_CONFIG_CHANGED:
        LOGI("APP_CMD_CONFIG_CHANGED");
        gContextCreated = false;
        gRecreateContext = true;
        break;

        // Command from main thread: the system is running low on memory.
        // Try to reduce your memory use.
    case APP_CMD_LOW_MEMORY:
        LOGI("APP_CMD_LOW_MEMORY");
        break;

        // Command from main thread: the app's activity has been started.
    case APP_CMD_START:
        LOGI("APP_CMD_START");
        break;

        // Command from main thread: the app's activity has been resumed.
    case APP_CMD_RESUME:
        LOGI("APP_CMD_RESUME");
        gIsPaused = false;
            pFrmApp->OnResume();
        break;

        // Command from main thread: the app should generate a new saved state
        // for itself, to restore from later if needed.  If you have saved state,
        // allocate it with malloc and place it in android_app.savedState with
        // the size in android_app.savedStateSize.  The will be freed for you
        // later.
    case APP_CMD_SAVE_STATE:
        LOGI("APP_CMD_SAVE_STATE");
        break;

        // Command from main thread: the app's activity has been paused.
    case APP_CMD_PAUSE:
        LOGI("APP_CMD_PAUSE");
        PauseVR(pFrmApp);
        break;

        // Command from main thread: the app's activity has been stopped.
    case APP_CMD_STOP:
        LOGI("APP_CMD_STOP");
        PauseVR(pFrmApp);
        break;

        // Command from main thread: the app's activity is being destroyed,
        // and waiting for the app thread to clean up and exit before proceeding.
    case APP_CMD_DESTROY:
        LOGI("APP_CMD_DESTROY");
        break;

    default:
        LOGI("Unknown APP_CMD_: %d", cmd);
        break;
    }
}

int InputCallback(android_app* pApp, AInputEvent* event)
{
    AioApplication* pFrmApp = (AioApplication*)pApp->userData;

    // Want touch events in [0,1] for width [Left, Right] and height [Bottom, Top]
    // Actually, we want screen size here, NOT surface size,
    // Touch events come in as screen size so where you are touching gets messed up.
    //float fltWidth = (float)pFrmApp->GetWindowWidth();
    //float fltHeight = (float)pFrmApp->GetWindowHeight();
    float fltWidth = (float)ANativeWindow_getWidth(pApp->window);
    float fltHeight = (float)ANativeWindow_getHeight(pApp->window);

    // A key is pressed
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
    {
        // See <AndroidNDK>\platforms\android-9\arch-arm\usr\include\android\input.h
        int iKeyCode = AKeyEvent_getKeyCode(event);
        int iKeyAction = AKeyEvent_getAction(event);

        switch (iKeyCode)
        {
        case AKEYCODE_HOME:     //  = 3,
            LOGI("KeyCode: AKEYCODE_HOME");
            break;
        case AKEYCODE_BACK:     //  = 4,
            LOGI("KeyCode: AKEYCODE_BACK");
            break;
        case AKEYCODE_MENU:     //  = 82,
            LOGI("KeyCode: AKEYCODE_MENU");
            break;
        default:
//            LOGI("KeyCode: code %d action %d repeat %d", iKeyCode, iKeyAction, AKeyEvent_getRepeatCount(event));
            break;
        }

        switch (iKeyAction)
        {
        case AKEY_EVENT_ACTION_DOWN:
            pFrmApp->GetInput().KeyDownEvent(iKeyCode, AKeyEvent_getRepeatCount(event));
            break;
        case AKEY_EVENT_ACTION_UP:
            pFrmApp->GetInput().KeyUpEvent(iKeyCode, AKeyEvent_getRepeatCount(event));
            break;
        case AKEY_EVENT_ACTION_MULTIPLE:
            break;
        }

        return 1;
    }

    // Touch the screen
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        // See <AndroidNDK>\platforms\android-9\arch-arm\usr\include\android\input.h
        float fltOneX = 0.0f;
        float fltOneY = 0.0f;

        int iPointerAction = AMotionEvent_getAction(event);
        int iPointerIndx = (iPointerAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int iPointerID = AMotionEvent_getPointerId(event, iPointerIndx);
        int iAction = (iPointerAction & AMOTION_EVENT_ACTION_MASK);
        switch (iAction)
        {
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_DOWN:
            fltOneX = AMotionEvent_getX(event, iPointerIndx);
            fltOneY = AMotionEvent_getY(event, iPointerIndx);
            pFrmApp->GetInput().PointerDownEvent(iPointerID, fltOneX / fltWidth, fltOneY / fltHeight);
            break;
        
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_UP:
            //LogInfo("AMOTION_EVENT_ACTION_[POINTER]_UP: 0x%x", iPointerID);
            fltOneX = AMotionEvent_getX(event, iPointerIndx);
            fltOneY = AMotionEvent_getY(event, iPointerIndx);
            pFrmApp->GetInput().PointerUpEvent(iPointerID, fltOneX / fltWidth, fltOneY / fltHeight);
            break;
        
        case AMOTION_EVENT_ACTION_MOVE:
            {
                int iHistorySize = AMotionEvent_getHistorySize(event);
                int iPointerCount = AMotionEvent_getPointerCount(event);
                for (int iHistoryIndx = 0; iHistoryIndx < iHistorySize; iHistoryIndx++)
                {
                    for (int iPointerIndx = 0; iPointerIndx < iPointerCount; iPointerIndx++)
                    {
                        iPointerID = AMotionEvent_getPointerId(event, iPointerIndx);
                        fltOneX = AMotionEvent_getHistoricalX(event, iPointerIndx, iHistoryIndx);
                        fltOneY = AMotionEvent_getHistoricalY(event, iPointerIndx, iHistoryIndx);
                        // LogInfo("    PointerID %d => (%0.2f, %0.2f)", iPointerID, fltOneX, fltOneY);
                    }
                }
        
                for (int iPointerIndx = 0; iPointerIndx < iPointerCount; iPointerIndx++)
                {
                    iPointerID = AMotionEvent_getPointerId(event, iPointerIndx);
                    fltOneX = AMotionEvent_getX(event, iPointerIndx);
                    fltOneY = AMotionEvent_getY(event, iPointerIndx);
                    // LogInfo("    PointerID %d => (%0.2f, %0.2f)", iPointerID, fltOneX, fltOneY);
                    pFrmApp->GetInput().PointerMoveEvent(iPointerID, fltOneX / fltWidth, fltOneY / fltHeight);
                }
        
            }
        break;
        
        case AMOTION_EVENT_ACTION_CANCEL:
            LOGI("AMOTION_EVENT_ACTION_CANCEL: 0x%x", iPointerID);
            break;
        
        case AMOTION_EVENT_ACTION_OUTSIDE:
            LOGI("AMOTION_EVENT_ACTION_OUTSIDE: 0x%x", iPointerID);
            break;
        }
        
        return 1;
    }

    return 0;
}

void PauseVR(AioApplication* pApp)
{
    gIsPaused = true;
    if(gSvrInitialized)
    {
        EndVR(pApp, true);
    }
}

void EndVR(AioApplication* pApp, bool need_exit)
{
    if( gSvrInitialized )
    {
        LOGI("SVR_ENDVR");
        svrEndVr();
        pApp->Shutdown();
        gSvrInitialized = false;
        // destory app.
        if (need_exit) {
            exit(0);
        }
    }
}

void StartVR(AioApplication* pApp)
{
    //Could be called again, if native window changes. End previous start first.
    EndVR(pApp, false);

    SvrApplicationContext& appContext = pApp->GetApplicationContext();
    
    float fltWidth = (float)ANativeWindow_getWidth(appContext.nativeWindow);
    float fltHeight = (float)ANativeWindow_getHeight(appContext.nativeWindow);
    
    // Only start in Landscape Mode
    // Later: With the tethered device we need to start in "portrait" mode
    if( true || fltWidth >= fltHeight )
    {
        LOGI("APP_INITIALIZE");
        pApp->Initialize();
        
        SvrApplicationContext& appContext = pApp->GetApplicationContext();

        svrSetTrackingMode(appContext.trackingMode);

        svrBeginParams beginParams;
        beginParams.cpuPerfLevel = appContext.cpuPerfLevel;
        beginParams.gpuPerfLevel = appContext.gpuPerfLevel;
        beginParams.nativeWindow = appContext.nativeWindow;

        beginParams.optionFlags = 0;
        if (appContext.isProtectedContent)
            beginParams.optionFlags |= kProtectedContent;
        
        beginParams.mainThreadId = gettid();
        beginParams.colorSpace =  appContext.isColorSpaceSRGB ? kColorSpaceSRGB : kColorSpaceLinear;

        LOGI("SVR_BEGINVR");
        svrBeginVr(&beginParams);

        gSvrInitialized = true;
        gNeedsReInitialization = false;
    } 
}

void android_main(android_app *pAppState)
{
    LOGI("Svr Application (c) 2015 QUALCOMM Incorporated, Built: %s %s\n\n", __DATE__, __TIME__);

    pthread_setname_np(pthread_self(), "SVREye");

    AioApplication* pApp = new AioApplication();

    LOGI("App starting...");
    LOGI("Thread ID : %d", gettid());

    ALooper* looper = ALooper_forThread();
    if (looper == NULL)
    {
        LOGI("Looper was null, preparing new one");
        looper = ALooper_prepare(0);
    }
    else
    {
        LOGI("Looper %p", looper);
    }

    svrInitParams initParams;
    initParams.javaVm = pAppState->activity->vm;
    (*pAppState->activity->vm).AttachCurrentThread(&initParams.javaEnv, NULL);
    initParams.javaActivityObject = pAppState->activity->clazz;

    if ( svrInitialize(&initParams) != SVR_ERROR_NONE)
    {
        // Can't exit here since it yanks the rug out from the dialog box telling user SVR not supported :)
        //LOGE("android_main: svrInitialize failed, appication exiting");
        //return;
    }

    pAppState->userData = pApp;
    pAppState->onAppCmd = CommandCallback;
    pAppState->onInputEvent = InputCallback;

    SvrApplicationContext& appContext = pApp->GetApplicationContext();
    appContext.activity = pAppState->activity;
    appContext.assetManager = pAppState->activity->assetManager;
    appContext.externalPath = (char *)pAppState->activity->externalDataPath;
    pApp->LoadConfiguration();

    while (1)
    {
//        PROFILE_SCOPE(GROUP_WORLDRENDER, 0, "Eye Buffer Render");

        int id;
        int events;
        android_poll_source* pSource;

        // Loop until all events are read
        while ((id = ALooper_pollAll((gIsPaused)?-1:0, NULL, &events, (void**)&pSource)) >= 0)
        {
            // Process this event.
            if (pSource != NULL)
            {
                pSource->process(pAppState, pSource);
            }

            // Check if we are exiting.
            if (pAppState->destroyRequested != 0)
            {
                pAppState->activity->vm->DetachCurrentThread();
                exit(0);    // Need to exit or some stuff is not cleaned up and app will hang on re-launch
                return;
            }
        }

        if(!gIsPaused)
        {
            if( gRecreateContext )
            {
                createContext(pAppState);
            }
            if (gContextCreated)
            {
                if( (gNeedsReInitialization == true) || (gSvrInitialized == false) )
                {
                    {
                        StartVR(pApp);
                    }
                }

                SvrApplicationContext& appContext = pApp->GetApplicationContext();

                if (gSvrInitialized)
                {
                    pApp->Update();
                    pApp->Render();
                }

                EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
                eglSwapBuffers(appContext.display, surface);

                appContext.frameCount++;
            }
        }
    }
}
