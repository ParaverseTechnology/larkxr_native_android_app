//
// Created by fcx on 2020/4/13.
//

#include "skybox.h"
#include "texture.h"
#include "vertex_array_object.h"
#include "logger.h"
#include "asset_loader.h"

#define LOG_TAG "SkyBox"

namespace  {
    static const float SkyBoxVertices[] = {
            // Positions
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,

            -1.0F, -1.0F,  1.0F,
            -1.0F, -1.0F, -1.0F,
            -1.0F,  1.0F, -1.0F,
            -1.0F,  1.0F, -1.0F,
            -1.0F,  1.0F,  1.0F,
            -1.0F, -1.0F,  1.0F,

            -1.0F,  1.0F, -1.0F,
            1.0F,  1.0F, -1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            -1.0F,  1.0F,  1.0F,
            -1.0F,  1.0F, -1.0F,

            -1.0F, -1.0F, -1.0F,
            -1.0F, -1.0F,  1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            -1.0F, -1.0F,  1.0F,
            1.0F, -1.0F,  1.0F,

            -1.0F, -1.0F,  1.0F,
            -1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F, -1.0F,  1.0F,
            -1.0F, -1.0F,  1.0F,

            -1.0F,  1.0F, -1.0F,
            -1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F,  1.0F, -1.0F,
            -1.0F,  1.0F, -1.0F,
    };
#ifdef __ANDROID__
    const char* vertexShaderSource = "#version 300 es\n"
                                     "layout (location = 0) in vec3 position;\n"
                                     "  uniform mat4 uView;\n"
                                     "  uniform mat4 uProjection;\n"
                                     "out vec3 v3fCoord;\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    vec4 WVP_Pos = uProjection * uView * vec4(position, 1.0);\n"
                                     "    gl_Position = WVP_Pos.xyww;\n"
//                                     "    gl_Position = vec4(position, 1.0);\n"
                                     "    v3fCoord = position;\n"
                                     "}";
    const char * fragmentShaderSource = "#version 300 es\n"
                                        "precision mediump float;\n"
                                        "in vec3 v3fCoord;\n"
                                        "out vec4 FragColor;\n"
                                        "uniform samplerCube atexture;\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "    FragColor = texture(atexture, v3fCoord);\n"
                                        "}";
#else
    const char* vertexShaderSource = "#version 410 core\n"
                                     "layout (location = 0) in vec3 position;\n"
                                     "  uniform mat4 uView;\n"
                                     "  uniform mat4 uProjection;\n"
                                     "  out vec3 v3fCoord;\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    vec4 WVP_Pos = uProjection * uView * vec4(position, 1.0);\n"
                                     "    gl_Position = WVP_Pos.xyww;\n"
                                    // "    gl_Position = vec4(position, 1.0);\n"
                                     "    v3fCoord = position;\n"
                                     "}";
    const char * fragmentShaderSource = "#version 410 core\n"
                                        "precision mediump float;\n"
                                        "in vec3 v3fCoord;\n"
                                        "out vec4 FragColor;\n"
                                        "uniform samplerCube atexture;\n"
                                        //"uniform sampler2D atexture;\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "    FragColor = texture(atexture, v3fCoord);\n"
                                         //"    FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
                                        "}";
#endif
}
namespace lark {
#ifdef __ANDROID__
SkyBox::SkyBox(const char* path) :
        Object(),
        path_(path),
        light_dir_(0, 0, 0, 0)
{
    // claer error;
    HasGLError();

    name_ = LOG_TAG;

    LoadShader("shader/vertex/skybox_vertex.glsl", "shader/fragment/skybox_fragment.glsl",
               vertexShaderSource, fragmentShaderSource);
    if (has_error_)
        return;
    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");
    texture_location_ = shader_->GetUniformLocation("atexture");

    vao_ = std::make_shared<VertexArrayObject>(true, false);

    texture_ = AssetLoader::instance()->FindTexture(path);
    light_dir_ = glm::vec4(-0.8F, 0.45F, 0.4F, 0.40F);
    if (!texture_) {
        has_error_ = true;
        return;
    }
    InitVertices();
}
#else
SkyBox::SkyBox(const char* path) :
    Object(),
    path_(path),
    light_dir_(0, 0, 0, 0)
{
    // claer error;
    HasGLError();

    name_ = LOG_TAG;
    LoadShader("shader/vertex/skybox_vertex.glsl", "shader/fragment/skybox_fragment.glsl",
            vertexShaderSource, fragmentShaderSource);
    if (has_error_)
        return;

    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");
    texture_location_ = shader_->GetUniformLocation("atexture");

    vao_ = std::make_shared<VertexArrayObject>(true, false);

    texture_.reset(Texture::LoadSkyboxTexture(path_.c_str())) ;
//        texture_.reset(Texture::loadSkyboxTexture("textures/skybox_cloudsun.jpg")) ;
//        texture_ = Texture::loadSkyboxTexture("textures/skybox_2.jpg");
    if (!texture_) {
        LOGW("load texture failed.");
        has_error_ = true;
        return;
    }
    //texture_->SetupBitmap();

    light_dir_ = glm::vec4(-0.8F, 0.45F, 0.4F, 0.40F);
    InitVertices();
}
#endif


SkyBox::~SkyBox() = default;

void SkyBox::InitVertices() {
    if (!vao_) return;
    vao_->BindVAO();
    vao_->BindArrayBuffer();
    glBufferData(GL_ARRAY_BUFFER, sizeof(SkyBoxVertices), SkyBoxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
    vao_->UnbindArrayBuffer();
    vao_->UnbindVAO();
}

void SkyBox::Draw(Eye eye, const glm::mat4 &projection, const glm::mat4 &eyeView) {
    Object::Draw(eye, projection, eyeView);

    if (!enable_)
        return;

    // Skybox didn't need eye.
    glm::mat4 viewClone = eyeView;
    viewClone[3][0] = 0;
    viewClone[3][1] = 0;
    viewClone[3][2] = 0;
    viewClone[3][3] = 1;

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    if (!texture_ || !vao_) return;

    shader_->UseProgram();
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(viewClone));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    texture_->BindTextureCubeMap();
    glUniform1i(texture_location_, 0);
    vao_->BindVAO();
    glDrawArrays(GL_TRIANGLES, 0, vertices_);

    vao_->UnbindVAO();
    texture_->UnbindTextureCubeMap();
    shader_->UnUseProgram();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    HasGLError();
}

void SkyBox::SetTexture(const char* path) {
    std::shared_ptr<Texture> texture = AssetLoader::instance()->FindTexture(path);
    if (texture) {
        texture_ = texture;
    } else {
        LOGW("cant find skybox texture %s", path);
    }
}
}