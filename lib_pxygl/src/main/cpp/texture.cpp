//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#include "texture.h"
#ifdef __ANDROID__
//#include "env_context.h"
#include <android/bitmap.h>
#include <GLES2/gl2.h>
#include "asset_file.h"
#endif // __ANDROID__

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // ! STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "logger.h"

#define LOG_TAG "pxygl_Texture"

namespace lark {
uint64_t Texture::objcet_count_ = 0;
Texture::Texture(const std::string& path) :
        texture_(0),
        bitmap_(nullptr),
        width_(0),
        height_(0),
        stride_(0),
        size_(0) ,
        type_name_(""),
        type_(),
        format_(),
        id_(++objcet_count_),
        path_(path) {
//    LOGV_F("create texture id %?d texture %?d %s", id_, texture_, path_);
}

Texture::~Texture() {
//    LOGV_F("release texture id %?d texture %?d %s", id_, texture_, path_);
    Clear();
}

void Texture::Clear() {
    if (texture_ != 0) {
        glDeleteTextures(1, &texture_);
        texture_ = 0;
    }
    CleanBitmap();
}

void Texture::set_texture(int texture) {
    Clear();
    texture_ = texture;
}

void Texture::CleanBitmap()
{
    if (bitmap_ != NULL)
        stbi_image_free(bitmap_);
    bitmap_ = NULL;
}

Texture * Texture::GenTexture(const std::string& path) {
    GLuint texture = 0;
    glGenTextures(1, &texture);

    auto * m = new Texture(path);
    m->set_texture(texture);
    return m;
}

#ifdef __ANDROID__
void Texture::SetTextureInfo(Texture * texture, AndroidBitmapInfo & info) {
    texture->width_ = info.width;
    texture->height_ = info.height;
    texture->stride_ = info.stride;
    texture->size_ = info.stride * info.height;
    switch (info.format) {
        case ANDROID_BITMAP_FORMAT_RGB_565:
            texture->type_ = GL_UNSIGNED_SHORT_5_6_5;
            texture->format_ = GL_RGB565;
            break;
        case ANDROID_BITMAP_FORMAT_RGBA_4444:
            texture->type_ = GL_UNSIGNED_SHORT_4_4_4_4;
            texture->format_ = GL_RGBA4;
            break;
        case ANDROID_BITMAP_FORMAT_RGBA_8888:
            texture->type_ = GL_UNSIGNED_BYTE;
            texture->format_ = GL_RGBA;
            break;
        case ANDROID_BITMAP_FORMAT_A_8:
            texture->type_ = GL_UNSIGNED_BYTE;
            texture->format_ = GL_RED;
            break;
        default:
            texture->type_ = GL_UNSIGNED_BYTE;
            texture->format_ = GL_RGBA;
            break;
    }
    // test srgb
//    texture->format_ = GL_SRGB;
}
Texture *Texture::LoadTexture(AAssetManager* assetManager, BitmapFactory* bitmapFactory, JNIEnv *env, const char *assetFile) {
    return LoadTexture(assetManager, bitmapFactory, env, assetFile, 0);
}

Texture *
Texture::LoadTexture(AAssetManager* assetManager, BitmapFactory* bitmapFactory, JNIEnv *env, const char *assetFile, GLuint textureId) {
    auto * textureFile = new AssetFile(assetManager, assetFile);
    if (!textureFile->Open())
        return nullptr;
    const void * data = textureFile->GetBuffer();
    size_t length = textureFile->GetLength();

    AndroidBitmapInfo info;
    uint8_t * bmp = bitmapFactory->DecodeByteArray(env, data, length, info);
    if (bmp == nullptr)
        return nullptr;

    Texture * texture = nullptr;
    if (textureId != 0) {
        texture = new Texture(assetFile);
        texture->set_texture(textureId);
    } else {
        texture = GenTexture(assetFile);
    }
    texture->bitmap_ = bmp;
    SetTextureInfo(texture, info);

    delete textureFile;
    return texture;
}

Texture *
Texture::LoadTexture(BitmapFactory *bitmapFactory, JNIEnv *env, const char *buffer, int bufferLen, GLuint textureId) {
    LOGV("LoadTexture1 %d", bufferLen);
    AndroidBitmapInfo info;
    uint8_t * bmp = bitmapFactory->DecodeByteArray(env, buffer, bufferLen, info);
    if (bmp == nullptr)
        return nullptr;
    LOGV("LoadTexture2 %d", bufferLen);
    Texture * texture = nullptr;
    if (textureId != 0) {
        texture = new Texture("");
        texture->set_texture(textureId);
    } else {
        texture = GenTexture("");
    }
    texture->bitmap_ = bmp;
    SetTextureInfo(texture, info);
    LOGV("LoadTexture3 %d", bufferLen);
    return texture;
}
#endif // __ANDROID__

Texture* Texture::LoadNetTexture(const char * url) {
    return LoadNetTexture(url, 0);
}

Texture* Texture::LoadNetTexture(const char *url, GLuint textureId) {
    // TODO
    return nullptr;
}

Texture* Texture::LoadTexture(const uint8_t* data, GLuint format, int width, int height, int stride)
{
    Texture* texture = GenTexture("");
    texture->width_ = width;
    texture->height_ = height;
    texture->stride_ = stride;
    texture->type_ = GL_UNSIGNED_BYTE;
    texture->format_ = format;
    texture->BindTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture->UnBindTexture();
    return texture;
}

Texture* Texture::LoadTextureFromData(const char* data, size_t data_size)
{
    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory((stbi_uc const *)data, data_size, &width, &height, &channels, 0);
    if (image_data == nullptr) {
        LOGW("stb load from data failed. %?d", data_size);
        return nullptr;
    }
    return SetupImageData(image_data, width, height, channels, 0);
}

Texture* Texture::LoadTexture(const char* assetFile) {
    return LoadTexture(assetFile, 0);
}

Texture* Texture::LoadTexture(const char *assetFile, GLuint textureId) {
    int width, height, channels;
    unsigned char* data = stbi_load(assetFile, &width, &height, &channels, 0);
    if (data == nullptr) {
        LOGW("stb load texture failed. %s", assetFile);
        return nullptr;
    }
    LOGV("stb load texture success. %s", assetFile);
    return SetupImageData(data, width, height, channels, textureId);
}

Texture* Texture::SetupImageData(unsigned char* data, int width, int height, int channels, GLuint textureId)
{
    Texture* texture = nullptr;
    if (textureId != 0) {
        texture = new Texture("");
        texture->set_texture(textureId);
    }
    else {
        texture = GenTexture("");
    }
    texture->bitmap_ = data;
    texture->width_ = width;
    texture->height_ = height;
    // strie = WIDTH X CHANNEL
    texture->stride_ = width * channels;
    // TODO channel 1 2
    // 1 ： 灰度图
    // 2 ： 灰度图加透明度
    // 3 ： 红绿蓝 RGB 三色图
    // 4 ： 红绿蓝加透明度 RGBA 图
    texture->type_ = GL_UNSIGNED_BYTE;
    switch (channels)
    {
    case 1:
        texture->format_ = GL_RED;
        break;
    case 3:
        texture->format_ = GL_RGB;
        break;
    case 4:
        texture->format_ = GL_RGBA;
        break;
    default:
        texture->format_ = GL_RGBA;
        break;
    }
    return texture;
}

uint8_t* Texture::CropBitmap(const uint8_t * origBitmap, const size_t origW, const size_t origH, const size_t x, const size_t y, const size_t w, const size_t h) {
    if (x >= origW || y >= origH) {
        LOGE("Croped image is fully out of bound.");
        return nullptr;
    }
    const size_t x2 = x + w;
    const size_t y2 = y + h;
    size_t xpadding = 0;
    if (x2 > origW || y2 > origH)
        LOGW("Pixel filled of croped image will be less than expected because out of bound.");
    if (x2 > origW)
        xpadding = x2 - origW;

    auto * croped = new uint8_t [w * h];
    uint8_t * ptr = croped;
    for (int j = y; j < origH && j < y2; j++) {
        const uint8_t * origPtr = origBitmap + x + origW * j;
        for (int i = x; i < origW && i < x2; i++)
            *ptr++ = *origPtr++;
        ptr += xpadding;
    }
    return croped;
}

void Texture::SetupBitmap(const uint8_t* data, GLuint format, int width, int height, int stride)
{
    if (texture_ == 0) {
        GLuint texture = 0;
        glGenTextures(1, &texture);
        set_texture(texture);
    }
    width_ = width;
    height_ = height;
    stride_ = stride;
    type_ = GL_UNSIGNED_BYTE;
    format_ = format;
    BindTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    UnBindTexture();
}

void Texture::SetupBitmap()
{
    BindTexture();
    BindBitmap(format_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    UnBindTexture();
    CleanBitmap();
}

Texture * Texture::LoadSkyboxTexture(const char * assetFile) {
    return SetupSkyboxTexture(LoadTexture(assetFile));
}

#ifdef __ANDROID__
Texture *Texture::LoadSkyboxTexture(AAssetManager* assetManager, BitmapFactory* bitmapFactory, JNIEnv* env, const char *assetFile) {
    return SetupSkyboxTexture(LoadTexture(assetManager, bitmapFactory, env, assetFile));
}
#endif

Texture *Texture::SetupSkyboxTexture(Texture *texture) {
    if (texture == nullptr)
        return nullptr;

    size_t stride = texture->stride_ / 4;
    size_t width = texture->width_ / 4;
    size_t height = texture->height_ / 3;
    if (((height * 3) != texture->height_) ||
        ((stride * 4) != texture->stride_)) {
        LOGW("May not a Skybox image.  Stop process");
        delete texture;
        return nullptr;
    }

    /**
     *    1
     *  4 5 6 7
     *    9
     *    GL_TEXTURE_CUBE_MAP_POSITIVE_X 0	Right
     *    GL_TEXTURE_CUBE_MAP_NEGATIVE_X 1	Left
     *    GL_TEXTURE_CUBE_MAP_POSITIVE_Y 2	Top
     *    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 3	Bottom
     *    GL_TEXTURE_CUBE_MAP_POSITIVE_Z 4	Back
     *    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 5	Front
     */
    const int index[] = {6, 4, 1, 9, 5, 7};
    const int faces[] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    texture->BindTextureCubeMap();

    for (int i = 0; i < 6; i++) {
        int x = stride * (index[i] % 4);
        int y = height * (index[i] / 4);
        uint8_t * bitmap = CropBitmap(texture->bitmap_, texture->stride_, texture->height_, x, y, stride, height);

        // Always output as GL_RGB5_A1 because the skybox don't need quality
        glTexImage2D(faces[i], 0, GL_RGB5_A1, width, height, 0, texture->format_, texture->type_, bitmap);

//        glTexImage2D(faces[i], 0, GL_RGBA, width, height, 0, texture->format_, texture->type_, bitmap);
        // test srgb
//        glTexImage2D(faces[i], 0, GL_SRGB, width, height, 0, texture->format_, texture->type_, bitmap);

        delete [] bitmap;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    texture->UnbindTextureCubeMap();
    texture->CleanBitmap();

    return texture;
}
}