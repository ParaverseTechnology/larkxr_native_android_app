//
// Created by Administrator on 2021/7/9.
//

#include <input.h>
#include <ui/localization.h>
#include "quick_config_setup.h"
#include "setup.h"

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float RES_1_POSITION_X = 0.13F;
    constexpr float RES_1_POSITION_Y = 0.7F;
    constexpr float RES_1_POSITION_Z = 0.01F;

    constexpr float RES_2_POSITION_X = RES_1_POSITION_X + 0.5;
    constexpr float RES_2_POSITION_Y = RES_1_POSITION_Y;
    constexpr float RES_2_POSITION_Z = RES_1_POSITION_Z;

    constexpr float RES_3_POSITION_X = RES_2_POSITION_X + 0.5;
    constexpr float RES_3_POSITION_Y = RES_2_POSITION_Y;
    constexpr float RES_3_POSITION_Z = RES_2_POSITION_Z;

    constexpr float RES_4_POSITION_X = RES_3_POSITION_X + 0.5;
    constexpr float RES_4_POSITION_Y = RES_3_POSITION_Y;
    constexpr float RES_4_POSITION_Z = RES_3_POSITION_Z;
}

QuickConfigSetup::QuickConfigSetup(int groupid, Setup* setup): ItemBase(groupid), setup_(setup) {
    setTitle(localization::Loader::getResource().ui_setup_advance_quick_config);

    {
        btn_level_manual_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_advance_quick_config_manual);
        btn_level_manual_->SetFontSize(26);
        btn_level_manual_->Move(Base::position_.x + RES_1_POSITION_X, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_level_manual_.get());
        AddChild(btn_level_manual_);
    }
    {
        btn_level_fast_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_advance_quick_config_fast);
        btn_level_fast_->SetFontSize(26);
        btn_level_fast_->Move(Base::position_.x + RES_2_POSITION_X, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_level_fast_.get());
        AddChild(btn_level_fast_);
    }
    {
        btn_level_normal_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_advance_quick_config_normal);
        btn_level_normal_->SetFontSize(26);
        btn_level_normal_->Move(Base::position_.x + RES_3_POSITION_X, RES_3_POSITION_Y, RES_3_POSITION_Z);
        PushAABB(btn_level_normal_.get());
        AddChild(btn_level_normal_);
    }
    {
        btn_level_extreme_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_advance_quick_config_extreme);
        btn_level_extreme_->SetFontSize(26);
        btn_level_extreme_->Move(Base::position_.x + RES_4_POSITION_X, RES_4_POSITION_Y, RES_4_POSITION_Z);
        PushAABB(btn_level_extreme_.get());
        AddChild(btn_level_extreme_);
    }
}

QuickConfigSetup::~QuickConfigSetup() = default;

void QuickConfigSetup::Reset() {
    Set(lark::DEFAULT_QUICK_CONFIG_LEVEL);
}

void QuickConfigSetup::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    btn_level_manual_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X, position.y + RES_1_POSITION_Y));
    btn_level_fast_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X, position.y + RES_2_POSITION_Y));
    btn_level_normal_->SetAABBPositon(glm::vec2(position.x + RES_3_POSITION_X, position.y + RES_3_POSITION_Y));
    btn_level_extreme_->SetAABBPositon(glm::vec2(position.x + RES_4_POSITION_X, position.y + RES_4_POSITION_Y));
}

void QuickConfigSetup::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;
    btn_level_manual_->SetPositionZ(z);
    btn_level_fast_->SetPositionZ(z);
    btn_level_normal_->SetPositionZ(z);
    btn_level_extreme_->SetPositionZ(z);

    if (btn_level_manual_->picked() && Input::IsInputEnter()) {
        OnChange(lark::QuickConfigLevel_Manual);
    }

    if (btn_level_fast_->picked() && Input::IsInputEnter()) {
        OnChange(lark::QuickConfigLevel_Fast);
    }

    if (btn_level_normal_->picked() && Input::IsInputEnter()) {
        OnChange(lark::QuickConfigLevel_Normal);
    }

    if (btn_level_extreme_->picked() && Input::IsInputEnter()) {
        OnChange(lark::QuickConfigLevel_Extreme);
    }
}



void QuickConfigSetup::Enter() {
    level_ = lark::XRConfig::quick_config_level;
    SetColor();
}

void QuickConfigSetup::Leave() {

}


void QuickConfigSetup::OnChange(lark::QuickConfigLevel config) {
    if (config != level_) {
        Set(config);
    }
}

void QuickConfigSetup::SetManual() {
    Set(lark::QuickConfigLevel_Manual, false);
}

void QuickConfigSetup::Set(lark::QuickConfigLevel config, bool freshData) {
    level_ = config;
    lark::XRConfig::QuickConfigWithDefaulSetup(config);
    if (freshData) {
        setup_->FreshData();
    }
    SetColor();
}

void QuickConfigSetup::SetColor() {
    switch (level_) {
        case lark::QuickConfigLevel_Manual:
            btn_level_manual_->set_color(COLOR_ACTIVE);
            btn_level_fast_->set_color(COLOR_UN_ACTIVE);
            btn_level_normal_->set_color(COLOR_UN_ACTIVE);
            btn_level_extreme_->set_color(COLOR_UN_ACTIVE);
            break;
        case lark::QuickConfigLevel_Fast:
            btn_level_manual_->set_color(COLOR_UN_ACTIVE);
            btn_level_fast_->set_color(COLOR_ACTIVE);
            btn_level_normal_->set_color(COLOR_UN_ACTIVE);
            btn_level_extreme_->set_color(COLOR_UN_ACTIVE);
            break;
        case lark::QuickConfigLevel_Normal:
            btn_level_manual_->set_color(COLOR_UN_ACTIVE);
            btn_level_fast_->set_color(COLOR_UN_ACTIVE);
            btn_level_normal_->set_color(COLOR_ACTIVE);
            btn_level_extreme_->set_color(COLOR_UN_ACTIVE);
            break;
        case lark::QuickConfigLevel_Extreme:
            btn_level_manual_->set_color(COLOR_UN_ACTIVE);
            btn_level_fast_->set_color(COLOR_UN_ACTIVE);
            btn_level_normal_->set_color(COLOR_UN_ACTIVE);
            btn_level_extreme_->set_color(COLOR_ACTIVE);
            break;
        default:
            break;
    }
}
