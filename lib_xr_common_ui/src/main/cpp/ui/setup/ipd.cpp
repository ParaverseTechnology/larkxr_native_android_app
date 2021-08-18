//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#include <ui/localization.h>
#include "ipd.h"
#include "lark_xr/xr_config.h"
//#include "cloudlarkvr/render.h"

namespace {
    constexpr float SLIDER_POSITION_X = 0.22F;
    constexpr float SLIDER_POSITION_Y = 0.4F;
    constexpr float SLIDER_POSITION_Z = 0.01F;
    constexpr float SLIDER_SIZE_X = 1.90F;
    constexpr float SLIDER_SIZE_Y = 0.2F;
}

Ipd::Ipd(int groupid):ItemBase(groupid), ipd_(64) {
    setTitle(localization::Loader::getResource().ui_setup_pitch);

    {
        slider_ = std::make_shared<Slider>((Slider::Callback *)this);
        slider_->setValue((float)(ipd_ - MIN) / (float)RANGE);
        slider_->set_position(glm::vec3(SLIDER_POSITION_X, SLIDER_POSITION_Y, SLIDER_POSITION_Z+ Base::position_.z));
        slider_->set_size(glm::vec2(SLIDER_SIZE_X, SLIDER_SIZE_Y));
        PushAABB(slider_.get());
        AddChild(slider_);
    }

    {
        icon_ = std::make_shared<Image>();
        icon_->SetPath("textures/ui/eye_new.png", true);
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
        value_->SetFontSize(36);
        value_->set_position(glm::vec3(0.18F, 0.83F, Base::position_.z + 0.02F));
        AddChild(value_);
    }
}

Ipd::~Ipd() = default;

void Ipd::Reset() {
    ipd_ = (int)(lark::LK_CONFIG_DEFAULT_IPD * 1000);
    slider_->setValue((float)(ipd_ - MIN) / (float)RANGE);
    SetValueStr();
}

void Ipd::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;
    slider_->SetPositionZ(z);
    icon_->SetPositionZ(z);
    data_border_->SetPositionZ(z);
    value_->SetPositionZ(z + 0.02F);
}

void Ipd::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    slider_->SetAABBPositon(glm::vec2(position.x + SLIDER_POSITION_X, position.y + SLIDER_POSITION_Y - slider_->GetAABBSize().y / 2));
}

void Ipd::onSliderValueChanged(float value) {
    ipd_ = (int)(MIN + RANGE * value);
    SetValueStr();
}

void Ipd::SetValueStr() {
    // ipd range 50-70mm
    std::wstring val = std::to_wstring(ipd_) + L"mm";
    if (value_)
        value_->SetText(val);

    lark::XRConfig::ipd = (float) ipd_ / 1000.0F;
}

void Ipd::Enter() {
    FreshData();
}

void Ipd::Leave() {
//    SetValueStr();
}

void Ipd::FreshData() {
    ipd_ = (int)(lark::XRConfig::ipd * 1000);
    // ipd range 50-70mm
    std::wstring val = std::to_wstring(ipd_) + L"mm";
    if (value_)
        value_->SetText(val);
    slider_->setValue((float)(ipd_ - MIN) / (float)RANGE);
}

