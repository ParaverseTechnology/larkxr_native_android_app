//
// Created by fcx@pingxingyung.com on 2021/6/10.
// Copyright (c) 2021 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_FFR_SETUP_H
#define CLOUDLARKXR_FFR_SETUP_H

#include <ui/component/button.h>
#include "item_base.h"

class QuickConfigSetup;
class FFRSetup: public ItemBase {
public:
    FFRSetup(int group, QuickConfigSetup* setup);
    ~FFRSetup();
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


#endif //CLOUDLARKXR_FFR_SETUP_H
