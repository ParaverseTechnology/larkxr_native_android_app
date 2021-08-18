//
// Created by fcx@pingixngyun.com on 2019/11/13.
//

#include <log.h>
#include "slider.h"
#include "input.h"

#define LOG_TAG "slider"

using namespace glm;

Slider::Slider(Callback * callback):
    AABB(id()),
    bg_color_(0x00031eff),
    active_color_(0xd7e1ffff),
    bg_height_(0.075F),
    active_bg_height_(0.015F),
    value_(0.5F),
    slider_bg_(nullptr),
    slider_lint_(nullptr),
    slider_bg_active_(nullptr),
    callback_(callback) {
    Init();
}

Slider::~Slider() = default;

void Slider::Init() {
    slider_bg_ = std::make_shared<ColorBox>();
    slider_bg_->set_color(bg_color_);
    AddChild(slider_bg_);

    slider_lint_ = std::make_shared<Image>();
    slider_lint_->SetPath("textures/ui/circle_huakuai.png", true);
    AddChild(slider_lint_);

    slider_bg_active_ = std::make_shared<ColorBox>();
    slider_bg_active_->set_color(active_color_);
    AddChild(slider_bg_active_);
}

void Slider::SetBgColor(uint64_t rgba) {
    bg_color_ = rgba;
    slider_bg_->set_color(rgba);
}

void Slider::SetActiveColor(uint64_t rgba) {
    active_color_ = rgba;
    slider_bg_active_->set_color(rgba);
}

void Slider::set_position(const glm::vec3& position) {
    Base::set_position(position);
    Resize();
}

void Slider::set_size(const glm::vec2& size) {
    Base::set_size(size);
    Resize();
}

void Slider::SetBgHeight(float height) {
    bg_height_ = height;
    Resize();
}

void Slider::SetActiveBgHeight(float height) {
    active_bg_height_ = height;
    Resize();
}

void Slider::Resize() {
    float vx = Base::size_.x * value_;

    slider_bg_->set_position(vec3(Base::position_.x,
                                Base::position_.y - bg_height_ / 2.0F,
                                Base::position_.z));
    slider_bg_->set_size(vec2(Base::size_.x, bg_height_));

    slider_bg_active_->set_position(vec3(Base::position_.x,
                                      Base::position_.y - active_bg_height_ / 2.0F,
                                      Base::position_.z + 0.0015F));
    slider_bg_active_->set_size(vec2(vx, active_bg_height_));

    slider_lint_->set_position(vec3(Base::position_.x + vx - slider_lint_->GetSize().x / 2.0F,
                                  Base::position_.y - slider_lint_->GetSize().y / 2.0F,
                                  Base::position_.z + 0.002F));

    AABB::SetAABBSize(glm::vec2(Base::size_.x, Base::size_.y + 0.2F));
}

void Slider::setValue(float value) {
    if (value > 1)
        value_ = 1;
    else if (value < 0)
        value_ = 0;
    else
        value_ = value;

    Resize();
}

void Slider::addValue(float vlaue) {
    setValue(value_ + vlaue);
}

void Slider::minusValue(float vlaue) {
    setValue(value_ - vlaue);
}

void Slider::set_position(float x, float y, float z) {
    Base::set_position(x, y, z);
    Resize();
}

void Slider::SetPositionX(float x) {
    Base::SetPositionX(x);
    Resize();
}

void Slider::SetPositionY(float y) {
    Base::SetPositionY(y);
    Resize();
}

void Slider::SetPositionZ(float z) {
    Base::SetPositionZ(z);
    Resize();
}

void Slider::HandleInput(glm::vec2 *point, int pointCount) {
    float dx = 0;
    float dy = 0;
    glm::vec2 aabPosition = GetAABBPosition();
    glm::vec2 aabbSize = GetAABBSize();
    for (int i = 0;i < pointCount; i ++) {
        if (i == Input::GetCurrentRayCastType()) {
            dx = point[i].x - aabPosition.x;
            dy = point[i].y - aabPosition.y;
            break;
        }
    };

    if (dx <= 0 || dx > aabbSize.x || dy < 0 || dy > aabbSize.y) {
        return;
    }

    if (Input::GetCurrentInputState().enterButtonDown || Input::GetCurrentInputState().triggerButtonDown) {
        float value = dx / aabbSize.x;
        setValue(value);
        if (callback_ != nullptr) {
            callback_->onSliderValueChanged(value);
        }
    }
}
