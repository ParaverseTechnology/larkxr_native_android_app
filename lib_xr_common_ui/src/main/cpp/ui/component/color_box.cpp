//
// Created by fcx@pingixngyun.com on 2019/11/13.
//

#include "log.h"
#include "color_box.h"
#include "vertex_array_object.h"
#include "env_context.h"
#define LOG_TAG "color_box"

using namespace glm;
using namespace std;

ColorBox::ColorBox(glm::vec4 color, glm::vec2 size, glm::vec3 position) {
    color_ = color;
    size_ = size;
    position_ = position;
    Init();
}

ColorBox::ColorBox(){
    Init();
}

void ColorBox::Init() {
    name_ = LOG_TAG;
    enable_ = false;

    LoadShaderFromAsset(Context::instance()->asset_manager(), "shader/vertex/color_vertex.glsl", "shader/fragment/color_fragment.glsl");
    if (has_error_)
        return;

    model_location_ = shader_->GetUniformLocation("uModel");
    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");

    color_loaction_ = shader_->GetUniformLocation("uColor");

    vao_ = std::make_shared<lark::VertexArrayObject>(true, false);
    if (!vao_) return;
    vao_->BindVAO();
    vao_->BindArrayBuffer();
    int stride = (2 + 3) * sizeof(float);
    GLuint offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*) offset);

    offset += sizeof(float) * 3;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*) offset);
    vao_->UnbindVAO();
    vao_->UnbindArrayBuffer();

    SetVrtxCoord();
    enable_ = true;
}

void ColorBox::SetVrtxCoord() {
    vao_->BindVAO();
    vao_->BindArrayBuffer();
    const float x = position_.x;
    const float y = position_.y;
    const float z = position_.z;
    const float w = size_.x * scale_;
    const float h = size_.y * scale_;
    const float VrtxCoord [] = {
            //AB
            //CD
            // Textcoord need upside down
            x, y + h, z, 0, 0,  // A
            x, y, z, 0, 1,  // C
            x + w, y, z, 1, 1,  // D
            x, y + h, z, 0, 0,  // A
            x + w, y, z, 1, 1,  // D
            x + w, y + h, z, 1, 0   // B
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(VrtxCoord), VrtxCoord, GL_STREAM_DRAW);
    vao_->UnbindVAO();
    vao_->UnbindArrayBuffer();
}

void ColorBox::set_scale(float scale) {
    component::Base::set_scale(scale);
    SetVrtxCoord();
}

void ColorBox::set_size(const component::size& size) {
    component::Base::set_size(size);
    SetVrtxCoord();
}

void ColorBox::set_position(const component::position& position) {
    component::Base::set_position(position);
    SetVrtxCoord();
}


void ColorBox::set_position(float x, float y, float z) {
    Base::set_position(x, y, z);
    SetVrtxCoord();
}

void ColorBox::SetPositionX(float x) {
    Base::SetPositionX(x);
    SetVrtxCoord();
}

void ColorBox::SetPositionY(float y) {
    Base::SetPositionY(y);
    SetVrtxCoord();
}

void ColorBox::SetPositionZ(float z) {
    Base::SetPositionZ(z);
    SetVrtxCoord();
}

void ColorBox::Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) {
    Object::Draw(eye, projection, eyeView);

    if (!enable_)
        return;

    if (!vao_)
        return;

    shader_->UseProgram();
    // mvp
    glUniformMatrix4fv(model_location_, 1, GL_FALSE, glm::value_ptr(GetTransforms()));
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(eyeView));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));
    // color
    glUniform4fv(color_loaction_, 1, glm::value_ptr(color_));

    vao_->BindVAO();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader_->UnUseProgram();
    vao_->UnbindVAO();
}