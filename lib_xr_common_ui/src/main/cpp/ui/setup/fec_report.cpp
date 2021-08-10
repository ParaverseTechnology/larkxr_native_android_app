//
// Created by fcx@pingixngyun.com on 2021/6/30.
//

#include "fec_report.h"
#include "lark_xr/xr_config.h"
#include <input.h>
#include "quick_config_setup.h"

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float RES_1_POSITION_X = 0.13F;
    constexpr float RES_1_POSITION_Y = 0.9F;
    constexpr float RES_1_POSITION_Z = 0.01F;

    constexpr float RES_2_POSITION_X = 0.13F;
    constexpr float RES_2_POSITION_Y = 0.5F;
    constexpr float RES_2_POSITION_Z = 0.01F;
}

FECReport::FECReport(int group, QuickConfigSetup* setup): ItemBase(group), quick_config_setup_(setup) {
    setTitle(L"开启FEC报告？");

    {
        glm::vec2 p{};
        btn_on_ = std::make_shared<TextButton>(L"是");
        btn_on_->Move(Base::position_.x + RES_1_POSITION_X, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_on_.get());
        AddChild(btn_on_);
    }
    {
        glm::vec2 p{};
        btn_off_ = std::make_shared<TextButton>(L"否");
        btn_off_->Move(Base::position_.x + RES_2_POSITION_X, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_off_.get());
        AddChild(btn_off_);
    }
}

FECReport::~FECReport() {

}

void FECReport::Reset() {
    Set(lark::LK_CONFIG_DEFAULT_REPORT_FEC_FAILED);
}

void FECReport::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    btn_on_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X, position.y + RES_1_POSITION_Y));
    btn_off_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X, position.y + RES_2_POSITION_Y));
}

void FECReport::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;
    btn_on_->SetPositionZ(z);
    btn_off_->SetPositionZ(z);

    if (btn_on_->picked() && Input::IsInputEnter()) {
        OnChange(true);
    }

    if (btn_off_->picked() && Input::IsInputEnter()) {
        OnChange(false);
    }
}

void FECReport::Enter() {
    FreshData();
}

void FECReport::Leave() {

}

void FECReport::OnChange(bool use) {
    if (use != use_) {
        Set(use);
        // set quick config to manual when use change.
        quick_config_setup_->SetManual();
    }
}

void FECReport::Set(bool use) {
    use_ = use;
    lark::XRConfig::report_fec_failed = use;
    if (use_) {
        btn_on_->set_color(COLOR_ACTIVE);
        btn_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_on_->set_color(COLOR_UN_ACTIVE);
        btn_off_->set_color(COLOR_ACTIVE);
    }
}

void FECReport::FreshData() {
    use_ = lark::XRConfig::report_fec_failed;
    if (use_) {
        btn_on_->set_color(COLOR_ACTIVE);
        btn_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_on_->set_color(COLOR_UN_ACTIVE);
        btn_off_->set_color(COLOR_ACTIVE);
    }
}
