//
// Created by fcx on 2020/5/7.
//

#ifndef CLOUDLARKXR_WVR_SCENE_LOCAL_H
#define CLOUDLARKXR_WVR_SCENE_LOCAL_H

#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <skybox.h>
#include "ui/navigation.h"
#include "wvr_scene.h"
#include "rect_texture.h"

class WvrSceneLocal: public WvrScene {
public:
    using WvrScene::WvrScene;
    ~WvrSceneLocal();
    void InitVR();
    void SwitchInteractionMode();
    void SwitchGazeTriggerType();
    bool InitGL(void* left_eye_queue, void* right_eye_qeue,
                std::vector<WvrFrameBuffer*>* left_eye_fbo, std::vector<WvrFrameBuffer*>* right_eye_fbo) override;
    bool ShutdownGL() override;
    void HomePage();
protected:
    bool HandleInput() override;
private:
    void OnCloseApp();

    bool back_button_down_last_frame_[Input::RayCast_Count]{};
    bool trigger_button_down_last_frame_[Input::RayCast_Count]{};
    bool enter_button_down_last_frame_[Input::RayCast_Count]{};

    std::shared_ptr<lark::SkyBox> sky_box_{};
    std::shared_ptr<lark::Controller> controller_left_{};
    std::shared_ptr<lark::Controller> controller_right_{};

    std::shared_ptr<Navigation> navigation_{};

    WVR_InteractionMode interaction_mode_{};
    WVR_GazeTriggerType gaze_trigger_type_{};
};


#endif //CLOUDLARKXR_WVR_SCENE_LOCAL_H
