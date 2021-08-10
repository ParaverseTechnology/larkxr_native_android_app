//
// Created by fcx@pingixngyun.com on 2019/12/6.
//

#include <ui/localization.h>
#include "lark_xr/xr_config.h"
#include "coderate.h"
#include "quick_config_setup.h"

namespace {
    constexpr float SLIDER_POSITION_X = 0.22F;
    constexpr float SLIDER_POSITION_Y = 0.4F;
    constexpr float SLIDER_POSITION_Z = 0.01F;
    constexpr float SLIDER_SIZE_X = 1.90F;
    constexpr float SLIDER_SIZE_Y = 0.2F;
}

CodeRate::CodeRate(int group, QuickConfigSetup* setup): ItemBase(group), code_rate_(35), quick_config_setup_(setup) {
    setTitle(localization::Loader::getResource().ui_setup_coderate);

    {
        slider_ = std::make_shared<Slider>((Slider::Callback *)this);
        slider_->setValue(((code_rate_ - MIN) / RANGE));
        slider_->set_position(glm::vec3(SLIDER_POSITION_X, SLIDER_POSITION_Y, SLIDER_POSITION_Z+ Base::position_.z));
        slider_->set_size(glm::vec2(SLIDER_SIZE_X, SLIDER_SIZE_Y));
        PushAABB(slider_.get());
        AddChild(slider_);
    }

    {
        icon_ = std::make_shared<Image>();
        icon_->SetPath("textures/ui/hight.png", true);
        icon_->set_position(glm::vec3(1.4F, 0.82F, Base::position_.z + 0.01F));
        AddChild(icon_);
    }

    {
        data_border_ = std::make_shared<Image>();
        data_border_->set_parent(this);
        data_border_->SetPath("textures/ui/data.png", true);
        data_border_->set_position(glm::vec3(0.13F, 0.4F, Base::position_.z + 0.01F));
        AddChild(data_border_);
    }

    {
        value_ = std::make_shared<Text>(L"");
        value_->SetFontSize(30);
        value_->set_position(glm::vec3(0.18F, 0.83F, Base::position_.z + 0.02F));
        AddChild(value_);
    }
}

CodeRate::~CodeRate() = default;

void CodeRate::Reset() {
    code_rate_ = lark::LK_CONFIG_DEFAULT_BITRATE / 1000;
    slider_->setValue((code_rate_ - MIN) / RANGE);
    SetValueStr();
}

void CodeRate::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    slider_->SetAABBPositon(glm::vec2(position.x + SLIDER_POSITION_X, position.y + SLIDER_POSITION_Y - slider_->GetAABBSize().y / 2));
}

void CodeRate::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;
    slider_->SetPositionZ(z);
    icon_->SetPositionZ(z);
    data_border_->SetPositionZ(z);
    value_->SetPositionZ(z + 0.02F);
}

void CodeRate::onSliderValueChanged(float value) {
    code_rate_ = (int)(MIN + RANGE * value);
    SetValueStr();
    // set quick config to manual when use change.
    quick_config_setup_->SetManual();
}

void CodeRate::SetValueStr() {
    std::wstring val = std::to_wstring(code_rate_) + L"Mbps";
    if (value_)
        value_->SetText(val);
    // setup to cloudlark xr.
    lark::XRConfig::bitrate = code_rate_ * 1000;
}

void CodeRate::Enter() {
    FreshData();
}

void CodeRate::Leave() {
//    SetValueStr();
}

void CodeRate::FreshData() {
    // sync value
    code_rate_ = lark::XRConfig::bitrate / 1000;
    std::wstring val = std::to_wstring(code_rate_) + L"Mbps";
    if (value_)
        value_->SetText(val);
    slider_->setValue(((code_rate_ - MIN) / RANGE));
}
