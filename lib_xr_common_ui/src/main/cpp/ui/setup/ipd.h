//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#ifndef CLOUDLARKVRDEMO_IPD_H
#define CLOUDLARKVRDEMO_IPD_H

#include "item_base.h"
#include "ui/component/slider.h"

class Ipd: public ItemBase, Slider::Callback {
public:
    // ipd range 50 - 70 mm
    static constexpr int MIN = 50;
    static constexpr int RANGE = 21;

    Ipd(int groupid);
    ~Ipd() override;

    inline int ipd() { return ipd_; };
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
    void SetValueStr();

    std::shared_ptr<Image> icon_;
    std::shared_ptr<Image> data_border_;
    std::shared_ptr<Slider> slider_;
    std::shared_ptr<Text> value_;

    int ipd_;
};


#endif //CLOUDLARKVRDEMO_IPD_H
