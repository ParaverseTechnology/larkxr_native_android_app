//
// Created by Administrator on 2021/7/7.
//
#include <jni.h>
#include <string>
#include <log.h>
#include <env_context.h>
#include <build_config.h>

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1openxr_1pico_MainActivity_nativeInit(JNIEnv *env, jobject thiz,
        jobject asset_manager, jstring internal_data_path_,
        jstring external_data_path_) {
    const char *internalDataPath = env->GetStringUTFChars(internal_data_path_, 0);
    const char *externalDataPath = env->GetStringUTFChars(external_data_path_, 0);

    LOGI("MainActivity_init: call  Context::getInstance()->init");
    JavaVM* vm = nullptr;
    env->GetJavaVM(&vm);
    Context::Init(vm, env, asset_manager, thiz, internalDataPath, externalDataPath);
    LOGI("register WVR main when library loading");

    env->ReleaseStringUTFChars(internal_data_path_, internalDataPath);
    env->ReleaseStringUTFChars(external_data_path_, externalDataPath);

    BuildConfig::init(env, "com/pxy/larkxr_openxr_pico/BuildConfig");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_larkxr_1openxr_1pico_MainActivity_nativeDestory(JNIEnv *env, jobject thiz) {
    Context::Reset();
}