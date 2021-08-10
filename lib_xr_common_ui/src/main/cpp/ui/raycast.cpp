//
// Created by fcx on 2020/4/14.
//

#include <log.h>
#include <env_context.h>
#include "raycast.h"
#include "vertex_array_object.h"
#define LOG_TAG "Raycast"

namespace {
    const float START_X = 0.001;
    const float START_Y = 0.0;
    const float END_X = 0;
    const float END_Y = 0.7;
//    const float START_Z = 0.01;
    const float START_Z = 0.001;
    const float END_Z = 0.001;
//    float verticesTexture[] = {
//            // 正面
//            -START_X, -START_Y, START_Z,   // 左下角
//            START_X,  -START_Y, START_Z,   // 右下角
//            END_X,      END_Y,  END_Z,     // 右上角
//            // 反面
//            -START_X, -START_Y, -START_Z,   // 左下角
//            END_X,      END_Y,  END_Z,      // 右上角
//            START_X,  -START_Y, -START_Z,   // 右下角
//            // 侧面
//            -START_X, -START_Y,  START_Z,   // 左下角
//            -START_X, -START_Y, -START_Z,   // 左下角
//            END_X,      END_Y,  END_Z,      // 右上角
//            // 侧面
//            START_X, -START_Y,  START_Z,   // 左下角
//            END_X,      END_Y,  END_Z,      // 右上角
//            START_X, -START_Y,  -START_Z,   // 左下角
//    };
    float verticesTexture[] = {
            // 正面
            -START_X, -START_Y, START_Z,   // 左下角
            START_X,  -START_Y, START_Z,   // 右下角
            END_X,      END_Y,  END_Z,     // 右上角
            // 反面
            -START_X, -START_Y, -START_Z,   // 左下角
            END_X,      END_Y,  END_Z,      // 右上角
            START_X,  -START_Y, -START_Z,   // 右下角
            // 侧面
            -START_X, -START_Y,  START_Z,   // 左下角
            -START_X, -START_Y, -START_Z,   // 左下角
            END_X,      END_Y,  END_Z,      // 右上角
            // 侧面
            START_X, -START_Y,  START_Z,   // 左下角
            END_X,      END_Y,  END_Z,      // 右上角
            START_X, -START_Y,  -START_Z,   // 左下角
    };
}

Raycast::Raycast() {
    Raycast::Init();
}

void Raycast::Init() {
    name_ = LOG_TAG;
    enable_ = false;
    LoadShaderFromAsset(Context::instance()->asset_manager(), "shader/vertex/color_vertex.glsl", "shader/fragment/color_fragment.glsl");

    if (has_error_) {
        LOGE("loadShaderFromAsset raycast has error");
        return;
    }

    model_location_ = shader_->GetUniformLocation("uModel");
    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");

    color_loaction_ = shader_->GetUniformLocation("uColor");

    auto* vao = new lark::VertexArrayObject(true, true);
    vao_.reset(vao);
    // init texture vao
    InitVAO(verticesTexture, sizeof(verticesTexture), vao_.get());

    enable_ = true;
}

void Raycast::InitVAO(void *vertices, int verticesSize, lark::VertexArrayObject *vao) {
    vao->BindVAO();
    vao->BindArrayBuffer();

    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STREAM_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    vao->UnbindVAO();
    vao->UnbindArrayBuffer();
}

void
Raycast::Draw(Eye eye, const glm::mat4 &projection, const glm::mat4 &eyeView) {
    Object::Draw(eye, projection, eyeView);

    if (!enable_)
        return;

    lark::VertexArrayObject * vao = vao_.get();
    shader_->UseProgram();
    // mvp
    glUniformMatrix4fv(model_location_, 1, GL_FALSE, glm::value_ptr(GetTransforms()));
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(eyeView));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));
    // color
    glUniform4fv(color_loaction_, 1, glm::value_ptr(color_));
    vao->BindVAO();
    glDrawArrays(GL_TRIANGLES, 0, 12);
    shader_->UnUseProgram();
    vao->UnbindVAO();
}
