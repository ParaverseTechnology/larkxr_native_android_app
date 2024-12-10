//
// Created by fcx@pingixngyun.com on 2021/6/30.
//

#include "fec_report.h"
#include "lark_xr/xr_config.h"
#include <input.h>
#include <ui/localization.h>
#include "quick_config_setup.h"

//#define TEST_FEC_RESOND_CONFIGS 1

namespace {
    const glm::vec4 COLOR_ACTIVE = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);
    const glm::vec4 COLOR_UN_ACTIVE = glm::vec4(0.843, 0.882, 1.0, 0.5);

    constexpr float RES_1_POSITION_X = 0.13F;
    constexpr float RES_1_POSITION_Y = 0.9F;
    constexpr float RES_1_POSITION_Z = 0.01F;

    constexpr float RES_2_POSITION_X = 0.43F;
    constexpr float RES_2_POSITION_Y = 0.9F;
    constexpr float RES_2_POSITION_Z = 0.01F;

    constexpr float RES_BITRATE_1_POSITION_X = 0.13F;
    constexpr float RES_BITRATE_1_POSITION_Y = 0.1F;
    constexpr float RES_BITRATE_1_POSITION_Z = 0.01F;

    constexpr float RES_BITRATE_2_POSITION_X = 0.43F;
    constexpr float RES_BITRATE_2_POSITION_Y = 0.1F;
    constexpr float RES_BITRATE_2_POSITION_Z = 0.01F;
}

FECReport::FECReport(int group, QuickConfigSetup* setup): ItemBase(group
#ifdef TEST_FEC_RESOND_CONFIGS 1
                                                                   , glm::vec2(4.37F, 1.55F)
#endif
                                                                   ), quick_config_setup_(setup) {
    setTitle(localization::Loader::getResource().ui_setup_advance_report_fec_title);

    {
        btn_on_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_yes);
        btn_on_->Move(Base::position_.x + RES_1_POSITION_X, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_on_.get());
        AddChild(btn_on_);
    }

    {
        btn_off_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_no);
        btn_off_->Move(Base::position_.x + RES_2_POSITION_X, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_off_.get());
        AddChild(btn_off_);
    }

#ifdef TEST_FEC_RESOND_CONFIGS

    {
        btn_bitrate_on_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_yes);
        btn_bitrate_on_->Move(Base::position_.x + RES_BITRATE_1_POSITION_X, RES_BITRATE_1_POSITION_Y, RES_BITRATE_1_POSITION_Z);
        PushAABB(btn_bitrate_on_.get());
        AddChild(btn_bitrate_on_);
    }

    {
        btn_bitrate_off_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_no);
        btn_bitrate_off_->Move(Base::position_.x + RES_BITRATE_2_POSITION_X, RES_BITRATE_2_POSITION_Y, RES_BITRATE_2_POSITION_Z);
        PushAABB(btn_bitrate_off_.get());
        AddChild(btn_bitrate_off_);
    }

    {
        title_bitrate_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_advance_dynamic_bitrate_title);
        title_bitrate_->set_position(glm::vec3(Base::position_.x + 0.13F, Base::position_.y + 0.5F, Base::position_.z + 0.001F));
        AddChild(title_bitrate_);
    }

    {
        btn_continue_decode_on_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_yes);
        btn_continue_decode_on_->Move(Base::position_.x + RES_BITRATE_1_POSITION_X + 1, RES_BITRATE_1_POSITION_Y, RES_BITRATE_1_POSITION_Z);
        PushAABB(btn_continue_decode_on_.get());
        AddChild(btn_continue_decode_on_);
    }

    {
        btn_continue_decode_off_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_no);
        btn_continue_decode_off_->Move(Base::position_.x + RES_BITRATE_2_POSITION_X + 1, RES_BITRATE_2_POSITION_Y, RES_BITRATE_2_POSITION_Z);
        PushAABB(btn_continue_decode_off_.get());
        AddChild(btn_continue_decode_off_);
    }

    {
        title_continue_decode_ = std::make_shared<Text>(L"丢帧时继续解码");
        title_continue_decode_->set_position(glm::vec3(Base::position_.x + 0.13F + 1, Base::position_.y + 0.5F, Base::position_.z + 0.001F));
        AddChild(title_continue_decode_);
    }

    {
        btn_fix_waittimeout_on_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_yes);
        btn_fix_waittimeout_on_->Move(Base::position_.x + RES_1_POSITION_X + 1, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_fix_waittimeout_on_.get());
        AddChild(btn_fix_waittimeout_on_);
    }

    {
        btn_fix_waittimeout_off_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_no);
        btn_fix_waittimeout_off_->Move(Base::position_.x + RES_2_POSITION_X + 1, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_fix_waittimeout_off_.get());
        AddChild(btn_fix_waittimeout_off_);
    }

    {
        title_fix_waittimeout_ = std::make_shared<Text>(L"固定等待时间");
        title_fix_waittimeout_->set_position(glm::vec3(Base::position_.x + 0.13F + 1, Base::position_.y + 1.25F, Base::position_.z + 0.001F));
        AddChild(title_fix_waittimeout_);
    }

    {
        btn_resend_nackpacket_on_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_yes);
        btn_resend_nackpacket_on_->Move(Base::position_.x + RES_1_POSITION_X + 2.5, RES_1_POSITION_Y, RES_1_POSITION_Z);
        PushAABB(btn_resend_nackpacket_on_.get());
        AddChild(btn_resend_nackpacket_on_);
    }

    {
        btn_resend_nackpacket_off_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_no);
        btn_resend_nackpacket_off_->Move(Base::position_.x + RES_2_POSITION_X + 2.5, RES_2_POSITION_Y, RES_2_POSITION_Z);
        PushAABB(btn_resend_nackpacket_off_.get());
        AddChild(btn_resend_nackpacket_off_);
    }

    {
        title_resend_nackpacket_ = std::make_shared<Text>(L"重发Nack报文");
        title_resend_nackpacket_->set_position(glm::vec3(Base::position_.x + 0.13F + 2.5, Base::position_.y + 1.25F, Base::position_.z + 0.001F));
        AddChild(title_resend_nackpacket_);
    }
#endif
}

FECReport::~FECReport() = default;

void FECReport::Reset() {
    Set(lark::LK_CONFIG_DEFAULT_REPORT_FEC_FAILED);
}

void FECReport::SetAABBPositon(const glm::vec2 &position) {
    AABB::SetAABBPositon(position);
    btn_on_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X, position.y + RES_1_POSITION_Y));
    btn_off_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X, position.y + RES_2_POSITION_Y));

#ifdef TEST_FEC_RESOND_CONFIGS
    btn_bitrate_on_->SetAABBPositon(glm::vec2(position.x + RES_BITRATE_1_POSITION_X, position.y + RES_BITRATE_1_POSITION_Y));
    btn_bitrate_off_->SetAABBPositon(glm::vec2(position.x + RES_BITRATE_2_POSITION_X, position.y + RES_BITRATE_2_POSITION_Y));

    btn_continue_decode_on_->SetAABBPositon(glm::vec2(position.x + RES_BITRATE_1_POSITION_X + 1, position.y + RES_BITRATE_1_POSITION_Y));
    btn_continue_decode_off_->SetAABBPositon(glm::vec2(position.x + RES_BITRATE_2_POSITION_X + 1, position.y + RES_BITRATE_2_POSITION_Y));

    btn_fix_waittimeout_on_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X + 1, position.y + RES_1_POSITION_Y));
    btn_fix_waittimeout_off_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X + 1, position.y + RES_2_POSITION_Y));

    btn_resend_nackpacket_on_->SetAABBPositon(glm::vec2(position.x + RES_1_POSITION_X + 2.5, position.y + RES_1_POSITION_Y));
    btn_resend_nackpacket_off_->SetAABBPositon(glm::vec2(position.x + RES_2_POSITION_X + 2.5, position.y + RES_2_POSITION_Y));
#endif
}

void FECReport::HandleInput(glm::vec2 *point, int pointCount) {
    ItemBase::HandleInput(point, pointCount);

    // sync z.
    float z = Base::position_.z + 0.01F;

#ifdef TEST_FEC_RESOND_CONFIGS
    title_bitrate_->SetPositionZ(z);
    title_continue_decode_->SetPositionZ(z);
    title_fix_waittimeout_->SetPositionZ(z);
    title_resend_nackpacket_->SetPositionZ(z);
#endif

    btn_on_->SetPositionZ(z);
    btn_off_->SetPositionZ(z);

#ifdef TEST_FEC_RESOND_CONFIGS
    btn_bitrate_on_->SetPositionZ(z);
    btn_bitrate_off_->SetPositionZ(z);

    btn_continue_decode_on_->SetPositionZ(z);
    btn_continue_decode_off_->SetPositionZ(z);

    btn_fix_waittimeout_on_->SetPositionZ(z);
    btn_fix_waittimeout_off_->SetPositionZ(z);

    btn_resend_nackpacket_on_->SetPositionZ(z);
    btn_resend_nackpacket_off_->SetPositionZ(z);
#endif

    if (btn_on_->picked() && Input::IsInputEnter()) {
        OnChange(true);
    }
    if (btn_off_->picked() && Input::IsInputEnter()) {
        OnChange(false);
    }

#ifdef TEST_FEC_RESOND_CONFIGS
    if (btn_bitrate_on_->picked() && Input::IsInputEnter()) {
        OnChangeBitrate(true);
    }
    if (btn_bitrate_off_->picked() && Input::IsInputEnter()) {
        OnChangeBitrate(false);
    }

    if (btn_continue_decode_on_->picked() && Input::IsInputEnter()) {
        OnChangeContinueDecode(true);
    }
    if (btn_continue_decode_off_->picked() && Input::IsInputEnter()) {
        OnChangeContinueDecode(false);
    }

    if (btn_fix_waittimeout_on_->picked() && Input::IsInputEnter()) {
        OnChangeFixWaitTimeout(true);
    }
    if (btn_fix_waittimeout_off_->picked() && Input::IsInputEnter()) {
        OnChangeFixWaitTimeout(false);
    }

    if (btn_resend_nackpacket_on_->picked() && Input::IsInputEnter()) {
        OnChangeResentNackpacket(true);
    }
    if (btn_resend_nackpacket_off_->picked() && Input::IsInputEnter()) {
        OnChangeResentNackpacket(false);
    }
#endif
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
    lark::XRConfig::network_control.onepacket_fec = use;
    lark::XRConfig::network_control.dynamic_bitrate = use;
    lark::XRConfig::network_control.resend = use;
    lark::XRConfig::network_control.continue_decode_when_frame_lost = use;
    lark::XRConfig::network_control.fix_wait_frame_timeout = use;
    lark::XRConfig::network_control.resend_nack_packet = use;
    if (use_) {
        btn_on_->set_color(COLOR_ACTIVE);
        btn_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_on_->set_color(COLOR_UN_ACTIVE);
        btn_off_->set_color(COLOR_ACTIVE);
    }
}

void FECReport::FreshData() {
    use_ = lark::XRConfig::network_control.onepacket_fec;

    if (use_) {
        btn_on_->set_color(COLOR_ACTIVE);
        btn_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_on_->set_color(COLOR_UN_ACTIVE);
        btn_off_->set_color(COLOR_ACTIVE);
    }

#ifdef TEST_FEC_RESOND_CONFIGS
    use_bitrate_control_ = lark::XRConfig::network_control.dynamic_bitrate;
    if (use_bitrate_control_) {
        btn_bitrate_on_->set_color(COLOR_ACTIVE);
        btn_bitrate_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_bitrate_on_->set_color(COLOR_UN_ACTIVE);
        btn_bitrate_off_->set_color(COLOR_ACTIVE);
    }

    continue_decode_ = lark::XRConfig::network_control.continue_decode_when_frame_lost;
    if (continue_decode_) {
        btn_continue_decode_on_->set_color(COLOR_ACTIVE);
        btn_continue_decode_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_continue_decode_on_->set_color(COLOR_UN_ACTIVE);
        btn_continue_decode_off_->set_color(COLOR_ACTIVE);
    }

    fix_wait_timeout_ = lark::XRConfig::network_control.fix_wait_frame_timeout;
    if (fix_wait_timeout_) {
        btn_fix_waittimeout_on_->set_color(COLOR_ACTIVE);
        btn_fix_waittimeout_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_fix_waittimeout_on_->set_color(COLOR_UN_ACTIVE);
        btn_fix_waittimeout_off_->set_color(COLOR_ACTIVE);
    }

    resent_nackpacket_ = lark::XRConfig::network_control.resend_nack_packet;
    if (resent_nackpacket_) {
        btn_resend_nackpacket_on_->set_color(COLOR_ACTIVE);
        btn_resend_nackpacket_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_resend_nackpacket_on_->set_color(COLOR_UN_ACTIVE);
        btn_resend_nackpacket_off_->set_color(COLOR_ACTIVE);
    }
#endif
}

void FECReport::OnChangeBitrate(bool use) {
#ifdef TEST_FEC_RESOND_CONFIGS
    if (use_bitrate_control_ != use) {
        SetBitrateControl(use);
        // set quick config to manual when use change.
        quick_config_setup_->SetManual();
    }
#endif
}

void FECReport::SetBitrateControl(bool use) {
#ifdef TEST_FEC_RESOND_CONFIGS
    use_bitrate_control_ = use;
    lark::XRConfig::network_control.dynamic_bitrate = use;
    if (use_bitrate_control_) {
        btn_bitrate_on_->set_color(COLOR_ACTIVE);
        btn_bitrate_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_bitrate_on_->set_color(COLOR_UN_ACTIVE);
        btn_bitrate_off_->set_color(COLOR_ACTIVE);
    }
#endif
}

void FECReport::SetContinueDecode(bool continue_decode) {
#ifdef TEST_FEC_RESOND_CONFIGS
    continue_decode_ = continue_decode;
    lark::XRConfig::network_control.continue_decode_when_frame_lost = continue_decode;
    if (continue_decode_) {
        btn_continue_decode_on_->set_color(COLOR_ACTIVE);
        btn_continue_decode_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_continue_decode_on_->set_color(COLOR_UN_ACTIVE);
        btn_continue_decode_off_->set_color(COLOR_ACTIVE);
    }
#endif
}

void FECReport::OnChangeContinueDecode(bool use) {
#ifdef TEST_FEC_RESOND_CONFIGS
    if (continue_decode_ != use) {
        SetContinueDecode(use);
        // set quick config to manual when use change.
        quick_config_setup_->SetManual();
    }
#endif
}

void FECReport::OnChangeFixWaitTimeout(bool fix_wait_timeout) {
#ifdef TEST_FEC_RESOND_CONFIGS
    if (fix_wait_timeout_ != fix_wait_timeout) {
        SetFixWaitTimeout(fix_wait_timeout);
        // set quick config to manual when use change.
        quick_config_setup_->SetManual();
    }
#endif
}

void FECReport::SetFixWaitTimeout(bool fix_wait_timeout) {
#ifdef TEST_FEC_RESOND_CONFIGS
    fix_wait_timeout_ = fix_wait_timeout;
    lark::XRConfig::network_control.fix_wait_frame_timeout = fix_wait_timeout;
    if (fix_wait_timeout_) {
        btn_fix_waittimeout_on_->set_color(COLOR_ACTIVE);
        btn_fix_waittimeout_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_fix_waittimeout_on_->set_color(COLOR_UN_ACTIVE);
        btn_fix_waittimeout_off_->set_color(COLOR_ACTIVE);
    }
#endif
}

void FECReport::OnChangeResentNackpacket(bool resent) {
#ifdef TEST_FEC_RESOND_CONFIGS
    if (resent_nackpacket_ != resent) {
        SetResentNackPacket(resent);
        // set quick config to manual when use change.
        quick_config_setup_->SetManual();
    }
#endif
}

void FECReport::SetResentNackPacket(bool resent) {
#ifdef TEST_FEC_RESOND_CONFIGS
    resent_nackpacket_ = resent;
    lark::XRConfig::network_control.resend_nack_packet = resent;
    if (resent_nackpacket_) {
        btn_resend_nackpacket_on_->set_color(COLOR_ACTIVE);
        btn_resend_nackpacket_off_->set_color(COLOR_UN_ACTIVE);
    } else {
        btn_resend_nackpacket_on_->set_color(COLOR_UN_ACTIVE);
        btn_resend_nackpacket_off_->set_color(COLOR_ACTIVE);
    }
#endif
}
