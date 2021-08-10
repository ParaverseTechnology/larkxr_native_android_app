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
    void Set(bool use);
    bool use_ = false;

    std::shared_ptr<TextButton> btn_on_;
    std::shared_ptr<TextButton> btn_off_;
    QuickConfigSetup* quick_config_setup_;
};


#endif //CLOUDLARKXR_FEC_REPORT_H
