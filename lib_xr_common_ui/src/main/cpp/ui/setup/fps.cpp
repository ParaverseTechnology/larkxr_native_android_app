//
// Created by fcx on 2020/6/17.
//

#include "fps.h"
#include <log.h>
#include <ui/localization.h>
#include "lark_xr/xr_config.h"
#include "lark_xr/xr_client.h"
#include "input.h"
#define LOG_TAG "fps"

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float RES_POSITION_X = 0.13F;
    constexpr float RES_POSITION_Y = 1.0F;
    constexpr float RES_POSITION_Z = 0.01F;
    constexpr float RES_SKIP = 0.22;

    const std::vector<Fps::FpsSetup> FPS_SETUPS = {
            { L"60 (2K Video Common1)", 60 },
            { L"72 (Oculus Quest)", 72 },
            { L"75 (HTC Common1)", 75 },
            { L"90 (Common2)", 90 },
    };
}

Fps::Fps(int group): ItemBase(group) {
    setTitle(L"FPS");

    for (int i = 0; i < FPS_SETUPS.size(); i++) {
        std::shared_ptr<TextButton> btn = std::make_shared<TextButton>(FPS_SETUPS[i].tag);
        btn->SetFontSize(26);
        btn->Move(Base::position_.x + RES_POSITION_X, RES_POSITION_Y - i * RES_SKIP, RES_POSITION_Z);
        PushAABB(btn.get());
        AddChild(btn);
        fps_.push_back(btn);
    }
    for (int i = 0; i < fps_.size(); i++) {
        LOGV("fps %d", FPS_SETUPS[i].fps);
    }
}

void Fps::Reset() {
    if (lark::XRClient::system_info().headSetSdkVersion == Larkxr_HEADSET_SDK_OCULUS) {
        // setup to 72
        SetFps(1);
    } else {
        SetFps(2);
    }
}

void Fps::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    for (int i = 0; i < fps_.size(); i++) {
        fps_[i]->SetAABBPositon(glm::vec2(position.x + RES_POSITION_X, position.y + RES_POSITION_Y - i * RES_SKIP));
    }
}

void Fps::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;

    for (int i = 0; i < fps_.size(); i++) {
        fps_[i]->SetPositionZ(z);
        if (fps_[i]->picked() && Input::IsInputEnter()) {
            OnChangeFps(i);
        }
    }
}

void Fps::OnChangeFps(int index) {
    if (index != current_index_) {
        SetFps(index);
    }
}

void Fps::SetFps(int index) {
    if (index < 0 || index > fps_.size()) {
        LOGW("SetFps failed. outsize index %d", index);
        return;
    }
    current_index_ = index;
    for (int i = 0; i < fps_.size(); i++) {
        if (i == index) {
            lark::XRConfig::fps = FPS_SETUPS[i].fps;
            fps_[i]->set_color(COLOR_ACTIVE);
        } else {
            fps_[i]->set_color(COLOR_UN_ACTIVE);
        }
    }
}

void Fps::Enter() {
    FreshData();
}

void Fps::Leave() {

}

void Fps::FreshData() {
    // find active.
    for (int i = 0; i < fps_.size(); i++) {
        if (lark::XRConfig::fps == FPS_SETUPS[i].fps) {
            SetFps(i);
        }
    }
}
