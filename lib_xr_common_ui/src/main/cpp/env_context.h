//
// Created by fcx@pingxingyun.com on 2019/12/1.
//

#ifndef CLOUDLARKVRDEMO_ENV_CONTEXT_H
#define CLOUDLARKVRDEMO_ENV_CONTEXT_H

#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_activity.h>
#include <android/bitmap.h>
#include <memory>
#include "bitmap_factory.h"
#include "asset_file.h"

class Context;
class EnvWrapper {
private:
    JavaVM* vm_;
    JNIEnv* env_;
    bool need_attach_;

public:
    EnvWrapper(JavaVM * vm, JNIEnv * env, bool needAttach);

    ~EnvWrapper();

    inline JNIEnv * get() {
        return env_;
    }
};

class Context
{
    static std::unique_ptr<Context> s_instance_;
public:
    inline static Context * instance() {
        return s_instance_.get();
    }
    static void Init(JavaVM* vm, JNIEnv * env, jobject assetManagerInstance, jobject mainActivityInstance,
                     const char * internalDataPath, const char * externalDataPath);
    static void Init(JavaVM* vm, JNIEnv * env, ANativeActivity * activityNative, AAssetManager * assetManager,
            const char * internalDataPath, const char * externalDataPath);
    static void Init(ANativeActivity * activityNative);
    static void Reset();

    ~Context();

    EnvWrapper GetEnv();

    inline JavaVM* vm() { return vm_; };

    inline AAssetManager* asset_manager() {
        return asset_manager_;
    }

    inline lark::BitmapFactory * bitmap_factory() {
        return bitmap_factory_.get();
    }

    inline ANativeActivity* native_activity() {
        return native_activity_;
    }

    const char* internal_data_path() { return internal_data_path_; }
    const char* external_data_path() { return external_data_path_; }
private:
    // private constructor.
    Context(JavaVM* vm, JNIEnv * env, jobject assetManagerInstance, jobject mMainActivityInstance,
            const char * internalDataPath, const char * externalDataPath);

    Context(JavaVM* vm, JNIEnv * env, ANativeActivity * activityNative, AAssetManager * assetManager, const char * internalDataPath, const char * externalDataPath);

    Context(ANativeActivity * activityNative);
    ANativeActivity * native_activity_;
    JavaVM * vm_;

    AAssetManager* asset_manager_;

    std::shared_ptr<lark::BitmapFactory> bitmap_factory_;

    char internal_data_path_[100]{};
    char external_data_path_[100]{};
};
#endif //CLOUDLARKVRDEMO_ENV_CONTEXT_H
