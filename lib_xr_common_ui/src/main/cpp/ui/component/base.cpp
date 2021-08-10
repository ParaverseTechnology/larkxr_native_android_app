//
// Created by fcx@pingxingyun.com on 2019/11/11.
//

#include "base.h"

namespace component {
    Base::Base(): color_(color(1.0, 1.0, 1.0, 1.0)),
        size_(size(0.0, 0.0)),
        position_(position(0.0, 0.0, 0.0)),
        scale_(1.0F),
        container_size_(size(0.0, 0.0)) {
    }

    void Base::set_color(uint64_t rgb, uint8_t alpha) {
        color_.r = (rgb >> 16) / 255.0F;
        color_.g = (rgb >> 8 & 0xff) / 255.0f;
        color_.b = (rgb & 0xff) / 255.0F;
        color_.a = alpha / 255.0F;
    }

    void Base::set_color(uint64_t rgba) {
        color_.r = (rgba >> 24) / 255.0F;
        color_.g = (rgba >> 16 & 0xff) / 255.0F;
        color_.b = (rgba >> 8 & 0xff) / 255.0F;
        color_.a = (rgba & 0xff) / 255.0F;
    }

    void Base::set_color(color c) {
        color_ = c;
    }

    color Base::GetColor() {
        return color_;
    }

    void Base::set_position(float x, float y, float z) {
        position_.x = x;
        position_.y = y;
        position_.z = z;
    }

    void Base::set_position(const position& p) {
        position_ = p;
    }

    position Base::GetPosition() {
        return position_;
    }

    void Base::set_size(const size& s) {
        size_ = s;
    }

    size Base::GetSize() {
        return size_;
    }

    void Base::set_scale(float scale) {
        scale_ = scale;
    }

    float Base::GetScale() {
        return scale_;
    }

    void Base::SetPositionX(float x) {
        position_.x = x;
    }

    void Base::SetPositionY(float y) {
        position_.y = y;
    }

    void Base::SetPositionZ(float z) {
        position_.z = z;
    }

    void Base::set_container_size(const size &s) {
        container_size_ = s;
    }

    size Base::container_size() {
        return container_size_;
    }
}