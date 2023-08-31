//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <utils.h>

#include <memory>
#include "xr_scene_local.h"
#include "hxr_utils.h"

namespace hxr {

XrSceneLocal::XrSceneLocal() :
        sky_box_(nullptr),
        controller_left_(nullptr),
        controller_right_(nullptr),
        navigation_(nullptr) {
}

XrSceneLocal::~XrSceneLocal() {

}

void XrSceneLocal::InitGL() {
    XrScene::InitGL();

    // test skybox
    // add to scenen;
     sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    AddObject(sky_box_);

    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, lark::CONTROLLER_HTC_FOCULS);
    controller_left_->Move(-0.3, 0, -0.3);
    // add to scene;
    AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, lark::CONTROLLER_HTC_FOCULS);
    controller_right_->Move(0.3, 0, -0.3);
    // add to scene;
    AddObject(controller_right_);

    // navigation.
     navigation_ = std::make_shared<Navigation>(true);
     AddObject(navigation_);
}

void XrSceneLocal::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}

void XrSceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}

void XrSceneLocal::LoadingPage() {
    LOGV("================LoadingPage");
    navigation_->SetRouter(Navigation::LOADING);
}

void XrSceneLocal::HandleInput(const InputState &input_state) {

    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // 0->left, 1->right, 2->hmd
    lark::Ray rays[Input::RayCast_Count] = {};
    Input::InputState * inputState = Input::GetInputState();

    for (int hand = 0; hand < 2; hand++) {
        bool isLeft = hand == 0;

        if(!input_state.ControllerActive[hand]) {
            if (isLeft) {
                controller_left_->set_active(false);
            } else {
                controller_right_->set_active(false);
            }
            continue;
        }

        lark::Transform transform(input_state.ControllerState[hand].pose.rotation.toGlm(),
                                  input_state.ControllerState[hand].pose.position.toGlm());
        // transform.Rotate(glm::pi<float>() / 5, glm::vec3(-1, 0, 0));

        lark::Ray *ray = nullptr;
        if (isLeft) {
            controller_left_->set_transform(transform);
            controller_left_->set_active(true);
            ray = &rays[0];
        } else {
            controller_right_->set_transform(transform);
            controller_right_->set_active(true);
            ray = &rays[1];
        }
        ray->ori = transform.GetPosition();
        ray->dir = transform.Forward();

        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        // reset all
        inputState[rayCastType] = {};

        if (input_state.m_isUse6DofProfile) {
            backButtonDownThisFrame[rayCastType] = input_state.BClick.currentState || input_state.YClick.currentState;
            enterButtonDownThisFrame[rayCastType] = input_state.AClick.currentState || input_state.XClick.currentState;
            triggerDownThisFrame[rayCastType] = input_state.TriggerClick[hand].currentState;
        } else {
            backButtonDownThisFrame[rayCastType] = input_state.BackClick.currentState;
            enterButtonDownThisFrame[rayCastType] = input_state.TrackPadClick.currentState;
            triggerDownThisFrame[rayCastType] = input_state.TriggerClick3Dof.currentState;
        }


        bool backButtonDownLastFrame = back_button_down_last_frame_[rayCastType];
        back_button_down_last_frame_[rayCastType] = backButtonDownThisFrame[rayCastType];

        bool traggerButtonDownLastFrame = trigger_button_down_last_frame_[rayCastType];
        trigger_button_down_last_frame_[rayCastType] = triggerDownThisFrame[rayCastType];

        bool enterButtonDownLastFrame = enter_button_down_last_frame_[rayCastType];
        enter_button_down_last_frame_[rayCastType] = enterButtonDownThisFrame[rayCastType];

        if (input_state.m_isUse6DofProfile) {
            inputState[rayCastType].backShortPressed = input_state.BClick.currentState || input_state.YClick.currentState;
            inputState[rayCastType].enterShortPressed = input_state.AClick.currentState || input_state.XClick.currentState;
            inputState[rayCastType].triggerShortPressed = input_state.TriggerClick[hand].currentState;
        } else {
            inputState[rayCastType].backShortPressed = input_state.BackClick.currentState;
            inputState[rayCastType].enterShortPressed = input_state.TrackPadClick.currentState;
            inputState[rayCastType].triggerShortPressed = input_state.TriggerClick3Dof.currentState;
        }

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
    XrScene::HandleInput(input_state);
}

void XrSceneLocal::SetSkyBox(int index) {
    const char *path = index == 0 ? "textures/skybox_8_2k.jpg" : "textures/skybox_9.jpg";
    sky_box_->SetTexture(path);
}
}