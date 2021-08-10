//
// Created by fcx@pingixngyun.com on 2019/12/6.
//

#include <ui/localization.h>
#include <utils.h>
#include "room_height.h"
#include "lark_xr/xr_config.h"
//#include "cloudlarkvr/vr_api.h"

namespace {
    constexpr float SLIDER_POSITION_X = 0.22F;
    constexpr float SLIDER_POSITION_Y = 0.4F;
    constexpr float SLIDER_POSITION_Z = 0.01F;
    constexpr float SLIDER_SIZE_X = 1.90F;
    constexpr float SLIDER_SIZE_Y = 0.2F;
}

RoomHeight::RoomHeight(int groupid):ItemBase(groupid), room_(0) {
    setTitle(localization::Loader::getResource().ui_setup_inited_height);

    {
        slider_ = std::make_shared<Slider>((Slider::Callback *)this);
        slider_->setValue((room_ - MIN) / RANGE);
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
        value_->SetFontSize(36);
        value_->set_position(glm::vec3(0.18F, 0.83F, Base::position_.z + 0.02F));
        AddChild(value_);
    }
}

RoomHeight::~RoomHeight() = default;

void RoomHeight::Reset() {
    room_ = lark::LK_CONFIG_DEFAULT_ROOM_HEIGHT;
    slider_->setValue((room_ - MIN) / RANGE);
    SetValueStr();
}

void RoomHeight::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    slider_->SetAABBPositon(glm::vec2(position.x + SLIDER_POSITION_X, position.y + SLIDER_POSITION_Y - slider_->GetAABBSize().y / 2));
}

void RoomHeight::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;
    slider_->SetPositionZ(z);
    icon_->SetPositionZ(z);
    data_border_->SetPositionZ(z);
    value_->SetPositionZ(z + 0.02F);
}

void RoomHeight::onSliderValueChanged(float value) {
    room_ = MIN + RANGE * value;
    SetValueStr();
}

void RoomHeight::SetValueStr() {
    // room range 0 - 2 m
    char out[50];
    sprintf(out, "%2.2f", room_);
    std::string outStr(out);
    std::wstring val = utils::StringToWstring(outStr) + L"m";
    if (value_)
        value_->SetText(val);

    // setup room height.
    lark::XRConfig::room_height = room_;
}

void RoomHeight::Enter() {
    FreshData();
}

void RoomHeight::Leave() {
//    SetValueStr();
}

void RoomHeight::FreshData() {
    room_ =  lark::XRConfig::room_height;
    char out[50];
    sprintf(out, "%2.2f", room_);
    std::string outStr(out);
    std::wstring val = utils::StringToWstring(outStr) + L"m";
    if (value_)
        value_->SetText(val);
    slider_->setValue(((room_ - MIN) / RANGE));
}
