//
// Created by fcx@pingxingyung.com on 2022/11/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include "setup_skybox.h"
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

SetupSkyBox::SetupSkyBox(int groupid) : ItemBase(groupid) {
    setTitle(localization::Loader::getResource().ui_setup_skybox_title);
    {
        glm::vec2 p{};
        btn_skybox_type0_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_skybox_0);
        btn_skybox_type0_->Move(Base::position_.x + RES_1_POSITION_X, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_skybox_type0_.get());
        AddChild(btn_skybox_type0_);
    }
    {
        glm::vec2 p{};
        btn_skybox_type1_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_skybox_1);
        btn_skybox_type1_->Move(Base::position_.x + RES_2_POSITION_X, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_skybox_type1_.get());
        AddChild(btn_skybox_type1_);
    }
    btn_skybox_type0_->set_color(COLOR_ACTIVE);
    btn_skybox_type1_->set_color(COLOR_UN_ACTIVE);
}

SetupSkyBox::~SetupSkyBox() {

}

void SetupSkyBox::Reset() {
    ItemBase::Reset();
}

void SetupSkyBox::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    btn_skybox_type0_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X, position.y + RES_1_POSITION_Y));
    btn_skybox_type1_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X, position.y + RES_2_POSITION_Y));
}

void SetupSkyBox::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);
    // sync z.
    float z = Base::position_.z + 0.01F;
    btn_skybox_type0_->SetPositionZ(z);
    btn_skybox_type1_->SetPositionZ(z);

    if (!Application::instance()) {
        return;
    }

    if (btn_skybox_type0_->picked() && Input::IsInputEnter()) {
        Application::instance()->SetupSkyBox(0);
        btn_skybox_type0_->set_color(COLOR_ACTIVE);
        btn_skybox_type1_->set_color(COLOR_UN_ACTIVE);
    }

    if (btn_skybox_type1_->picked() && Input::IsInputEnter()) {
        Application::instance()->SetupSkyBox(1);
        btn_skybox_type0_->set_color(COLOR_UN_ACTIVE);
        btn_skybox_type1_->set_color(COLOR_ACTIVE);
    }
}

void SetupSkyBox::Enter() {

}

void SetupSkyBox::Leave() {

}

void SetupSkyBox::FreshData() {
    ItemBase::FreshData();
}
