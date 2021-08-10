//
// Created by fcx on 2020/6/5.
//

#ifndef CLOUDLARKXR_ASSET_LOADER_H
#define CLOUDLARKXR_ASSET_LOADER_H

#include <string>
#include <map>
#ifdef __ANDROID__
#include <android/native_activity.h>
#endif
#include "texture.h"
#include "shader.h"
#include "pxygl.h"

namespace lark {
enum AssetType {
    AssetType_Shader = 0,     //
    AssetType_Texture,        //
    AssetType_Model,          //
};

enum ShaderAssetType {
    ShaderAssetType_File = 0, // load from asset file.
    ShaderAssetType_Source,   // load from source code.
};

struct ShaderAsset {
    ShaderAssetType type;
    std::string name;          // shader name.
    std::string vertexPath;    // require
    std::string fragmentPath;  // require
    std::string vertex;        // set direct when use source.
    std::string fragment;      // set direct when use source.
};

enum TextureAssetType {
    TextureAssetType_Local_Normal = 0, // local texture.
    TextureAssetType_Local_Skybox,     // local skybox texure.
    TextureAssetType_Net_Normal,       // load texture from network.
    TextureAssetType_Net_Skybox,       // load skybox from network.
};

struct TextureAsset {
    TextureAssetType type;
    std::string path;        // path or net url.
};

struct ModelAsset {
    std::string path;       // modal path.
};

struct Asset {
    AssetType type;
};
struct AssetLists {
    std::vector<ShaderAsset> sharders;
    std::vector<TextureAsset> textures;
    std::vector<ModelAsset> modals;
};

#ifdef __ANDROID__
// TODO config this.
struct AndroidAssetContext {
    ANativeActivity* nativeActivity;
    JNIEnv* env;
    BitmapFactory* bitmapFactory;
};

// ------------------------------------------------------------------------------------------------
// Create the directory for the extracted resource
int mkpath(const std::string& path, mode_t mode);

bool CopyAssetToInternalPath(ANativeActivity* nativeActivity, const std::string& fileName);
#endif

// pre decleare.
class Model;
class CLOUDLARK_PXYGL_API AssetLoader {
public:
    static AssetLoader* instance();
    static void Release();

#ifdef __ANDROID__
    void Load(AndroidAssetContext* androidAssetContext, const AssetLists& lists);
    std::shared_ptr<Shader> LoadShader(AAssetManager* assetManager, const ShaderAsset& shaderAsset);
    std::shared_ptr<Texture> LoadTexture(AndroidAssetContext* androidAssetContext, const TextureAsset& textureAsset);
    // TODO catce mesh and texture not model.
    std::shared_ptr<Model> LoadModel(AndroidAssetContext* context, const ModelAsset& modelAsset);
#else
    void Load(const AssetLists& lists);
    std::shared_ptr<Shader> LoadShader(const ShaderAsset& shaderAsset);
    std::shared_ptr<Texture> LoadTexture(const TextureAsset& textureAsset);
    std::shared_ptr<Model> LoadModel(const ModelAsset& modelAsset);
#endif

    std::shared_ptr<Shader> FindShader(const std::string& vPath, const std::string& fPaht);
    std::shared_ptr<Texture> FindTexture(const std::string& path);
    std::shared_ptr<Model> FindModel(const std::string& path);

    inline void set_assets_base_path(const std::string& path) {
        assets_base_path_ = path;
    }
    inline std::string assets_base_path() {
        return assets_base_path_;
    }
private:
    static AssetLoader* instance_;

    typedef std::pair<std::string, std::shared_ptr<Shader>> SHADER_PAIR;
    typedef std::pair<std::string, std::shared_ptr<Texture>> TEXTURE_PAIR;
    typedef std::pair<std::string, std::shared_ptr<Model>> MODEL_PAIR;

    typedef std::map<std::string, std::shared_ptr<Shader>> SHADER_MAP;
    typedef std::map<std::string, std::shared_ptr<Texture>> TEXTURE_MAP;
    typedef std::map<std::string, std::shared_ptr<Model>> MODEL_MAP;

    AssetLoader();
    ~AssetLoader();

#ifdef __ANDROID__
    bool LoadAssetShaderFile(AAssetManager* assetManager, ShaderAsset& shaderAsset);
#else
    bool LoadAssetShaderFile(ShaderAsset& shaderAsset);
#endif
    std::shared_ptr<Shader> GenShader(const ShaderAsset& shaderAsset);

    SHADER_MAP shader_map_ = {};
    TEXTURE_MAP texture_map_ = {};
    MODEL_MAP model_map_ = {};

    std::string assets_base_path_ = "";
};
}
#endif //CLOUDLARKXR_ASSET_LOADER_H
