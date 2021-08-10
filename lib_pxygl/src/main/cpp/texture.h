//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#ifndef MY_APPLICATION_TEXTURE_H
#define MY_APPLICATION_TEXTURE_H

#include "pxygl.h"
#include <string>

#ifdef __ANDROID__
#include <android/bitmap.h>
#include <android/asset_manager.h>
#include "bitmap_factory.h"
#endif

namespace lark {
class CLOUDLARK_PXYGL_API Texture {
private:
    static uint64_t objcet_count_;
public:
    ~Texture();

public:
    static Texture* GenTexture(const std::string& path);
#ifdef __ANDROID__
    static void SetTextureInfo(Texture * texture, AndroidBitmapInfo & info);
#endif
    
    // TODO
    static Texture* LoadNetTexture(const char * url);
    // TODO
    static Texture* LoadNetTexture(const char * url, GLuint textureId);
    // load texture with given data.
    static Texture* LoadTexture(const uint8_t* data, GLuint format, int width, int height, int stride);

#ifdef __ANDROID__
    static Texture* LoadTexture(AAssetManager* assetManager, BitmapFactory* bitmapFactory, JNIEnv* env, const char* assetFile);
    static Texture* LoadTexture(AAssetManager* assetManager, BitmapFactory* bitmapFactory, JNIEnv* env, const char* assetFile, GLuint textureId);
    static Texture* LoadTexture(BitmapFactory* bitmapFactory, JNIEnv* env, const char* buffer, int bufferLen, GLuint textureId);
    static Texture* LoadSkyboxTexture(AAssetManager* assetManager, BitmapFactory* bitmapFactory, JNIEnv* env, const char * assetFile);
#endif
    static Texture* LoadTextureFromData(const char* data, size_t data_size);
    static Texture* LoadTexture(const char * assetFile);
    static Texture* LoadTexture(const char * assetFile, GLuint textureId);
    static Texture* SetupImageData(unsigned char* data, int width, int height, int channel, GLuint textureId);
    static Texture* LoadSkyboxTexture(const char * assetFile);
    static Texture* SetupSkyboxTexture(Texture* texture);

    static uint8_t * CropBitmap(const uint8_t * origBitmap, const size_t origW, const size_t origH, const size_t x, const size_t y, const size_t w, const size_t h);

    void SetupBitmap(const uint8_t* data, GLuint format, int width, int height, int stride);
    void SetupBitmap();

    inline void BindBitmap(int internalFormat = -1) {
        if (internalFormat != -1)
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, format_, type_, bitmap_);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, format_, type_, bitmap_);
    }

    void CleanBitmap();

    inline void BindTexture() {
        glBindTexture(GL_TEXTURE_2D, texture_);
    }

    inline void UnBindTexture() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    inline void BindTextureCubeMap() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
    }

    inline void UnbindTextureCubeMap() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    inline size_t width() {
        return width_;
    }

    inline size_t height() {
        return height_;
    }

    inline GLuint texture() {
        return texture_;
    }

    inline size_t format() {
        return format_;
    }

    inline void set_format(size_t format) {
        format_ = format;
    }

    inline size_t type() {
        return type_;
    }

    inline uint8_t* bitmap() {
        return bitmap_;
    }

    inline const std::string & type_name() {
        return type_name_;
    }

    inline void set_type_name(const std::string& typeName) {
        type_name_ = typeName;
    }

    inline uint64_t id() { return id_; }
private:
    Texture(const std::string& path);

    void Clear();
    void set_texture(int texture);

    GLuint texture_;
    uint8_t* bitmap_;
    size_t width_;
    size_t height_;
    size_t stride_;
    size_t size_;
    size_t type_;
    size_t format_;
    std::string type_name_;
    uint64_t id_;
    std::string path_;
};
}

#endif //MY_APPLICATION_TEXTURE_H
