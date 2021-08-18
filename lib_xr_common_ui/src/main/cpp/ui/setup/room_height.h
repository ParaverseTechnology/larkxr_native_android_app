//
// Created by fcx@pingixngyun.com on 2019/12/6.
//

#ifndef CLOUDLARKVRDEMO_ROOM_HEIGHT_H
#define CLOUDLARKVRDEMO_ROOM_HEIGHT_H


#include "ui/component/slider.h"
#include "item_base.h"

class RoomHeight: public ItemBase, Slider::Callback {
public:
    // room range 0 - 2 m
    static constexpr float MIN = 0.0f;
    static constexpr float RANGE = 2.51f;

    RoomHeight(int groupid);
    ~RoomHeight();

    inline float room() { return room_; };
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

    float room_;
};


#endif //CLOUDLARKVRDEMO_ROOM_HEIGHT_H
