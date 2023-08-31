//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_XR_SCENE_CLOUD_H
#define LARKXR_XR_SCENE_CLOUD_H

#include "ui/menu_view.h"
#include "xr_scene.h"
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <rect_texture.h>
#include "skybox.h"
#include <ui/controller.h>
#ifdef ENABLE_CLOUDXR
#include <cloudxr_client.h>
#include <lark_xr/xr_video_frame.h>

#endif
#include "ui/menu_view.h"

namespace hxr {
class XrSceneCloud: public XrScene, public MenuView::Callback {
public:
    XrSceneCloud();
    ~XrSceneCloud();

    virtual void InitGL() override;
    virtual void HandleInput(const InputState& input_state) override;

    void OnConnect();
    void OnMediaReady(int nativeTexture);
    void OnMediaReady(int nativeTextrueLeft, int nativeTextureRight);
    void OnMediaReady();
    void UpdateTexture(const lark::XRVideoFrame &videoFrame);
    void OnClose();
    virtual void SetVideoFrame(const lark::XRVideoFrame& videoFrame);

    virtual void OnMenuViewSelect(bool submit) override;
#ifdef ENABLE_CLOUDXR
    void SetCloudXRClient(const std::shared_ptr<CloudXRClient>& client);
    void OnCloudXRConnected();
#endif

    void SetSkyBox(int index);

    inline bool IsMenuActive() { return menu_view_->active(); }
    inline void set_headpose(XrPosef pose) { headpose_ = pose; }
private:
    void ShowMenu();
    void HideMenu();
    void OnCloseApp();

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
    std::shared_ptr<Loading> loading_ = nullptr;
    std::shared_ptr<lark::Controller> controller_left_ = nullptr;
    std::shared_ptr<lark::Controller> controller_right_ = nullptr;

    std::shared_ptr<lark::Object> fake_hmd_;
    std::shared_ptr<MenuView> menu_view_;

    std::shared_ptr<RectTexture> rect_texture_{};


#ifdef ENABLE_CLOUDXR
    std::shared_ptr<CloudXRClient> cloudxr_client_ = {};
#endif

    XrPosef headpose_ = {};
};
}

#endif //LARKXR_XR_SCENE_CLOUD_H
