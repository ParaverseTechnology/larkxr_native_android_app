//
// Created by Administrator on 2021/7/9.
//

#ifndef CLOUDLARKXR_QUICK_CONFIG_SETUP_H
#define CLOUDLARKXR_QUICK_CONFIG_SETUP_H

#include <ui/component/button.h>
#include "item_base.h"
#include "lark_xr/xr_config.h"

class Setup;
class QuickConfigSetup: public ItemBase {
public:
    QuickConfigSetup(int groupid, Setup* setup);
    ~QuickConfigSetup();
    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;

    virtual void Enter() override;
    virtual void Leave() override;
    void SetManual();
private:
    void OnChange(lark::QuickConfigLevel config);
    void Set(lark::QuickConfigLevel config, bool freshData = true);
    void SetColor();

    lark::QuickConfigLevel level_ = lark::QuickConfigLevel_Normal;
    std::shared_ptr<TextButton> btn_level_manual_;
    std::shared_ptr<TextButton> btn_level_fast_;
    std::shared_ptr<TextButton> btn_level_normal_;
    std::shared_ptr<TextButton> btn_level_extreme_;
    Setup* setup_;
};


#endif //CLOUDLARKXR_QUICK_CONFIG_SETUP_H
