//
// Created by Administrator on 2021/7/7.
//

#ifndef CLOUDLARKXR_PVR_XR_SCENE_CLOUD_H
#define CLOUDLARKXR_PVR_XR_SCENE_CLOUD_H

#include <input.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <rect_texture.h>
#include "skybox.h"
#include "pvr_xr_scene.h"
#include "xr_input_state.h"
#include <ui/controller.h>
#ifdef ENABLE_CLOUDXR
#include <cloudxr_client.h>
#endif
#include "ui/menu_view.h"

class PvrXRSceneCloud : public PvrXRScene, public MenuView::Callback {
public:
    PvrXRSceneCloud();
    ~PvrXRSceneCloud();

    void OnConnect();
    void OnMediaReady(int nativeTexture);
    void OnMediaReady(int nativeTextrueLeft, int nativeTextureRight);
    void OnMediaReady();
    void UpdateTexture(const lark::XRVideoFrame &videoFrame);
    void OnClose();

    virtual void InitGL(GraphicsDeviceAndroid *device) override;

    virtual void HandleInput(const InputState& input_state, const XrSession& session, const XrSpace& space) override;

    virtual void SetVideoFrame(const lark::XRVideoFrame& videoFrame);

    virtual void OnMenuViewSelect(bool submit) override;

    inline bool IsMenuActive() { return menu_view_->active(); }

    // PICO 2.2.0
    // https://developer-cn.pico-interactive.com/document/native/release-notes/
    // inline void set_view_state(XrViewStatePICOEXT state) { view_state_ = state; }
    inline void set_headpose(XrPosef pose) { headpose_ = pose; }

    inline larkxrControllerDeviceState GetControllerState(int hand) {
        return controller_state_[hand];
    }

#ifdef ENABLE_CLOUDXR
    void SetCloudXRClient(const std::shared_ptr<CloudXRClient>& client);
    void OnCloudXRConnected();
#endif

    void SetSkyBox(int index);
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

    // PICO 2.2.0
    // https://developer-cn.pico-interactive.com/document/native/release-notes/
    // XrViewStatePICOEXT view_state_ = {};
    XrPosef headpose_ = {};

    larkxrControllerDeviceState controller_state_[LARKVR_TOTAL_CONTROLLER_COUNT];

#ifdef ENABLE_CLOUDXR
    std::shared_ptr<CloudXRClient> cloudxr_client_ = {};
#endif
};


#endif //CLOUDLARKXR_PVR_XR_SCENE_CLOUD_H
