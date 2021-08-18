//
// Created by fcx on 2020/6/5.
//

#include <fstream>
#include <sstream>
#include "asset_loader.h"
#include "logger.h"
#include "model.h"
#include "libgen.h"
#include <sys/stat.h>
#include <filesystem>

namespace lark {
AssetLoader* AssetLoader::instance_ = nullptr;

AssetLoader* AssetLoader::instance() {
    if (instance_ == nullptr) {
        instance_ = new AssetLoader();
    }
    return instance_;
}
void AssetLoader::Release() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

AssetLoader::AssetLoader() {
}

AssetLoader::~AssetLoader() {
    shader_map_.clear();
    texture_map_.clear();
    model_map_.clear();
}

#ifdef __ANDROID__
int mkpath(const std::string& path, mode_t mode)
{
    if (mkdir(path.c_str(), mode) == -1) {
        switch(errno) {
            case ENOENT:
                if (mkpath(path.substr(0, path.find_last_of('/')), mode) == -1)
                    return -1;
                else
                    return mkdir(path.c_str(), mode);
            case EEXIST:
                return 0;
            default:
                return -1;
        }
    }

    return 0;
}

bool CopyAssetToInternalPath(ANativeActivity* nativeActivity, const std::string& fileName) {
    std::string internal_path = std::string(nativeActivity->internalDataPath) + "/" + fileName;
    FILE *file = ::fopen(internal_path.c_str(), "rb");
    if (file != nullptr) {
        // skip when already exits.
        LOGV("file already here");
        return true;
    }
    // Open file
    AAsset* asset = AAssetManager_open(nativeActivity->assetManager, fileName.c_str(),
                                       AASSET_MODE_UNKNOWN);
    if (asset == nullptr) {
        // file not found.
        LOGW("open assets failed file not found %s", fileName.c_str());
        return false;
    }

    std::vector<char> data;
    off_t size = AAsset_getLength(asset);

    data.resize(size);
    AAsset_read(asset, &data[0], size);
    AAsset_close(asset);

    std::string dir = dirname(internal_path.c_str());

    if (mkpath(dir, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        LOGW("make path %s failed.", dir.c_str());
    }

    std::ofstream output(internal_path.c_str(),
                         std::ios::out | std::ios::binary);
    if (!output) {
        LOGW("open output file failed %s", internal_path.c_str());
        return false;
    }

    output.write(data.data(), data.size());
    output.close();

    LOGV("copy assets %s success", internal_path.c_str());
    return true;
}

void AssetLoader::Load(AndroidAssetContext* context, const AssetLists &lists) {
    LOGV("start load assets");
    for (auto shaderAsset: lists.sharders) {
        LoadShader(context->nativeActivity->assetManager, shaderAsset);
    }
    for (auto textureAsset : lists.textures) {
        LoadTexture(context, textureAsset);
    }
    for (auto modalAsset: lists.modals) {
        LoadModel(context, modalAsset);
    }
    LOGV("load assets finished.");
}

std::shared_ptr<Shader>
AssetLoader::LoadShader(AAssetManager *assetManager, const ShaderAsset &shaderAsset) {
    std::shared_ptr<Shader> shader = FindShader(shaderAsset.vertexPath, shaderAsset.fragmentPath);
    if (shader) {
        return shader;
    }
    std::string key = shaderAsset.vertexPath + shaderAsset.fragmentPath;
    if (shaderAsset.type == ShaderAssetType_Source){
        shader = GenShader(shaderAsset);
    } else {
        ShaderAsset asset = shaderAsset;
        if (assetManager != nullptr && LoadAssetShaderFile(assetManager, asset)) {
            shader = GenShader(asset);
        }
    }
    if (shader) {
        shader_map_.insert(SHADER_PAIR(key, shader));
    }
    return shader;
}

std::shared_ptr<Texture> AssetLoader::LoadTexture(AndroidAssetContext *androidAssetContext,
                                                  const TextureAsset &textureAsset) {
    std::shared_ptr<Texture> sharedTexture = FindTexture(textureAsset.path);
    if (sharedTexture) {
        return sharedTexture;
    }
    std::string key = textureAsset.path;
    Texture* texture = nullptr;
    switch (textureAsset.type) {
        case TextureAssetType_Local_Normal:
            texture = Texture::LoadTexture(androidAssetContext->nativeActivity->assetManager,
                                           androidAssetContext->bitmapFactory,
                                           androidAssetContext->env,
                                           textureAsset.path.c_str());
            break;
        case TextureAssetType_Local_Skybox:
            texture = Texture::LoadSkyboxTexture(androidAssetContext->nativeActivity->assetManager,
                                                 androidAssetContext->bitmapFactory,
                                                 androidAssetContext->env,
                                                 textureAsset.path.c_str());
            break;
        case TextureAssetType_Net_Normal:
            texture = Texture::LoadNetTexture(textureAsset.path.c_str());
            break;
        case TextureAssetType_Net_Skybox:
            // TODO
            break;
    }
    if (texture != nullptr) {
        sharedTexture = std::shared_ptr<Texture>(texture);
        texture_map_.insert(TEXTURE_PAIR(key, sharedTexture));
        return sharedTexture;
    }
    LOGE("load texture failed %s", textureAsset.path.c_str());
    return nullptr;
}

std::shared_ptr<Model> AssetLoader::LoadModel(AndroidAssetContext* context, const ModelAsset &modelAsset) {
    std::shared_ptr<Model> model = FindModel(modelAsset.path);
    if (model) {
        return model;
    }
    model = std::make_shared<Model>(modelAsset.path);
    if (model->Init(context)) {
        model_map_.insert(MODEL_PAIR(modelAsset.path, model));
        return model;
    }
    return nullptr;
}

bool AssetLoader::LoadAssetShaderFile(AAssetManager *assetManager, ShaderAsset &shaderAsset) {
    if (assetManager == nullptr)
        return false;

    AssetFile v(assetManager, shaderAsset.vertexPath.c_str());
    if (!v.Open()) {
        LOGW("load %s shader failed.", shaderAsset.vertexPath.c_str());
        return false;
    }
    shaderAsset.vertex.assign(v.ToString());
    AssetFile f(assetManager, shaderAsset.fragmentPath.c_str());
    if (!f.Open()) {
        LOGW("load %s shader failed.", shaderAsset.fragmentPath.c_str());
        return false;
    }
    shaderAsset.fragment.assign(f.ToString());
    return  true;
}
#else
void AssetLoader::Load(const AssetLists &lists) {
    for (auto shaderAsset: lists.sharders) {
        LoadShader(shaderAsset);
    }
    for (auto textureAsset : lists.textures) {
        LoadTexture(textureAsset);
    }
    for (auto modalAsset: lists.modals) {
        LoadModel(modalAsset);
    }
}
std::shared_ptr<Shader> AssetLoader::LoadShader(const ShaderAsset &shaderAsset) {
    std::shared_ptr<Shader> shader = FindShader(shaderAsset.vertexPath, shaderAsset.fragmentPath);
    if (shader) {
        return shader;
    }
    std::string key = shaderAsset.vertexPath + shaderAsset.fragmentPath;
    if (shaderAsset.type == ShaderAssetType_Source){
        shader = GenShader(shaderAsset);
    } else {
        ShaderAsset asset = shaderAsset;
        if (LoadAssetShaderFile(asset)) {
            shader = GenShader(asset);
        }
    }
    if (shader) {
        shader_map_.insert(SHADER_PAIR(key, shader));
    }
    return shader;
}

std::shared_ptr<Texture> AssetLoader::LoadTexture(const TextureAsset &textureAsset) {
    std::shared_ptr<Texture> sharedTexture = FindTexture(textureAsset.path);
    if (sharedTexture) {
        return sharedTexture;
    }
    std::string key = textureAsset.path;
    Texture* texture = nullptr;
    switch (textureAsset.type) {
        case TextureAssetType_Local_Normal:
            texture = Texture::LoadTexture(textureAsset.path.c_str());
            break;
        case TextureAssetType_Local_Skybox:
            texture = Texture::LoadSkyboxTexture(textureAsset.path.c_str());
            break;
        case TextureAssetType_Net_Normal:
            texture = Texture::LoadNetTexture(textureAsset.path.c_str());
            break;
        case TextureAssetType_Net_Skybox:
            // TODO
            break;
    }
    if (texture != nullptr) {
        sharedTexture = std::shared_ptr<Texture>(texture);
        texture_map_.insert(TEXTURE_PAIR(key, sharedTexture));
        return sharedTexture;
    }
    LOGE_F("load texture failed %s", textureAsset.path);
    return nullptr;
}
std::shared_ptr<Model> AssetLoader::LoadModel(const ModelAsset &modelAsset) {
    std::string key = modelAsset.path;
    auto it = model_map_.find(key);
    if (it != model_map_.end()) {
        return it->second;
    }
    std::shared_ptr<Model> model = std::make_shared<Model>(modelAsset.path);
    model_map_.insert(MODEL_PAIR(key, model));
    return model;
}
bool AssetLoader::LoadAssetShaderFile(ShaderAsset& shaderAsset) {
    // 1. 从文件路径中获取顶点/片段着色器
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        vShaderFile.open(shaderAsset.vertexPath);
        fShaderFile.open(shaderAsset.fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        shaderAsset.vertex = vShaderStream.str();
        shaderAsset.fragment = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        LOGE("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
        return false;
    }
    return true;
}
#endif

std::shared_ptr<Shader> AssetLoader::GenShader(const ShaderAsset& shaderAsset) {
    std::shared_ptr<Shader> shader = std::make_shared<Shader>(
            shaderAsset.name.c_str(),
            shaderAsset.vertexPath.c_str(), shaderAsset.vertex.c_str(),
            shaderAsset.fragmentPath.c_str(), shaderAsset.fragment.c_str());
    bool ret = shader->Compile();

    if (!ret) {
        return nullptr;
    }
    return shader;
}

std::shared_ptr<Shader>
AssetLoader::FindShader(const std::string &vPath, const std::string &fPaht) {
    std::string key = vPath + fPaht;
    auto it = shader_map_.find(key);
    if (it != shader_map_.end()) {
        return it->second;
    }
    return std::shared_ptr<Shader>();
}

std::shared_ptr<Texture> AssetLoader::FindTexture(const std::string &path) {
    auto it = texture_map_.find(path);
//    LOGV_F("FindTexture %s", path);
    if (it != texture_map_.end()) {
//        LOGV_F("FindTexture %s success", path);
        return it->second;
    }
    return std::shared_ptr<Texture>();
}

std::shared_ptr<Model> AssetLoader::FindModel(const std::string &path) {
    std::string key = path;
    auto it = model_map_.find(key);
    if (it != model_map_.end()) {
        return it->second;
    }
    return std::shared_ptr<Model>();
}
}

