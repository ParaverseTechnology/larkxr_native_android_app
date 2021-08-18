//
// Created by fcx on 2020/4/10.
//

#ifndef CLOUDLARKXR_ASSET_FILE_H
#define CLOUDLARKXR_ASSET_FILE_H


#ifdef __ANDROID__
#include <android/asset_manager.h>

namespace lark {
class AssetFile {
public:
    AssetFile(AAssetManager * assetManager, const char * assetPath);

    ~AssetFile();

    bool Open();

    void Close();

    const void * GetBuffer();

    size_t GetLength();

    char * ToString();

private:
    AAssetManager * asset_manager_;
    const char * path_;
    AAsset * asset_;
};
}
#endif

#endif //CLOUDLARKXR_ASSET_FILE_H
