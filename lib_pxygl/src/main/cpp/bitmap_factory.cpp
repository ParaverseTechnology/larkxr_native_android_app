//
// Created by fcx on 2020/4/10.
//

#include "bitmap_factory.h"
#include "logger.h"
#include <memory>

#ifdef __ANDROID__

namespace lark {
BitmapFactory::BitmapFactory(JNIEnv *env) {
    LOGV("BitmapFactory %p", env);
    if (env == nullptr) {
        LOGV("BitmapFactory cant init whit null env");
        return;
    }
    const char * BitmapFactoryClassName = "android/graphics/BitmapFactory";
    jclass localClazz = env->FindClass(BitmapFactoryClassName);
    if (localClazz == nullptr) {
        LOGE("Unable to find class %s", BitmapFactoryClassName);
        return;
    }

    bitmap_factory_class_ = reinterpret_cast<jclass>(env->NewGlobalRef(localClazz));
    id_decord_byte_array_ = env->GetStaticMethodID(bitmap_factory_class_, "decodeByteArray", "([BII)Landroid/graphics/Bitmap;");
}

void BitmapFactory::Clean(JNIEnv *env) {
    if (bitmap_factory_class_)
        env->DeleteGlobalRef(bitmap_factory_class_);
}

uint8_t *BitmapFactory::DecodeByteArray(JNIEnv *env, const void *array, size_t size,
                                        AndroidBitmapInfo &outputInfo) {
    jbyteArray jarray = env->NewByteArray(size);
    env->SetByteArrayRegion(jarray, 0, size, (jbyte *) array);

    jobject jBitmap = env->CallStaticObjectMethod(bitmap_factory_class_, id_decord_byte_array_, jarray, 0, size);
    uint8_t * pixels = nullptr;
    if (jBitmap == nullptr) {
        LOGE("Unable to decode");
    } else {
        pixels = DecodeAndroidBitmap(env, jBitmap, outputInfo);
        RecycleBitmap(env, jBitmap);
        env->DeleteLocalRef(jBitmap);
    }
    env->DeleteLocalRef(jarray);
    return pixels;
}

uint8_t *
BitmapFactory::DecodeBitmap(JNIEnv *env, jobject jBitmap, AndroidBitmapInfo &outputInfo) {
    uint8_t * pixels = nullptr;
    if (jBitmap == nullptr) {
        LOGE("Unable to decode");
    } else {
        pixels = DecodeAndroidBitmap(env, jBitmap, outputInfo);
        RecycleBitmap(env, jBitmap);
    }
    return pixels;
}

void BitmapFactory::RecycleBitmap(JNIEnv *env, jobject bitmap) {
    jclass clsBmp = env->GetObjectClass(bitmap);
    jmethodID recycleMethod = env->GetMethodID(clsBmp, "recycle", "()V");
    env->CallVoidMethod(bitmap, recycleMethod);
    env->DeleteLocalRef(clsBmp);
}

uint8_t *BitmapFactory::DecodeAndroidBitmap(JNIEnv *env, jobject jBitmap,
                                            AndroidBitmapInfo &outputInfo) {
    int ret = -1;
    AndroidBitmapInfo info;
    if ((ret = AndroidBitmap_getInfo(env, jBitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return nullptr;
    }
    // Read Bitmap pixels
    uint8_t * bitmapPixels = nullptr;
    if ((ret = AndroidBitmap_lockPixels(env, jBitmap, (void**)&bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return nullptr;
    }
    outputInfo = info;
    const size_t bmpSize = info.stride * info.height;
    auto bitmap = new uint8_t [bmpSize];
    memcpy(bitmap, bitmapPixels, bmpSize);
    AndroidBitmap_unlockPixels(env, jBitmap);
    return bitmap;
}
}
#endif
