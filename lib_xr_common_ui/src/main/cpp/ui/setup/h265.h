//
// Created by fcx on 2020/6/3.
//

#ifndef CLOUDLARKXR_H265_H
#define CLOUDLARKXR_H265_H

#include <ui/component/button.h>
#include "item_base.h"

class QuickConfigSetup;
class H265Setup: public ItemBase {
public:
    H265Setup(int groupid, QuickConfigSetup* setup);
    ~H265Setup();
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


#endif //CLOUDLARKXR_H265_H
