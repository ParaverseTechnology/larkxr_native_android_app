//
// Created by fcx@pingixngyun.com on 2019/11/13.
//

#include "border.h"

#define LOG_TAG "border"

Border::Border(): Object()
    ,component::Base()
    ,line_width_(1.0)
    , border_up_(nullptr)
    , border_left_(nullptr)
    , border_right_(nullptr)
    , border_down_(nullptr)
{
    border_up_ = std::make_shared<ColorBox>();
    AddChild(border_up_);

    border_down_ = std::make_shared<ColorBox>();
    AddChild(border_down_);

    border_right_ = std::make_shared<ColorBox>();
    AddChild(border_right_);

    border_left_ = std::make_shared<ColorBox>();
    AddChild(border_left_);
}

Border::~Border() = default;

void Border::setLineWidth(float width) {
    line_width_ = width;
    Resize();
}

void Border::set_size(const glm::vec2& size) {
    Base::set_size(size);
    Resize();
}

void Border::set_position(const glm::vec3& position) {
    Base::set_position(position);
    Resize();
}
void Border::set_color(uint64_t rgb, uint8_t alpha) {
    Base::set_color(rgb, alpha);
    border_up_->set_color(rgb, alpha);
    border_down_->set_color(rgb, alpha);
    border_left_->set_color(rgb, alpha);
    border_right_->set_color(rgb, alpha);
}

void Border::set_color(uint64_t rgba) {
    Base::set_color(rgba);
    border_up_->set_color(rgba);
    border_down_->set_color(rgba);
    border_left_->set_color(rgba);
    border_right_->set_color(rgba);
}

void Border::set_color(glm::vec4 color) {
    Base::set_color(color);
    border_up_->set_color(color);
    border_down_->set_color(color);
    border_left_->set_color(color);
    border_right_->set_color(color);
}

void Border::Resize() {
    border_down_->set_size(component::size(size_.x, line_width_));
    border_down_->set_position(component::position(position_.x, position_.y, position_.z));
    border_up_->set_size(component::size(size_.x, line_width_));
    border_up_->set_position(component::position(position_.x, position_.y + size_.y - line_width_, position_.z));
    border_left_->set_size(component::size(line_width_, size_.y));
    border_left_->set_position(component::position(position_.x, position_.y, position_.z));
    border_right_->set_size(component::size(line_width_, size_.y));
    border_right_->set_position(component::position(position_.x + size_.x - line_width_, position_.y, position_.z));
}
