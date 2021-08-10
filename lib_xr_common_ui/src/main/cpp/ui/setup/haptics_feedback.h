//
// Created by fcx on 2020/9/16.
//

#ifndef CLOUDLARKXR_HAPTICS_FEEDBACK_H
#define CLOUDLARKXR_HAPTICS_FEEDBACK_H

#include <ui/component/button.h>
#include "item_base.h"

class UseHapticsFeedback: public ItemBase {
public:
    UseHapticsFeedback(int groupid);
    ~UseHapticsFeedback();

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
};


#endif //CLOUDLARKXR_HAPTICS_FEEDBACK_H
