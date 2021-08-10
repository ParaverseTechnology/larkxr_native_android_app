//
// Created by fcx on 2020/4/10.
//

#include "asset_file.h"
#include "logger.h"
#include <iostream>
#ifdef __ANDROID__

namespace lark {
AssetFile::AssetFile(AAssetManager *assetManager, const char *assetPath):
        asset_manager_(assetManager),
        path_(assetPath),
        asset_(nullptr) {
}

AssetFile::~AssetFile() {
    Close();
}

bool AssetFile::Open() {
    if (asset_ != nullptr)
        return true;

    if (path_ == nullptr) {
        LOGE("File Path is NULL");
        return false;
    }

    if (asset_manager_ == nullptr) {
        LOGE("AssetManager is NULL");
        return false;
    }

    asset_ = AAssetManager_open(asset_manager_, path_, AASSET_MODE_UNKNOWN);
    if (asset_ == nullptr) {
        LOGE("Open file failed: %s", path_);
        return false;
    }

    return true;
}

void AssetFile::Close() {
    if (asset_ == nullptr)
        return;
    AAsset_close(asset_);
    asset_ = nullptr;
}

const void *AssetFile::GetBuffer() {
    if (asset_ == nullptr)
        return nullptr;
    return AAsset_getBuffer(asset_);
}

size_t AssetFile::GetLength() {
    if (asset_ == nullptr)
        return 0;
    return AAsset_getLength(asset_);
}

char *AssetFile::ToString() {
    if (asset_ == nullptr)
        return nullptr;
    const void * buffer = AAsset_getBuffer(asset_);
    const size_t N = AAsset_getLength(asset_);
    if (N == 0)
        return nullptr;
    char * dup = new char [N + 1];
    memcpy(dup, buffer, N);
    dup[N] = 0;
    return dup;
}
}


#endif
