//
// Created by fcx on 2020/5/7.
//

#include <log.h>
#include <env_context.h>
#include <wvr/wvr_render.h>
#include <application.h>
#include "wvr_scene_local.h"
#include "wvr_utils.h"
#define LOG_TAG "wvr_scene_local"

WvrSceneLocal::~WvrSceneLocal() = default;

void WvrSceneLocal::InitVR() {
    interaction_mode_ = WVR_GetInteractionMode();
    gaze_trigger_type_ = WVR_GetGazeTriggerType();
}

void WvrSceneLocal::SwitchInteractionMode() {
    if (interaction_mode_ == WVR_InteractionMode_SystemDefault) {
        interaction_mode_ = WVR_InteractionMode_Gaze;
    } else if (interaction_mode_ == WVR_InteractionMode_Gaze) {
        interaction_mode_ = WVR_InteractionMode_Controller;
    } else if (interaction_mode_ == WVR_InteractionMode_Controller) {
        interaction_mode_ = WVR_InteractionMode_SystemDefault;
    }
    LOGD("switchInteractionMode interaction_mode_: %d", interaction_mode_);
    WVR_SetInteractionMode(interaction_mode_);
}

void WvrSceneLocal::SwitchGazeTriggerType() {
    if (gaze_trigger_type_ == WVR_GazeTriggerType_Timeout) {
        gaze_trigger_type_ = WVR_GazeTriggerType_Button;
    } else if (gaze_trigger_type_ == WVR_GazeTriggerType_Button) {
        gaze_trigger_type_ = WVR_GazeTriggerType_TimeoutButton;
    } else if (gaze_trigger_type_ == WVR_GazeTriggerType_TimeoutButton) {
        gaze_trigger_type_ = WVR_GazeTriggerType_Timeout;
    }
    LOGD("switchGazeTriggerType gaze_trigger_type_: %d", gaze_trigger_type_);
    WVR_SetGazeTriggerType(gaze_trigger_type_);
}

bool WvrSceneLocal::InitGL(void* left_eye_queue, void* right_eye_qeue,
                           std::vector<WvrFrameBuffer*>* left_eye_fbo, std::vector<WvrFrameBuffer*>* right_eye_fbo) {
    // test skybox
    // add to scenen;
    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
//    sky_box_->setActive(true);
    WvrScene::AddObject(sky_box_);

    larkxrSystemInfo info = lark::XRClient::system_info();
    lark::ControllerConfig controllerConfig = lark::CONTROLLER_HTC_FOCULS;
    if (info.platFromType == Larkxr_Platform_HTC_FOCUS_PLUS) {
        controllerConfig = lark::CONTROLLER_HTC_FOCUS_PLUS;
    } else if (info.platFromType == Larkxr_Platform_HTC_FOCUS) {
        controllerConfig = lark::CONTROLLER_HTC_FOCULS;
    } else if (info.platFromType == Larkxr_Platform_PICO_NEO) {
        controllerConfig = lark::CONTROLLER_PICO_NEO;
    }
    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
//    controller_left_->Move(-0.3, 0, -0.3);
    controller_left_->set_active(false);
    // add to scene;
    WvrScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
//    controller_right_->Move(0.3, 0, -0.3);
    controller_right_->set_active(false);
    // add to scene;
    WvrScene::AddObject(controller_right_);

    // navigation.
    navigation_ = std::make_shared<Navigation>();
    WvrScene::AddObject(navigation_);
    return WvrScene::InitGL(left_eye_queue, right_eye_qeue, left_eye_fbo, right_eye_fbo);
}

bool WvrSceneLocal::ShutdownGL() {
    return WvrScene::ShutdownGL();
}

bool WvrSceneLocal::HandleInput() {
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // update global input state.
    Input::InputState * inputState = Input::GetInputState();
    // save ray
    // 0->left, 1->right.
    lark::Ray rays[Input::RayCast_Count] = {};
    glm::mat4 hmdMat{};
    for (auto devicePose: vr_device_pairs_) {
        if (devicePose.type == WVR_DeviceType_HMD) {
            // skip hmd
            hmdMat = wvr::toGlm(devicePose.pose.poseMatrix);
            continue;
        }
        lark::Ray *ray = nullptr;
        lark::Controller* controller;
        Input::RayCastType rayCastType;
        if (devicePose.type == WVR_DeviceType_Controller_Left) {
            ray = &rays[0];
            controller = controller_left_.get();
            rayCastType = Input::RayCast_left;
        } else {
            ray = &rays[1];
            controller = controller_right_.get();
            rayCastType = Input::RayCast_Right;
        }
        if (!controller) {
            // skip empyt controller
            continue;
        }
        if (!devicePose.pose.isValidPose) {
            controller->set_active(false);
            continue;
        }
        glm::mat4 mat = wvr::toGlm(devicePose.pose.poseMatrix);
        // set arm mode when 3dof.
        lark::Transform transform(mat);
        controller->set_active(true);
        controller->set_transform(transform);

        ray->ori = transform.GetPosition();
        ray->dir = transform.Forward();

        // input state
        if (WVR_GetInputButtonState(devicePose.type, WVR_InputId_Alias1_Menu)) {
            LOGI("Controller device Menu button was pressed");
            backButtonDownThisFrame[rayCastType] = true;
        } else {
            backButtonDownThisFrame[rayCastType] = false;
        }
        if (WVR_GetInputButtonState(devicePose.type, WVR_InputId_Alias1_Touchpad)) {
            LOGI("Controller device dpad button was pressed");
            enterButtonDownThisFrame[rayCastType] = true;
        } else {
            enterButtonDownThisFrame[rayCastType] = false;
        }

        if (WVR_GetInputButtonState(devicePose.type, WVR_InputId_Alias1_Trigger)
            || WVR_GetInputButtonState(devicePose.type, WVR_InputId_Alias1_Digital_Trigger)) {
            LOGI("Controller device Tragger button was pressed");
            triggerDownThisFrame[rayCastType] = true;
        } else {
            triggerDownThisFrame[rayCastType] = false;
        }

        bool backButtonDownLastFrame = back_button_down_last_frame_[rayCastType];
        back_button_down_last_frame_[rayCastType] = backButtonDownThisFrame[rayCastType];

        bool traggerButtonDownLastFrame = trigger_button_down_last_frame_[rayCastType];
        trigger_button_down_last_frame_[rayCastType] = triggerDownThisFrame[rayCastType];

        bool enterButtonDownLastFrame = enter_button_down_last_frame_[rayCastType];
        enter_button_down_last_frame_[rayCastType] = enterButtonDownThisFrame[rayCastType];

        inputState[rayCastType].backShortPressed = backButtonDownLastFrame && !backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterShortPressed = enterButtonDownLastFrame && !enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerShortPressed = traggerButtonDownLastFrame && !triggerDownThisFrame[rayCastType];
        inputState[rayCastType].backButtonDown = backButtonDownThisFrame[rayCastType];
        inputState[rayCastType].enterButtonDown = enterButtonDownThisFrame[rayCastType];
        inputState[rayCastType].triggerButtonDown = triggerDownThisFrame[rayCastType];

        //             call after pressup.
        if ( inputState[rayCastType].backShortPressed)
        {
//                LOGV( "back button short press %d", rayCastType);
            OnCloseApp();
        }

        // call ater pressup.
        if (inputState[rayCastType].triggerShortPressed) {
//                LOGV( "trigger button short press %d", rayCastType);
            Input::SetCurrentRayCastType(rayCastType);
        }
    }
    // update ui ray.
    navigation_->HandelInput(rays, 2);
    return WvrScene::HandleInput();
}


void WvrSceneLocal::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}

void WvrSceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}
