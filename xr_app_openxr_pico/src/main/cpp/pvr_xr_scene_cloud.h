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

class PvrXRSceneCloud : public PvrXRScene {
public:
    PvrXRSceneCloud();
    ~PvrXRSceneCloud();

    void OnConnect();
    void OnMediaReady(int nativeTexture);
    void OnMediaReady(int nativeTextrueLeft, int nativeTextureRight);
    void OnMediaReady();
    void OnClose();

    virtual void InitGL(GraphicsDeviceAndroid *device) override;

    virtual void HandleInput(const InputState& input_state, const XrSession& session, const XrSpace& space) override;

    virtual void SetVideoFrame(const lark::XRVideoFrame& videoFrame);
private:
    void OnCloseApp();

    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
    std::shared_ptr<Loading> loading_ = nullptr;
    std::shared_ptr<lark::Controller> controller_left_ = nullptr;
    std::shared_ptr<lark::Controller> controller_right_ = nullptr;

    std::shared_ptr<RectTexture> rect_texture_{};
};


#endif //CLOUDLARKXR_PVR_XR_SCENE_CLOUD_H
