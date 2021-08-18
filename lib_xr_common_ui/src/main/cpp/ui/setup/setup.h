//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#ifndef CLOUDLARKVRDEMO_SETUP_H
#define CLOUDLARKVRDEMO_SETUP_H

#include "ui/view.h"
#include "ipd.h"
#include "room_height.h"
#include "coderate.h"
#include "resolution.h"
#include "kcp.h"
#include "h265.h"
#include "fps.h"
#include "haptics_feedback.h"
#include "ffr_setup.h"
#include "fec_report.h"
#include "use_10bitencode.h"
#include "quick_config_setup.h"

class Setup: public View {
public:
    enum SetupGroup {
        SetupGroup_Normal = 0,
        SetupGroup_Advance = 1,
    };

    Setup(Navigation *navigation);
    ~Setup();

    void FreshData();
    // update evey frame called by render thread.
    virtual void Update() override;
protected:
    virtual void Init() override;
    // switch to current page.
    virtual void Enter() override;
    // leave current page.
    virtual void Leave() override;
private:
    SetupGroup group_ = SetupGroup_Normal;

    std::shared_ptr<QuickConfigSetup> quick_config_setup_;
    std::shared_ptr<Ipd> ipd_;
    std::shared_ptr<RoomHeight> room_height_;
    std::shared_ptr<Resolution> resolution_;
    std::shared_ptr<CodeRate> code_rate_;
    std::shared_ptr<UseHapticsFeedback> haptics_feedback_;

    std::shared_ptr<KCPSetup> kcp_setup_;
    std::shared_ptr<H265Setup> h265_setup_;
    std::shared_ptr<FFRSetup> ffr_setup_;
    std::shared_ptr<Fps> fps_;
    std::shared_ptr<FECReport> fec_;
    std::shared_ptr<Use10BitEncode> use_10bitencoder_;

    std::vector<std::shared_ptr<ItemBase>> items_;

    std::shared_ptr<ResetButton> reset_btn_;
    std::shared_ptr<TextButton> advance_btn_;
};


#endif //CLOUDLARKVRDEMO_SETUP_H
