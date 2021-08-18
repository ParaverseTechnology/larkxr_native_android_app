//
// Created by fcx@pingixngyun.com on 2019/12/9.
//

#include <log.h>
//#include <cloudlarkvr/render.h>
#include "resolution.h"
#include <ui/localization.h>
#include <lark_xr/xr_config.h>
#include <lark_xr/xr_client.h>
#include <utils.h>
#include "input.h"
#include "setup.h"
#define LOG_TAG "resolution"
#include "quick_config_setup.h"

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float SLIDER_POSITION_X = 0.22F;
    constexpr float SLIDER_POSITION_Y = 0.3F;
    constexpr float SLIDER_POSITION_Z = 0.01F;
    constexpr float SLIDER_SIZE_X = 1.90F;
    constexpr float SLIDER_SIZE_Y = 0.2F;

    constexpr float RES_POSITION_X = 0.13F;
    constexpr float RES_POSITION_Y = 1.0F;
    constexpr float RES_POSITION_Z = 0.01F;
    constexpr float RES_SKIP = 0.22;
}

Resolution::Resolution(int groupid, QuickConfigSetup* setup):ItemBase(groupid), quick_config_setup_(setup) {
    setTitle(localization::Loader::getResource().ui_setup_resolution_title);

    {
        slider_ = std::make_shared<Slider>((Slider::Callback *)this);
        slider_->setValue(((scale_ - MIN) / RANGE));
        slider_->set_position(glm::vec3(SLIDER_POSITION_X, SLIDER_POSITION_Y, SLIDER_POSITION_Z + Base::position_.z));
        slider_->set_size(glm::vec2(SLIDER_SIZE_X, SLIDER_SIZE_Y));
        PushAABB(slider_.get());
        AddChild(slider_);
    }

    {
        data_border_ = std::make_shared<Image>();
        data_border_->set_parent(this);
        data_border_->SetPath("textures/ui/data.png", true);
        data_border_->set_position(glm::vec3(0.13F, 0.3F, Base::position_.z + 0.01F));
        AddChild(data_border_);
    }

    {
        value_ = std::make_shared<Text>(L"");
        value_->SetFontSize(30);
        value_->set_position(glm::vec3(0.18F, 0.75F, Base::position_.z + 0.02F));
        AddChild(value_);
    }

    resolution_ = std::make_shared<Text>(L"");
    resolution_->SetFontSize(22);
    resolution_->Move(Base::position_.x + RES_POSITION_X, RES_POSITION_Y, RES_POSITION_Z);
    AddChild(resolution_);

    aligned_resolution_ = std::make_shared<Text>(L"");
    aligned_resolution_->SetFontSize(22);
    aligned_resolution_->set_position(glm::vec3(0.7F, 0.75, Base::position_.z + 0.02F));
//    AddChild(aligned_resolution_);
}

Resolution::~Resolution() = default;

void Resolution::Reset() {
    scale_ = lark::LK_CONFIG_DEFAULT_RESOLUTION_SCALE;
    slider_->setValue((scale_ - MIN) / RANGE);
    SetValueStr();
}

void Resolution::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    slider_->SetAABBPositon(glm::vec2(position.x + SLIDER_POSITION_X, position.y + SLIDER_POSITION_Y - slider_->GetAABBSize().y / 2));
}

void Resolution::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);
    // sync z.
    float z = Base::position_.z + 0.01F;
    slider_->SetPositionZ(z);
    data_border_->SetPositionZ(z);
    value_->SetPositionZ(z + 0.02F);
    resolution_->SetPositionZ(z);
}

void Resolution::OnChangeResolution(int index) {
}

void Resolution::SetResolution(int index) {
}

void Resolution::Enter() {
    FreshData();
}

void Resolution::Leave() {

}

void Resolution::onSliderValueChanged(float value) {
    scale_ = MIN + RANGE * value;
    quick_config_setup_->SetManual();
    // set quick config to manual when use change.
    SetValueStr();
}

void Resolution::SetValueStr() {
    char out[50];
    sprintf(out, "%2.2f", scale_);
    lark::XRConfig::resolution_scale = atof(out);
    value_->SetText(utils::StringToWstring(out));
//    LOGV("set value %f", lark::XRConfig::resolution_scale);
}

void Resolution::FreshData() {
    wchar_t buf[100];
    swprintf(buf, 100, L"Device %dX%d", lark::XRConfig::render_width, lark::XRConfig::render_height);
    resolution_->SetText(buf);

    swprintf(buf, 100, L" Scaled %dX%d",
             lark::XRConfig::align32ed_scaled_render_width(),
             lark::XRConfig::align32ed_scaled_render_height());
    aligned_resolution_->SetText(buf);

    scale_ = lark::XRConfig::resolution_scale;
    slider_->setValue(((scale_ - MIN) / RANGE));

    swprintf(buf, 100, L"%2.2f", scale_);
    value_->SetText(buf);
}
