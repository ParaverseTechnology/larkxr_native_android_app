//
// Created by Administrator on 2021/7/7.
//

#ifndef CLOUDLARKXR_PVR_XR_SCENE_LOCAL_H
#define CLOUDLARKXR_PVR_XR_SCENE_LOCAL_H

#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <test_obj.h>
#include "ui/navigation.h"
#include "skybox.h"
#include "pvr_xr_scene.h"
#include "xr_input_state.h"

class PvrXRSceneLocal: public PvrXRScene {
public:
    PvrXRSceneLocal();
    ~PvrXRSceneLocal();

    virtual void InitGL(GraphicsDeviceAndroid *device) override;

    virtual void HandleInput(const InputState& input_state, const XrSession& session, const XrSpace& space) override;

    void HomePage();
private:
    void OnCloseApp();

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
    std::shared_ptr<lark::Controller> controller_left_ = nullptr;
    std::shared_ptr<lark::Controller> controller_right_ = nullptr;
    std::shared_ptr<Navigation> navigation_ = nullptr;
    std::shared_ptr<TestObj> test_obj_ = nullptr;
    std::shared_ptr<Image> test_image_ = nullptr;
};


#endif //CLOUDLARKXR_PVR_XR_SCENE_LOCAL_H
