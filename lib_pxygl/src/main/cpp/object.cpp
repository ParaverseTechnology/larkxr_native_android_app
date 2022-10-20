//
// Created by fcx@pingxingyun.com on 2019/11/7.
//
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "logger.h"
#include "object.h"
#include "shader.h"
#include "texture.h"
#include "object.h"
#include "vertex_array_object.h"
#include "asset_loader.h"

namespace lark {

uint64_t Object::objcet_count_ = 0;

Object::Object() :
    parent_(nullptr), 
    enable_(true), 
    has_error_(false), 
    transform_(), 
    name_("default"),
    // add id to every id.
    id_(++objcet_count_), 
    active_(true), 
    shader_(nullptr),
    multiview_shader_(nullptr), 
    vao_(nullptr), 
    texture_(nullptr), 
    children_objects_() {
}

Object::~Object() = default;


void Object::InitVao(const void* vertices, int verticesSize, const void* indices, int indicesSize)
{
    vao_->BindVAO();
    vao_->BindArrayBuffer();
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STREAM_DRAW);
    vao_->BindElementArrayBuffer();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    vao_->UnbindVAO();
    vao_->UnbindArrayBuffer();
}

bool Object::AreEnabled() const {
    const bool enable = enable_;
    if (parent_ != nullptr)
        return enable & parent_->AreEnabled();
    return enable;
}

bool Object::HasGLError() const {
    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) {
        LOGE("%s glGetError():GL_OUT_OF_MEMORY", name_);
        return true;
    } else if (err != GL_NO_ERROR) {
        LOGE("%s glGetError():Not_GL_NO_ERROR %u", name_, err);
        return true;
    } else {
        return false;
    }
}

#ifdef __ANDROID__
void
Object::LoadShaderFromAsset(AAssetManager *assetManager, const char *vpath, const char *fpath) {
    ShaderAsset shaderAsset = {
            ShaderAssetType_File,
            name_,
            vpath,
            fpath,
            "", ""
    };
    shader_ = AssetLoader::instance()->LoadShader(assetManager, shaderAsset);
    has_error_ = !shader_;
}
void Object::LoadMultiviewShaderFromAsset(AAssetManager *assetManager, const char *vpath,
                                          const char *fpath) {
    ShaderAsset shaderAsset = {
            ShaderAssetType_File,
            name_,
            vpath,
            fpath,
            "", ""
    };
    multiview_shader_ = AssetLoader::instance()->LoadShader(assetManager, shaderAsset);
    has_error_ = !shader_;
}
#else
void Object::LoadShaderFromAsset(const char* vpath, const char* fpath) {
    ShaderAsset shaderAsset = {
            ShaderAssetType_File,
            name_,
            AssetLoader::instance()->assets_base_path() + vpath,
            AssetLoader::instance()->assets_base_path() + fpath,
            "", ""
    };
    shader_ = AssetLoader::instance()->LoadShader(shaderAsset);
    has_error_ = !shader_;
}

void Object::LoadMultiviewShaderFromAsset(const char* vpath, const char* fpath) {
    ShaderAsset shaderAsset = {
            ShaderAssetType_File,
            name_,
            AssetLoader::instance()->assets_base_path() + vpath,
            AssetLoader::instance()->assets_base_path() + fpath,
            "", ""
    };
    multiview_shader_ = AssetLoader::instance()->LoadShader(shaderAsset);
    has_error_ = !shader_;
}
#endif

void Object::LoadShader(const char* vfile, const char* ffile, const char* vstr, const char* fstr)
{
    ShaderAsset shaderAsset = {
            ShaderAssetType_Source,
            name_,
            vfile, ffile,
            vstr, fstr
    };
#ifdef __ANDROID__
    shader_ = AssetLoader::instance()->LoadShader(nullptr, shaderAsset);
#else
    shader_ = AssetLoader::instance()->LoadShader(shaderAsset);
#endif
    has_error_ = !shader_;
}

void Object::LoadMultviewShader(const char* vfile, const char* ffile, const char* vstr, const char* fstr)
{
    ShaderAsset shaderAsset = {
            ShaderAssetType_Source,
            name_,
            vfile, ffile,
            vstr, fstr
    };
#ifdef __ANDROID__
    multiview_shader_ = AssetLoader::instance()->LoadShader(nullptr, shaderAsset);
#else
    multiview_shader_ = AssetLoader::instance()->LoadShader(shaderAsset);
#endif
    has_error_ = !multiview_shader_;
}

glm::mat4 Object::GetTransforms() const {
    if (parent_ != nullptr) {
        return parent_->GetTransforms() * transform_.GetTrans();
    } else {
        return transform_.GetTrans();
    }
}

void Object::Update() {
    for(auto & mChildrenObject : children_objects_) {
        if (mChildrenObject.get()->active())
            mChildrenObject.get()->Update();
    }
}

void Object::Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view) {
    for(auto & mChildrenObject : children_objects_) {
        if (mChildrenObject.get()->active())
            mChildrenObject.get()->Draw(eye, projection, view);
    }
}

void Object::DrawMultiview(const glm::mat4& projection, const glm::mat4& view)
{
    for (auto& mChildrenObject : children_objects_) {
        if (mChildrenObject.get()->active())
            mChildrenObject.get()->DrawMultiview(projection, view);
    }
}

// TODO
glm::mat3 Object::MakeNormalMatrix(const glm::mat4 &view) const {
    glm::mat3 out(view[0][0], view[0][1], view[0][2],
                  view[1][0], view[1][1], view[1][2],
                  view[2][0], view[2][1], view[1][2]);
    return out;
}

void Object::AddChild(std::shared_ptr<Object>&& child) {
    if (!child)
        return;
    child->set_parent(this);
    children_objects_.push_back(child);
}

void Object::RemoveChild(std::shared_ptr<Object>&& child) {
    if (!child)
        return;
    // TODO objects manager
    for (auto it = children_objects_.begin(); it != children_objects_.end();) {
        if (child->id() == it->get()->id()) {
            children_objects_.erase(it++);
            break;
        } else {
            it++;
        }
    }
}

void Object::ClearChild() {
    if (children_objects_.empty()) {
        return;
    }
    children_objects_.clear();
}
}