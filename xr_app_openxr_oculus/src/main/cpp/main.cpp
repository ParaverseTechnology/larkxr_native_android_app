/************************************************************************************

Filename  : XrCompositor_NativeActivity.c
Content   : This sample uses the Android NativeActivity class.
Created   :
Authors   :

Copyright : Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

*************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android_native_app_glue.h>
#include <assert.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <openxr/openxr_oculus_helpers.h>
#include <asset_loader.h>
#include <asset_files.h>
#include <build_config.h>
#include "openxr_context.h"
#include "xr_scene_local.h"
#include "skybox.h"
#include "env_context.h"
#include "oxr_application.h"

struct AndroidAppState {
    oxr::OpenxrContext* context = nullptr;
    oxr::OxrApplication* application = nullptr;
};

/**
 * Process the next main command.
 */
static void app_handle_cmd(struct android_app* app, int32_t cmd) {
    auto* appState = (AndroidAppState*)app->userData;

    switch (cmd) {
        // There is no APP_CMD_CREATE. The ANativeActivity creates the
        // application thread from onCreate(). The application thread
        // then calls android_main().
        case APP_CMD_START: {
            ALOGV("onStart()");
            ALOGV("    APP_CMD_START");
            break;
        }
        case APP_CMD_RESUME: {
            ALOGV("onResume()");
            ALOGV("    APP_CMD_RESUME");
            appState->context->set_resumed(true);
            if (appState->application) {
                appState->application->OnResume();
            }
            break;
        }
        case APP_CMD_PAUSE: {
            ALOGV("onPause()");
            ALOGV("    APP_CMD_PAUSE");
            appState->context->set_resumed(false);
            if (appState->application) {
                appState->application->OnPause();
            }
            break;
        }
        case APP_CMD_STOP: {
            ALOGV("onStop()");
            ALOGV("    APP_CMD_STOP");
            break;
        }
        case APP_CMD_DESTROY: {
            ALOGV("onDestroy()");
            ALOGV("    APP_CMD_DESTROY");
            appState->context->set_native_window(nullptr);
            break;
        }
        case APP_CMD_INIT_WINDOW: {
            ALOGV("surfaceCreated()");
            ALOGV("    APP_CMD_INIT_WINDOW");
            appState->context->set_native_window(app->window);
            break;
        }
        case APP_CMD_TERM_WINDOW: {
            ALOGV("surfaceDestroyed()");
            ALOGV("    APP_CMD_TERM_WINDOW");
            appState->context->set_native_window(nullptr);
            break;
        }
        default:
            ALOGV("UNKNOWN APP cmd[%d]", cmd);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app) {
    ALOGV("----------------------------------------------------------------");
    ALOGV("android_app_entry()");
    ALOGV("    android_main()");

    JNIEnv* Env;
    (*app->activity->vm).AttachCurrentThread(&Env, nullptr);

    // Note that AttachCurrentThread will reset the thread name.
    prctl(PR_SET_NAME, (long)"Lark_OXR::Main", 0, 0, 0);

    // 初始化环境。
    Context::Init(app->activity);

    PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR;
    xrGetInstanceProcAddr(
            XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction*)&xrInitializeLoaderKHR);
    if (xrInitializeLoaderKHR != nullptr) {
        XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid;
        memset(&loaderInitializeInfoAndroid, 0, sizeof(loaderInitializeInfoAndroid));
        loaderInitializeInfoAndroid.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
        loaderInitializeInfoAndroid.next = nullptr;
        loaderInitializeInfoAndroid.applicationVM = app->activity->vm;
        loaderInitializeInfoAndroid.applicationContext = app->activity->clazz;
        xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&loaderInitializeInfoAndroid);
    }

    oxr::OpenxrContext context = {};

    context.CreateInstance();
    context.InitializeSystem();
    context.InitializeSession();
    context.CreateSwapchains();

    oxr::OxrApplication application;
    application.InitGL(&context);

    AndroidAppState appState = { &context, &application };
    app->userData = &appState;
    app->onAppCmd = app_handle_cmd;

    while (app->destroyRequested == 0) {
        // Read all pending events.
        for (;;) {
            int events;
            struct android_poll_source* source;
            // If the timeout is zero, returns immediately without blocking.
            // If the timeout is negative, waits indefinitely until an event appears.
            const int timeoutMilliseconds =
                    (!context.resumed() && !context.session_active() && app->destroyRequested == 0) ? -1 : 0;
            if (ALooper_pollAll(timeoutMilliseconds, nullptr, &events, (void**)&source) < 0) {
                break;
            }

            // Process this event.
            if (source != nullptr) {
                source->process(app, source);
            }
        }

        context.PollEvents();

        if (!context.session_active()) {
            continue;
        }

        application.Update();
        application.RenderFrame();
    }

    application.ShutdownGL();

    Context::Reset();

    context.Destory();

    (*app->activity->vm).DetachCurrentThread();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1openxr_1oculus_MainActivity_nativeInit(JNIEnv *env, jobject thiz) {
    BuildConfig::init(env, "com/pxy/larkxr_openxr_oculus/BuildConfig");
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1openxr_1oculus_MainActivity_nativeNetworkAvaliable(JNIEnv *env, jobject thiz) {
    Application* application = Application::instance();
    if (application != nullptr) {
        application->OnNetworkAvailable();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1openxr_1oculus_MainActivity_nativeNetworkLost(JNIEnv *env, jobject thiz) {
    Application* application = Application::instance();
    if (application != nullptr) {
        application->OnNetworkLost();
    }
}