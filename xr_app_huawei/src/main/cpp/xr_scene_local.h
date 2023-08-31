//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_XR_SCENE_LOCAL_H
#define LARKXR_XR_SCENE_LOCAL_H

#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include "ui/navigation.h"
#include "rect_texture.h"
#include "ui/menu_view.h"
#include "skybox.h"
#include "xr_scene.h"

namespace hxr {
class XrSceneLocal: public XrScene {
public:
    XrSceneLocal();
    ~XrSceneLocal();

    virtual void InitGL() override;
    virtual void HandleInput(const InputState& input_state) override;

    void HomePage();
    void LoadingPage();

    void SetSkyBox(int index);
private:
    void OnCloseApp();

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_;
    std::shared_ptr<lark::Controller> controller_left_;
    std::shared_ptr<lark::Controller> controller_right_;

    std::shared_ptr<Navigation> navigation_;
};
}

#endif //LARKXR_XR_SCENE_LOCAL_H
