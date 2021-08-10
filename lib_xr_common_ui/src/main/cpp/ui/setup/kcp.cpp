//
// Created by fcx on 2020/6/3.
//

#include "lark_xr/xr_config.h"
#include <input.h>
#include "kcp.h"
#include "quick_config_setup.h"

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float RES_1_POSITION_X = 0.13F;
    constexpr float RES_1_POSITION_Y = 1.0F;
    constexpr float RES_1_POSITION_Z = 0.01F;

    constexpr float RES_2_POSITION_X = 0.13F;
    constexpr float RES_2_POSITION_Y = 0.78F;
    constexpr float RES_2_POSITION_Z = 0.01F;

    constexpr float RES_3_POSITION_X = 0.13F;
    constexpr float RES_3_POSITION_Y = 0.56F;
    constexpr float RES_3_POSITION_Z = 0.01F;

    constexpr float RES_4_POSITION_X = 0.13F;
    constexpr float RES_4_POSITION_Y = 0.34F;
    constexpr float RES_4_POSITION_Z = 0.01F;
}

KCPSetup::KCPSetup(int groupid, QuickConfigSetup* setup):ItemBase(groupid), quick_config_setup_(setup) {
    setTitle(L"传输类型");

    {
        glm::vec2 p{};
        btn_udp_ = std::make_shared<TextButton>(L"UDP");
        btn_udp_->Move(Base::position_.x + RES_1_POSITION_X, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_udp_.get());
        AddChild(btn_udp_);
    }
    {
        glm::vec2 p{};
        btn_tcp_ = std::make_shared<TextButton>(L"TCP");
        btn_tcp_->Move(Base::position_.x + RES_2_POSITION_X, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_tcp_.get());
        AddChild(btn_tcp_);
    }
    {
        glm::vec2 p{};
        btn_kcp_ = std::make_shared<TextButton>(L"增强 UDP");
        btn_kcp_->Move(Base::position_.x + RES_3_POSITION_X, RES_3_POSITION_Y, RES_3_POSITION_Z);
        PushAABB(btn_kcp_.get());
        AddChild(btn_kcp_);
    }
    {
        glm::vec2 p{};
        btn_th_udp_ = std::make_shared<TextButton>(L"调速 UDP");
        btn_th_udp_->Move(Base::position_.x + RES_4_POSITION_X, RES_4_POSITION_Y, RES_4_POSITION_Z);
        PushAABB(btn_th_udp_.get());
        btn_th_udp_->set_active(false);
//        AddChild(btn_th_udp_);
    }
}

KCPSetup::~KCPSetup() = default;

void KCPSetup::Reset() {
    Set(lark::LK_CONFIG_DEFAULT_STREAM_TYPE);
}

void KCPSetup::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;
    btn_udp_->SetPositionZ(z);
    btn_tcp_->SetPositionZ(z);
    btn_kcp_->SetPositionZ(z);
    btn_th_udp_->SetPositionZ(z);

    if (btn_udp_->picked() && Input::IsInputEnter()) {
        OnChange(larkStreamType_UDP);
    }
    if (btn_tcp_->picked() && Input::IsInputEnter()) {
        OnChange(larkStreamType_TCP);
    }
    if (btn_kcp_->picked() && Input::IsInputEnter()) {
        OnChange(larkStreamType_KCP);
    }
    if (btn_th_udp_->picked() && Input::IsInputEnter()) {
        OnChange(larkStreamType_THROTTLED_UDP);
    }
}

void KCPSetup::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    btn_udp_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X, position.y + RES_1_POSITION_Y));
    btn_tcp_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X, position.y + RES_2_POSITION_Y));
    btn_kcp_->SetAABBPositon(glm::vec2(position.x + RES_3_POSITION_X, position.y + RES_3_POSITION_Y));
    btn_th_udp_->SetAABBPositon(glm::vec2(position.x + RES_4_POSITION_X, position.y + RES_4_POSITION_Y));
}

void KCPSetup::Enter() {
    FreshData();
}

void KCPSetup::Leave() {

}

void KCPSetup::OnChange(larkStreamType streamType) {
    Set(streamType);
    // set quick config to manual when use change.
    quick_config_setup_->SetManual();
}

void KCPSetup::Set(larkStreamType streamType) {
    stream_type_ = streamType;
    lark::XRConfig::stream_type = streamType;
    btn_udp_->set_color(stream_type_ == larkStreamType_UDP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
    btn_tcp_->set_color(stream_type_ == larkStreamType_TCP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
    btn_kcp_->set_color(stream_type_ == larkStreamType_KCP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
    btn_th_udp_->set_color(stream_type_ == larkStreamType_THROTTLED_UDP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
}

void KCPSetup::FreshData() {
    stream_type_ = lark::XRConfig::stream_type;
    btn_udp_->set_color(stream_type_ == larkStreamType_UDP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
    btn_tcp_->set_color(stream_type_ == larkStreamType_TCP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
    btn_kcp_->set_color(stream_type_ == larkStreamType_KCP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
    btn_th_udp_->set_color(stream_type_ == larkStreamType_THROTTLED_UDP ? COLOR_ACTIVE : COLOR_UN_ACTIVE);
}


