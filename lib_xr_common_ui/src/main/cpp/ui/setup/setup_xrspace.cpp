//
// Created by fcx@pingxingyung.com on 2022/11/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include "setup_xrspace.h"
#include <input.h>
#include <ui/localization.h>
#include "application.h"

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float RES_1_POSITION_X = 0.13F;
    constexpr float RES_1_POSITION_Y = 1.0F;
    constexpr float RES_1_POSITION_Z = 0.01F;

    constexpr float RES_2_POSITION_X = 0.13F;
    constexpr float RES_2_POSITION_Y = 0.78F;
    constexpr float RES_2_POSITION_Z = 0.01F;
}

SetupXrSpace::SetupXrSpace(int groupid) : ItemBase(groupid) {
    setTitle(localization::Loader::getResource().ui_setup_skybox_xrspace);

    {
        glm::vec2 p{};
        btn_space_local_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_skybox_xrspace_local);
        btn_space_local_->Move(Base::position_.x + RES_1_POSITION_X, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_space_local_.get());
        AddChild(btn_space_local_);
    }
    {
        glm::vec2 p{};
        btn_space_app_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_skybox_xrspace_app);
        btn_space_app_->Move(Base::position_.x + RES_2_POSITION_X, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_space_app_.get());
        AddChild(btn_space_app_);
    }
    btn_space_local_->set_color(COLOR_ACTIVE);
    btn_space_app_->set_color(COLOR_UN_ACTIVE);
}

SetupXrSpace::~SetupXrSpace() {

}

void SetupXrSpace::Reset() {
    ItemBase::Reset();
}

void SetupXrSpace::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    btn_space_local_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X, position.y + RES_1_POSITION_Y));
    btn_space_app_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X, position.y + RES_2_POSITION_Y));
}

void SetupXrSpace::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);
    // sync z.
    float z = Base::position_.z + 0.01F;
    btn_space_local_->SetPositionZ(z);
    btn_space_app_->SetPositionZ(z);

    if (!Application::instance()) {
        return;
    }

    if (btn_space_local_->picked() && Input::IsInputEnter()) {
        Application::instance()->SetupSapce(Application::Space_Local);
        btn_space_local_->set_color(COLOR_ACTIVE);
        btn_space_app_->set_color(COLOR_UN_ACTIVE);
    }

    if (btn_space_app_->picked() && Input::IsInputEnter()) {
        Application::instance()->SetupSapce(Application::Space_App);
        btn_space_local_->set_color(COLOR_UN_ACTIVE);
        btn_space_app_->set_color(COLOR_ACTIVE);
    }
}

void SetupXrSpace::Enter() {

}

void SetupXrSpace::Leave() {

}

void SetupXrSpace::FreshData() {
    ItemBase::FreshData();
}
