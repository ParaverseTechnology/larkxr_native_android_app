//
// Created by fcx@pingxingyun.com on 2019/11/12.
//

#ifndef CLOUDLARK_OCULUS_DEMO_MODEL_H
#define CLOUDLARK_OCULUS_DEMO_MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "object.h"
#include "mesh.h"

#ifdef ENABLE_ASSIMP
#include <assimp/scene.h>
#endif // ENABLE_ASSIMP

#ifdef __ANDROID__
#include <android/native_activity.h>

#ifdef ENABLE_ASSIMP
#include <assimp/Importer.hpp>
#endif // ENABLE_ASSIMP

#endif
#include "asset_loader.h"

namespace lark {
class Texture;
class CLOUDLARK_PXYGL_API Model: public Object {
public:
    enum MaterialTextureType {
        MaterialTextureType_DIFFUSE,
        MaterialTextureType_SPECULAR,
    };
    struct MaterialTexture {
        MaterialTextureType type;
        std::string path;
    };
#ifdef __ANDROID__
    Model(const std::string& path);
    bool Init(AndroidAssetContext* context);
#else
    Model(const std::string & path);
    bool Init();
#endif
    ~Model();

    void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view) override;
    void DrawMultiview(const glm::mat4& projection, const glm::mat4& view) override;
#ifdef __ANDROID__
    void AddMeterailTexture(AndroidAssetContext* context, const MaterialTexture& materialTexture);
#else
    void AddMeterailTexture(const Model::MaterialTexture& texture);
#endif
    void SetLight(const MeshLight& light);
    void SetColor(const glm::vec4& color);
private:

#ifdef ENABLE_ASSIMP
    void ProcessNode(aiNode *node, const aiScene *scene);
    std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    void LoadMaterialTextures(aiMaterial *mat,const aiTextureType& type,const std::string& typeName, const std::shared_ptr<Mesh> & mesh);
    // search texture in texture search path.
#endif

    std::shared_ptr<Texture> SearchTexture(const std::string & name);

    std::vector<std::shared_ptr<Mesh>> meshes_ = {};
    std::string model_path_ = "";
    std::vector<std::string> texture_search_path_ = {};

    std::vector<MaterialTexture> material_textures_ = {};
#ifdef __ANDROID__
    AndroidAssetContext* context_ = nullptr;
#endif
};
}

#endif //CLOUDLARK_OCULUS_DEMO_MODEL_H
