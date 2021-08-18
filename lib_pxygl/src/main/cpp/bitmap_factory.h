//
// Created by fcx on 2020/4/10.
//

#ifndef CLOUDLARKXR_BITMAP_FACTORY_H
#define CLOUDLARKXR_BITMAP_FACTORY_H

#ifdef __ANDROID__
#include <jni.h>
#include <android/bitmap.h>

namespace lark {
class BitmapFactory {
public:
    BitmapFactory(JNIEnv * env);
    void Clean(JNIEnv * env);

    /**
     * Put the source file byte array, and its size in byte.  And return the
     * decoded void* bitmap array.  You can retrive bitmap info from outputInfo.
     * Remember to delete returned array.
    **/
    uint8_t * DecodeByteArray(JNIEnv * env, const void * array, size_t size, AndroidBitmapInfo & outputInfo);
    /**
     *
     */
    uint8_t *  DecodeBitmap(JNIEnv * env, jobject jBitmap, AndroidBitmapInfo & outputInfo);

private:
    void RecycleBitmap(JNIEnv *env, jobject bitmap);
    uint8_t * DecodeAndroidBitmap(JNIEnv * env, jobject jBitmap, AndroidBitmapInfo & outputInfo);

    jclass bitmap_factory_class_;
    jmethodID id_decord_byte_array_;

};
}
#endif

#endif //CLOUDLARKXR_BITMAP_FACTORY_H
