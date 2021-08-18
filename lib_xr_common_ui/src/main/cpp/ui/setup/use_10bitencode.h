//
// Created by Administrator on 2021/7/8.
//

#ifndef CLOUDLARKXR_USE_10BITENCODE_H
#define CLOUDLARKXR_USE_10BITENCODE_H

#include <ui/component/button.h>
#include "item_base.h"

class QuickConfigSetup;
class Use10BitEncode: public ItemBase {
public:
    Use10BitEncode(int groupid, QuickConfigSetup* quickConfigSetup);
    ~Use10BitEncode();
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


#endif //CLOUDLARKXR_USE_10BITENCODE_H
