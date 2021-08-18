//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include <log.h>
#include "button.h"
#include "ui/localization.h"
#include "input.h"

#define LOG_TAG "button"

namespace {
    const char * ICON_SET        = "textures/ui/icon_set.png";
    const char * ICON_SET_HOVER  = "textures/ui/icon_set_hover.png";
    const char * ICON_ARROW_BACK = "textures/ui/arrow_back.png";
    const char * ICON_PAGE_RIGHT = "textures/ui/page_right.png";
    const char * ICON_PAGE_RIGHT_HOVER = "textures/ui/page_right_hover.png";
    const char * ICON_PAGE_LEFT = "textures/ui/page_left.png";
    const char * ICON_PAGE_LEFT_HOVER = "textures/ui/page_left_hover.png";
}

using namespace glm;

////////////////////////////////////////////////////////////////////////////////////////////////////
Button::Button(): Object(),
    AABB(id()),
    picked_(false),
    pressed_(false),
    icon_(nullptr),
    hover_icon_(nullptr),
    title_(new Text(L"")) {
}

Button::Button(const std::wstring &txt): Object(),
    AABB(id()),
    picked_(false),
    pressed_(false),
    icon_(nullptr),
    hover_icon_(nullptr),
    title_(new Text(txt)) {
}

glm::vec2 Button::GetSize() {
    // remember init.
    glm::vec2 res(0,0);
    if (title_)
        res += title_->GetSize();
    if (icon_)
        res += icon_->GetSize();
    if (hover_icon_)
        res += hover_icon_->GetSize();
    return res;
}

void Button::set_color(component::color color) {
    Base::set_color(color);
    title_->set_color(color);
}

void Button::SetText(const std::wstring & txt) {
    title_->SetText(txt, true);
}

void Button::HandleInput(glm::vec2 *point, int pointCount) {
    for (int i = 0;i < pointCount; i ++) {
        if (i == Input::GetCurrentRayCastType()) {
            picked_ = CheckPointIn(point[i]);
            break;
        }
    };

    float z = picked_ ? Base::position_.z + 0.04F : Base::position_.z;

    if (title_)
        title_->SetPositionZ(z);
    if (icon_) {
        icon_->SetPositionZ(z);
    }
    if (hover_icon_) {
        hover_icon_->SetPositionZ(z);
    }
    if (icon_ && hover_icon_) {
        icon_->set_active(!picked_);
        hover_icon_->set_active(picked_);
    }
}

void Button::SetPositionZ(float z) {
    Base::SetPositionZ(z);

    z = picked_ ? Base::position_.z + 0.04F : Base::position_.z;
    if (title_)
        title_->SetPositionZ(z);
    if (icon_)
        icon_->SetPositionZ(z);
    if (hover_icon_)
        hover_icon_->SetPositionZ(z);
}

std::wstring Button::GetText() {
    if (title_) {
        return title_->text();
    } else {
        return L"";
    }
}

void Button::SetFontSize(int fontSize) {
    if (title_)
        title_->SetFontSize(fontSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SetupButton::SetupButton() :Button() {
    init();
}

void SetupButton::init() {
    title_->SetText(localization::Loader::getResource().ui_home_setup);
    title_->set_color(glm::vec4(0.843F, 0.882F, 1.0F, 0.5F));
    title_->set_parent(this);
    AddChild(title_);

    icon_ = std::make_shared<Image>();
    icon_->set_parent(this);
    icon_->set_position(glm::vec3(title_->width() + 0.035F, -0.03F, 0.0F));
    icon_->SetPath(ICON_SET, true);
    AddChild(icon_);

    hover_icon_ = std::make_shared<Image>();
    hover_icon_->set_parent(this);
    hover_icon_->set_position(glm::vec3(title_->width() + 0.035F, -0.03F, 0.0F));
    hover_icon_->SetPath(ICON_SET_HOVER, true);
    AddChild(hover_icon_);

    AABB::SetAABBSize(GetSize());
}
////////////////////////////////////////////////////////////////////////////////////////////////////
BackButton::BackButton(): Button() {
    init();
}
void BackButton::init() {
    title_->SetText(localization::Loader::getResource().ui_home_back);
    title_->set_color(0xd7e1ffff);
    title_->set_parent(this);
    title_->set_position(vec3(0.20F, 0.03F, 0.0F));
    AddChild(title_);

    icon_ = std::make_shared<Image>();
    icon_->set_parent(this);
    icon_->SetPath(ICON_ARROW_BACK, true);
    AddChild(icon_);

    AABB::SetAABBSize(GetSize());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ResetButton::ResetButton(): Button()  {
    init();
}

void ResetButton::init() {
    title_->SetText(localization::Loader::getResource().ui_home_reset);
    title_->set_color(0xd7e1ffff);
    title_->set_parent(this);
    AddChild(title_);

    AABB::SetAABBSize(GetSize());
}
////////////////////////////////////////////////////////////////////////////////////////////////////
PageButton::PageButton(bool isLeft): mIsLeft(isLeft) {
    init();
}

void PageButton::init() {
    icon_ = std::make_shared<Image>();
    icon_->set_parent(this);
    icon_->SetPath(mIsLeft ? ICON_PAGE_LEFT : ICON_PAGE_RIGHT, true);
    icon_->set_size(vec2(0.25F, 2.5F));
    AddChild(icon_);

    hover_icon_ = std::make_shared<Image>();
    hover_icon_->set_parent(this);
    hover_icon_->SetPath(mIsLeft ? ICON_PAGE_LEFT_HOVER : ICON_PAGE_RIGHT_HOVER, true);
    hover_icon_->set_size(vec2(0.25F, 2.5F));
    hover_icon_->set_active(false);
    AddChild(hover_icon_);

    AABB::SetAABBSize(GetSize());
}
////////////////////////////////////////////////////////////////////////////////////////////////////
TextButton::TextButton(const std::wstring &txt) : Button(txt) {
    init();
}

void TextButton::init() {
    title_->set_color(0xd7e1ffff);
    title_->set_parent(this);
    title_->set_position(vec3(0.0F, 0.0F, 0.0F));
    AddChild(title_);

    AABB::SetAABBSize(GetSize());
}
