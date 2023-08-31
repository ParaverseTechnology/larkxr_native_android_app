//
// Created by fcx@pingxingyung.com on 2023/8/21.
// Copyright (c) 2023 www.pingxingyun.com All rights reserved.
//


#include <jni.h>
#include <android/native_window_jni.h>
#include "hxr_application.h"
#include "openxr_context.h"
#include "build_config.h"
#include "env_context.h"
#include "log.h"

enum AppState{
    ON_RESUME,
    ON_PAUSE,
    ON_DESTROY
};

// global obj
hxr::HxrApplication* g_app = nullptr;
hxr::OpenxrContext* g_openxr_context = nullptr;
pthread_t g_thread_id;
//Native window surface
ANativeWindow* g_native_window;
jobject g_activity_context = nullptr;

// global state
AppState g_app_state = ON_RESUME;
bool sessionRunning = false;
bool stateBegeinSession = false;
bool stateEndSession = false;

// render thread.
void* threadStartCallback(void* data) {
    JNIEnv* Env;
    Context::instance()->vm()->AttachCurrentThread(&Env, nullptr);

    // main loop
    g_openxr_context->CreateInstance(g_activity_context,
                                     Context::instance()->vm());
    g_openxr_context->InitializeSystem();

    g_openxr_context->InitializeSession(g_native_window);

    g_openxr_context->CreateSwapchains();

    g_app->InitGL(g_openxr_context);

    while (true)
    {
        bool exitRenderLoop = false;
        bool sessionStateStopping = false;
        bool sessionStateExiting = false;
        g_openxr_context->PollEvents(&exitRenderLoop, &sessionRunning, &sessionStateStopping, &sessionStateExiting);

        if (sessionStateStopping) {
            stateEndSession = true;
        }

        if(g_app_state == ON_DESTROY && sessionStateExiting){
            LOGI("sessionStateExiting destroy");
            exitRenderLoop = true;
        }else { //destroy
            LOGI("sessionStateExiting g_AppState: %d", g_app_state);
            exitRenderLoop = false;
        }

        if(stateBegeinSession == true && stateEndSession == true)
        {
            g_openxr_context->graphics_plugin().InitEGLSurface(g_native_window);
            LOGI("stateBegeinSession == true && stateEndSession == true");
            g_openxr_context->BeginSession();
            sessionRunning = true;
            stateBegeinSession = false;
            stateEndSession = false;
        }
        if (exitRenderLoop)
        {
            LOGI("exitRenderLoop");
            break;
        }

        //LOGI("sessionRunning : %d",sessionRunning);
        if (sessionRunning)
        {
            g_app->Update();
            g_app->RenderFrame();

            //mRender = 1;
        }
        else
        {
            LOGI("session not Running");
            // Throttle loop since xrWaitFrame won't be called.
            timespec total_time;
            timespec left_time;
            total_time.tv_sec = 0;
            total_time.tv_nsec = (long)(250000000) ;
            nanosleep(&total_time, &left_time);
        }
    }

    g_app->OnPause();
    g_openxr_context->Destory();

    pthread_exit(nullptr);
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_xr_1app_1huawei_MainActivity_nativeInit(JNIEnv *env, jclass clazz, jint width,
jint height, jint tex, jobject act,
        jobject sf, jobject am, jstring internal_data_path_, jstring external_data_path_) {

    if (g_app == nullptr) {
        g_activity_context = env->NewGlobalRef(act);

        const char *internalDataPath = env->GetStringUTFChars(internal_data_path_, 0);
        const char *externalDataPath = env->GetStringUTFChars(external_data_path_, 0);

        LOGI("MainActivity_init: call  Context::getInstance()->init");
        JavaVM* vm = nullptr;
        env->GetJavaVM(&vm);
        Context::Init(vm, env, am, g_activity_context, internalDataPath, externalDataPath);
        LOGI("register WVR main when library loading");

        env->ReleaseStringUTFChars(internal_data_path_, internalDataPath);
        env->ReleaseStringUTFChars(external_data_path_, externalDataPath);

        BuildConfig::init(env, "com/pxy/xr_app_huawei/BuildConfig");

        g_openxr_context = new hxr::OpenxrContext();
        g_app = new hxr::HxrApplication();

        g_native_window = ANativeWindow_fromSurface(env, sf);

        pthread_create(&g_thread_id, nullptr, threadStartCallback, nullptr);
    } else {
        g_app_state = ON_RESUME;
        g_native_window = ANativeWindow_fromSurface(env, sf);
        stateBegeinSession = true;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_xr_1app_1huawei_MainActivity_uninit(JNIEnv *env, jclass clazz) {
    LOGI("in uninit");
    if(g_app == nullptr){
        LOGI("gApp == nullptr");
        return;
    }
    g_app_state = ON_PAUSE;
    g_app->OnPause();
    xrRequestExitSession(g_openxr_context->session());
    LOGI("out uninit");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_xr_1app_1huawei_MainActivity_nativeDestroy(JNIEnv *env, jclass clazz) {
    LOGI("vr nativeOnDestroy");
    g_app_state = ON_DESTROY;
    if (nullptr != g_app)
    {
        pthread_join(g_thread_id, nullptr);

        delete g_app;
        g_app = nullptr;

        delete g_openxr_context;
        g_openxr_context = nullptr;
    }

    if (g_activity_context != nullptr) {
        env->DeleteGlobalRef(g_activity_context);
        g_activity_context = nullptr;
    }


    LOGI("uninit vr nativeOnDestroy end");
}