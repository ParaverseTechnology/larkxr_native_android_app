//
// Created by fcx@pingixngyun.com on 2019/12/6.
//

#ifndef CLOUDLARKVRDEMO_CODERATE_H
#define CLOUDLARKVRDEMO_CODERATE_H

#include "ui/component/slider.h"
#include "item_base.h"

class QuickConfigSetup;
class CodeRate : public ItemBase, Slider::Callback {
public:
    // coderate range 5 - 50 m
    static constexpr float MIN = 5;
    static constexpr float RANGE = 96;

    CodeRate(int group, QuickConfigSetup* setup);
    ~CodeRate() override ;

    inline int code_rate() { return code_rate_; }
    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;
    // slider value change.
    virtual void onSliderValueChanged(float value) override;
    virtual void FreshData() override;
    virtual void Enter() override;
    virtual void Leave() override;
private:
    void SetValueStr();

    std::shared_ptr<Image> icon_;
    std::shared_ptr<Image> data_border_;
    std::shared_ptr<Slider> slider_;
    std::shared_ptr<Text> value_;

    int code_rate_;
    QuickConfigSetup* quick_config_setup_;
};


#endif //CLOUDLARKVRDEMO_CODERATE_H
