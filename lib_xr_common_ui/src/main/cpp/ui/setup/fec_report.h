//
// Created by fcx@pingxingyun.com on 2021/6/30.
//

#ifndef CLOUDLARKXR_FEC_REPORT_H
#define CLOUDLARKXR_FEC_REPORT_H

#include <ui/component/button.h>
#include "item_base.h"

class QuickConfigSetup;
class FECReport: public ItemBase {
public:
    FECReport(int group, QuickConfigSetup* setup);
    ~FECReport();
    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;

    virtual void Enter() override;
    virtual void Leave() override;
    virtual void FreshData() override;
private:
    void OnChange(bool use);
    void OnChangeBitrate(bool use);
    void OnChangeContinueDecode(bool continue_decode);
    void OnChangeFixWaitTimeout(bool fix_wait_timeout);
    void OnChangeResentNackpacket(bool resent);
    void Set(bool use);
    void SetBitrateControl(bool use);
    void SetContinueDecode(bool continue_decode);
    void SetFixWaitTimeout(bool fix_wait_timeout);
    void SetResentNackPacket(bool resent);

    bool use_ = true;
    bool use_bitrate_control_ = true;
    bool continue_decode_ = true;
    bool fix_wait_timeout_ = false;
    bool resent_nackpacket_ = true;

    std::shared_ptr<TextButton> btn_on_;
    std::shared_ptr<TextButton> btn_off_;

#if 0
    std::shared_ptr<Text>     title_bitrate_;

    std::shared_ptr<TextButton> btn_bitrate_on_;
    std::shared_ptr<TextButton> btn_bitrate_off_;

    std::shared_ptr<Text>       title_continue_decode_;
    std::shared_ptr<TextButton> btn_continue_decode_on_;
    std::shared_ptr<TextButton> btn_continue_decode_off_;

    std::shared_ptr<Text>       title_fix_waittimeout_;
    std::shared_ptr<TextButton> btn_fix_waittimeout_on_;
    std::shared_ptr<TextButton> btn_fix_waittimeout_off_;

    std::shared_ptr<Text>       title_resend_nackpacket_;
    std::shared_ptr<TextButton> btn_resend_nackpacket_on_;
    std::shared_ptr<TextButton> btn_resend_nackpacket_off_;
#endif

    QuickConfigSetup* quick_config_setup_;
};


#endif //CLOUDLARKXR_FEC_REPORT_H
