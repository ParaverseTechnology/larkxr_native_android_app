//
// Created by fcx@pingixngyun.com on 2019/12/9.
//

#ifndef CLOUDLARKVRDEMO_RESOLUTION_H
#define CLOUDLARKVRDEMO_RESOLUTION_H

#include <ui/component/button.h>
#include "ui/component/slider.h"
#include "item_base.h"

class QuickConfigSetup;
class Resolution: public ItemBase, Slider::Callback {
public:
    // scale range 0.25 - 1 m
    static constexpr float MIN = 0.25f;
    static constexpr float RANGE = 0.75f;

    Resolution(int groupid, QuickConfigSetup* setup);
    ~Resolution();

    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;
    // slider value change.
    virtual void onSliderValueChanged(float value) override;

    virtual void Enter() override;
    virtual void Leave() override;
    virtual void FreshData() override;
private:
    void OnChangeResolution(int index);
    void SetResolution(int index);
    void SetValueStr();

    std::shared_ptr<Slider> slider_;
    std::shared_ptr<Text> resolution_ = nullptr;
    std::shared_ptr<Image> data_border_;
    std::shared_ptr<Text> value_;
    std::shared_ptr<Text> aligned_resolution_ = nullptr;
    float scale_ = 1.0f;
    QuickConfigSetup* quick_config_setup_;
};


#endif //CLOUDLARKVRDEMO_RESOLUTION_H
