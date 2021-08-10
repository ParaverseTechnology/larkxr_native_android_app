//
// Created by fcx@pingxingyun.com on 2019/11/12.
//

#ifdef  __ANDROID__
//#include <env_context.h>
//#include "assimp/port/AndroidJNI/AndroidJNIIOSystem.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_activity.h>
#include <libgen.h>

#ifdef ENABLE_ASSIMP
#include <assimp/postprocess.h>
#include "assimp/port/AndroidJNI/AndroidJNIIOSystem.h"
#endif // ENABLE_ASSIMP

#endif //  __ANDROID__

#ifdef ENABLE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "assimp/defs.h"
#else // use tiny obj loader
#include "tiny_obj_loader.h"
#endif // ENABLE_ASSIMP

#include "logger.h"
#include "model.h"

namespace lark {
#ifdef __ANDROID__
Model::Model(const std::string & path):
    model_path_(path),
    meshes_()
{
    // default search current path.
    texture_search_path_.push_back(path.substr(0, path.find_last_of('/')));
}
#else
Model::Model(const std::string & path):
    model_path_(path),
    meshes_(),
    texture_search_path_() {
    // default search current path.
    texture_search_path_.push_back(path.substr(0, path.find_last_of('/')));
}
#endif

Model::~Model() {
    LOGV("release model");
};

void Model::Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view)
{
    Object::Draw(eye, projection, view);

    for (auto mesh = meshes_.begin(); mesh < meshes_.end(); mesh++) {
        mesh->get()->Draw(eye, projection, view);
    }
}

void Model::DrawMultiview(const glm::mat4& projection, const glm::mat4& view)
{
    Object::DrawMultiview(projection, view);

    for (auto mesh = meshes_.begin(); mesh < meshes_.end(); mesh++) {
        mesh->get()->DrawMultiview(projection, view);
    }
}

#ifdef  __ANDROID__
bool Model::Init(AndroidAssetContext* context) {
    if (context->nativeActivity == nullptr) {
        LOGV("AndroidAssetContext not ready");
        return false;
    }
    // catche context.
    context_ = context;

#ifdef ENABLE_ASSIMP
    Assimp::Importer importer;
    Assimp::AndroidJNIIOSystem* ioSystem = new Assimp::AndroidJNIIOSystem(context->nativeActivity);
    importer.SetIOHandler(ioSystem);
    const aiScene* scene = importer.ReadFile(model_path_, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOGV("ERROR::ASSIMP:: %s", importer.GetErrorString());
        return false;
    }
    ProcessNode(scene->mRootNode, scene);
#else
    // use tiny obj loader
    LOGV("use tiny obj loader");

    std::string dir = dirname(model_path_.c_str());
    dir += "/";
    LOGV("load model dir %s", dir.c_str());

    std::vector<std::string> files;

    // Open file
    AAssetDir* asset_dir = AAssetManager_openDir(context_->nativeActivity->assetManager, dir.c_str());
    const char* file_name = AAssetDir_getNextFileName(asset_dir);
    do {
        files.emplace_back(dir + file_name);
        file_name = AAssetDir_getNextFileName(asset_dir);
    } while (file_name != nullptr);
    AAssetDir_close(asset_dir);

    for (auto &asset : files) {
        LOGV("content assets %s", asset.c_str() );
        if (!lark::CopyAssetToInternalPath(context->nativeActivity, asset)) {
            LOGE("CopyAssetToInternalPath failed %s", asset.c_str());
        }
    }

    std::string inputfile = std::string(context->nativeActivity->internalDataPath) + "/" + model_path_;
    std::string matpath = std::string(context->nativeActivity->internalDataPath) + "/" + dir;

    std::vector<tinyobj::shape_t>       shapes = {};
    std::vector<tinyobj::material_t>    materials = {};

    LOGV("start load obj %s, matpath %s", inputfile.c_str(), matpath.c_str());
    std::string err = tinyobj::LoadObj(shapes, materials, inputfile.c_str(), nullptr);

    LOGV("load obj res: %s; shapes %ld; materials %ld", err.c_str(), shapes.size(), materials.size());
// Loop over shapes
    for (auto & shape : shapes) {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        for (size_t j = 0; j < shape.mesh.positions.size() / 3; j++)
        {
            MeshVertex vertex = {};
            vertex.Position.x = shape.mesh.positions[3 * j + 0];
            vertex.Position.y = shape.mesh.positions[3 * j + 1];
            vertex.Position.z = shape.mesh.positions[3 * j + 2];

            vertex.Normal.x = shape.mesh.normals[3 * j + 0];
            vertex.Normal.y = shape.mesh.normals[3 * j + 1];
            vertex.Normal.z = shape.mesh.normals[3 * j + 2];

            vertex.TexCoords.x = shape.mesh.texcoords[2 * j + 0];
            vertex.TexCoords.y = shape.mesh.texcoords[2 * j + 1];
//            LOGV("vertext coords %f %f", vertex.TexCoords.x, vertex.TexCoords.y);
            mesh->AddVerties(vertex);
        }
        mesh->AddIndices(shape.mesh.indices);
        mesh->SetupMesh();
        mesh->set_parent(this);
        meshes_.push_back(mesh);
    }
#endif // ENABLE_ASSIMP

    // clear context after init.
    context_ = nullptr;
    return true;
}
#else
bool Model::Init()
{
    Poco::Timestamp timestamp;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(model_path_, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOGV_F("ERROR::ASSIMP:: %s; path %s;", std::string(importer.GetErrorString()), model_path_);
        return false;
    }
    ProcessNode(scene->mRootNode, scene);
    LOGV_F("==================Load modal %?d ms", timestamp.elapsed() / 1000);
    return false;
}
#endif //  __ANDROID__

#ifdef ENABLE_ASSIMP
void Model::ProcessNode(aiNode *node, const aiScene *scene) {
    // 处理节点所有的网格（如果有的话）
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
         meshes_.push_back(ProcessMesh(mesh, scene));
    }
    // 接下来对它的子节点重复这一过程
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    std::shared_ptr<Mesh> res = std::make_shared<Mesh>();
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertex vertex = {};
        // 处理顶点位置、法线和纹理坐标
        // 顶点
        if (mesh->mVertices) {
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;
        }
        // 法线
        if (mesh->mNormals) {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }
        // 纹理坐标
        // 网格是否有纹理坐标？
        if(mesh->mTextureCoords[0]) {
            vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
//            LOGV("vertext coords %f %f", vertex.TexCoords.x, vertex.TexCoords.y);
        }
        // tangent 切线
        if (mesh->mTangents) {
            vertex.Tangent.x = mesh->mTangents[i].x;
            vertex.Tangent.y = mesh->mTangents[i].y;
            vertex.Tangent.z = mesh->mTangents[i].z;
        }
        // bitangent 二重
        if (mesh->mBitangents) {
            vertex.Bitangent.x = mesh->mBitangents[i].x;
            vertex.Bitangent.y = mesh->mBitangents[i].y;
            vertex.Bitangent.z = mesh->mBitangents[i].z;
        }
        res->AddVerties(vertex);
    }
    // 处理索引
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            res->AddIndices(face.mIndices[j]);
        }
    }
    // 处理材质
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", res);
        // LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", res);
    }

    res->SetupMesh();
    res->set_parent(this);
    return res;
}

void
Model::LoadMaterialTextures(aiMaterial *mat, const aiTextureType &type, const std::string &typeName,
                            const std::shared_ptr<Mesh> & mesh) {
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString ai_fileName;
        mat->GetTexture(type, i, &ai_fileName);
        std::string fileName(ai_fileName.C_Str());

        std::shared_ptr<Texture> texture = SearchTexture(fileName);
        if (texture == nullptr)
            continue;

        texture->set_type_name(typeName);

        // init texture
        texture->BindTexture();
        texture->BindBitmap(GL_RGB5_A1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texture->UnBindTexture();
        texture->CleanBitmap();

        mesh->AddTexture(texture);
    }
}
#endif // ENABLE_ASSIMP

void Model::SetLight(const MeshLight &light) {
    for(auto mesh = meshes_.begin(); mesh < meshes_.end(); mesh++) {
        mesh->get()->set_light(light);
    }
}

void Model::SetColor(const glm::vec4 &color) {
    for(auto mesh = meshes_.begin(); mesh < meshes_.end(); mesh++) {
        mesh->get()->set_color(color);
    }
}

std::shared_ptr<Texture> Model::SearchTexture(const std::string & name) {
    std::shared_ptr<Texture> res = nullptr;
    for (auto path = texture_search_path_.begin(); path < texture_search_path_.end(); path++) {
        std::string filename = *path + "/" + name;
        LOGV("===============SearchTexture %s", filename.c_str());
#ifdef __ANDROID__
        if (context_ == nullptr) {
            LOGW("SearchTexture failed. context empty");
            return nullptr;
        }
        TextureAsset textureAsset = {
                TextureAssetType_Local_Normal,
                filename,
        };
        res = AssetLoader::instance()->LoadTexture(context_, textureAsset);
#else
        res.reset(Texture::LoadTexture(filename.c_str()));
#endif
        if (res != nullptr) {
            break;
        }
    }
    return res;
}

#ifdef __ANDROID__
void Model::AddMeterailTexture(AndroidAssetContext* context, const MaterialTexture& materialTexture) {
    // search texture.
    // cache context
    context_ = context;
    std::shared_ptr<Texture> texture = SearchTexture(materialTexture.path);
    if (texture != nullptr) {
        LOGV("add texture %s %d", materialTexture.path.c_str(), materialTexture.type);
        texture->set_type_name(materialTexture.type == MaterialTextureType_DIFFUSE ? "texture_diffuse" : "texture_specular");

        // init texture
        texture->BindTexture();
        texture->BindBitmap(GL_RGB5_A1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texture->UnBindTexture();
        texture->CleanBitmap();

        material_textures_.push_back(materialTexture);
        for(auto mesh = meshes_.begin(); mesh < meshes_.end(); mesh++) {
            mesh->get()->AddTexture(texture);
        }
    }
    // clear context
    context_ = nullptr;
}
#else
void Model::AddMeterailTexture(const Model::MaterialTexture& materialTexture) {
    // search texture.
    std::shared_ptr<Texture> texture = SearchTexture(materialTexture.path);
    if (texture != nullptr) {
        texture->set_type_name(materialTexture.type == MaterialTextureType_DIFFUSE ? "texture_diffuse" : "texture_specular");

        // init texture
        texture->BindTexture();
        //texture->BindBitmap(GL_RGB5_A1);
        texture->BindBitmap();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        texture->UnBindTexture();
        texture->CleanBitmap();

        material_textures_.push_back(materialTexture);
        for(auto mesh = meshes_.begin(); mesh < meshes_.end(); mesh++) {
            mesh->get()->AddTexture(texture);
        }
    }
}
#endif
}