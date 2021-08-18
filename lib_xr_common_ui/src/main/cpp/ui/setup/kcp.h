//
// Created by fcx on 2020/6/3.
//

#ifndef CLOUDLARKXR_KCP_H
#define CLOUDLARKXR_KCP_H

#include <ui/component/button.h>
#include "item_base.h"
#include "lark_xr/xr_config.h"

class QuickConfigSetup;
class KCPSetup: public ItemBase {
public:
    KCPSetup(int groupid, QuickConfigSetup* setup);
    ~KCPSetup();

    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;

    virtual void Enter() override;
    virtual void Leave() override;
    virtual void FreshData() override;
private:
    void OnChange(larkStreamType streamType);
    void Set(larkStreamType streamType);
    larkStreamType stream_type_ = larkStreamType_UDP;

    std::shared_ptr<TextButton> btn_udp_;
    std::shared_ptr<TextButton> btn_tcp_;
    std::shared_ptr<TextButton> btn_th_udp_;
    std::shared_ptr<TextButton> btn_kcp_;
    QuickConfigSetup* quick_config_setup_;
};


#endif //CLOUDLARKXR_KCP_H
