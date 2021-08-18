//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#include <ui/component/text.h>
#include "item_base.h"
#include "input.h"

ItemBase::ItemBase(int group):
    AABB(id()),
    bg_color_(0x000530A2),
    bg_color_active_(0x000530B2),
    border_up_color_(0xd7e1ffff),
    aabb_list_(),
    picked_(false),
    group_(group)
{
    init();
}

ItemBase::~ItemBase() = default;

void ItemBase::init() {
    Base::size_ = glm::vec2(2.37F, 1.55F);
    AABB::SetAABBSize(Base::size_);

    bg_ = std::make_shared<ColorBox>();
    bg_->set_size(Base::size_);
    bg_->set_color(bg_color_);
    bg_->set_position(Base::position_);
    AddChild(bg_);

    border_up_ = std::make_shared<ColorBox>();
    border_up_->set_position(glm::vec3(0.115F, Base::position_.y + Base::size_.y - 0.015F, Base::position_.z + 0.001F));
    border_up_->set_size(glm::vec2(0.75F, 0.05F));
    border_up_->set_color(0xd7e1ffff);
    AddChild(border_up_);

    title_ = std::make_shared<Text>(L"");
    title_->set_position(glm::vec3(Base::position_.x + 0.13F, Base::position_.y + 1.25F, Base::position_.z + 0.001F));
    AddChild(title_);
}


void ItemBase::HandleInput(glm::vec2 *point, int pointCount) {
    bool picked = false;
    for (int i = 0;i < pointCount; i ++) {
        if (i == Input::GetCurrentRayCastType()) {
            picked = CheckPointIn(point[i]);
            break;
        }
    };

    if (picked) {
        for(auto& aabb : aabb_list_) {
            aabb->HandleInput(point, pointCount);
        }
    }

    float z = picked ? 0.05F : 0;

    bg_->SetPositionZ(z);
    border_up_->SetPositionZ(z + 0.01F);
    title_->SetPositionZ(z + 0.01F);

    // sync z;
    Base::position_.z = z;
}


void ItemBase::resize() {
    bg_->set_size(Base::size_);
    border_up_->set_position(glm::vec3(0.115, Base::position_.y + Base::size_.y - 0.015F, Base::position_.z + 0.001F));
}

void ItemBase::set_size(const glm::vec2 &size) {
    Base::set_size(size);
    resize();
}

void ItemBase::setBgColor(uint64_t rgba) {
    bg_->set_color(rgba);
}

void ItemBase::setTitle(const std::wstring &title) {
    title_->SetText(title);
}

void ItemBase::PushAABB(AABB *aabb) {
    if (aabb != nullptr)
        aabb_list_.push_back(aabb);
}

void ItemBase::clearAABB() {
    aabb_list_.clear();
}
