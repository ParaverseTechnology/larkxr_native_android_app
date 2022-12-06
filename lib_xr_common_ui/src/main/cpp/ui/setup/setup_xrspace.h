//
// Created by fcx@pingxingyung.com on 2022/11/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_SETUP_XRSPACE_H
#define LARKXR_SETUP_XRSPACE_H

#include <ui/component/button.h>
#include "item_base.h"

class SetupXrSpace: public ItemBase {
public:
    SetupXrSpace(int groupid);
    ~SetupXrSpace();

    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;

    virtual void Enter() override;
    virtual void Leave() override;
    virtual void FreshData() override;

private:
    std::shared_ptr<TextButton> btn_space_local_;
    std::shared_ptr<TextButton> btn_space_app_;
};


#endif //LARKXR_SETUP_XRSPACE_H
