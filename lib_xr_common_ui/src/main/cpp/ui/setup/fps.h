//
// Created by fcx on 2020/6/17.
//

#ifndef CLOUDLARKXR_FPS_H
#define CLOUDLARKXR_FPS_H


#include <ui/component/button.h>
#include "item_base.h"

class Fps: public ItemBase {
public:
    struct FpsSetup {
        std::wstring tag;
        int fps;
    };
    Fps(int group);

    virtual void Reset() override;

    // handle set postion.
    virtual void SetAABBPositon(const glm::vec2 & position) override;
    // handle input
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;

    virtual void Enter() override;
    virtual void Leave() override;
    virtual void FreshData() override;
private:
    void OnChangeFps(int index);
    void SetFps(int index);

    std::vector<std::shared_ptr<TextButton>> fps_ = {};
    int current_index_ = 0;
};


#endif //CLOUDLARKXR_FPS_H
