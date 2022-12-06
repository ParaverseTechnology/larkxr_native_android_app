//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <application.h>

#include <memory>
#include "xr_scene_cloud.h"

#define LOG_TAG "oxr_scene_cloud"

namespace oxr {
XrSceneCloud::XrSceneCloud() {

}

XrSceneCloud::~XrSceneCloud() {

}

void XrSceneCloud::InitGL() {
    XrScene::InitGL();

    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    AddObject(loading_);

    rect_texture_ = std::make_shared<RectTexture>();
    AddObject(rect_texture_);

    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, lark::CONTROLLER_OCULUS_QUEST);
    controller_left_->Move(-0.3, 0, -0.3);
    // add to scene;
    AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, lark::CONTROLLER_OCULUS_QUEST);
    controller_right_->Move(0.3, 0, -0.3);
    // add to scene;
    AddObject(controller_right_);

    fake_hmd_ = std::make_shared<lark::Object>();
    AddObject(fake_hmd_);

    menu_view_ = std::make_shared<MenuView>(this);
    menu_view_->Move(-0.75, -0.75, -1.8);
    menu_view_->set_active(false);
    AddObject(menu_view_);
    fake_hmd_->AddChild(menu_view_);
}

void XrSceneCloud::HandleInput(const InputState &input_state) {
    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // 0->left, 1->right, 2->hmd
    lark::Ray rays[Input::RayCast_Count] = {};
    Input::InputState * inputState = Input::GetInputState();

    for (int hand = 0; hand < 2; hand++) {
        bool isLeft = hand == 0;

        if(!input_state.ControllerActive[hand]) {
            continue;
        }

        lark::Transform transform(toGlm(input_state.ControllerPose[hand].pose.orientation), toGlm(input_state.ControllerPose[hand].pose.position));
        // transform.Rotate(glm::pi<float>() / 5, glm::vec3(-1, 0, 0));

        lark::Ray *ray = nullptr;
        if (isLeft) {
            controller_left_->set_transform(transform);
            // controller_left_->set_active(true);
            ray = &rays[0];
        } else {
            controller_right_->set_transform(transform);
            // controller_right_->set_active(true);
            ray = &rays[1];
        }
        ray->ori = transform.GetPosition();
        ray->dir = transform.Forward();

        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        // reset all
        inputState[rayCastType] = {};

        backButtonDownThisFrame[rayCastType] = input_state.BClick.currentState || input_state.YClick.currentState;
        enterButtonDownThisFrame[rayCastType] = input_state.AClick.currentState || input_state.XClick.currentState;
        triggerDownThisFrame[rayCastType] = input_state.TriggerClick[hand].currentState;

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

        // short press backbutton
        if (inputState[rayCastType].backShortPressed) {
            if (menu_view_->active()) {
                HideMenu();
            }
        }

        //             call after pressup.
        if (inputState[rayCastType].triggerButtonDown && inputState[rayCastType].backShortPressed)
        {
            LOGV("close app." );
            if (lark::AppListTask::run_mode() == lark::GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_SELF) {
                // 显示退出菜单
                ShowMenu();
            }
        }

        // call ater pressup.
        if (inputState[rayCastType].triggerShortPressed) {
//                LOGV( "trigger button short press %d", rayCastType);
            Input::SetCurrentRayCastType(rayCastType);
        }
    }

    if (menu_view_->active()) {
        // update input state.
        menu_view_->Update();
        menu_view_->HandleInput(rays, 2);
    }
    // update ui ray.
    XrScene::HandleInput(input_state);
}

void XrSceneCloud::OnConnect() {
    LOGV("================OnConnect");
    if (loading_) {
        loading_->Enter();
    }
    HideMenu();
}

void XrSceneCloud::SetVideoFrame(const lark::XRVideoFrame &videoFrame) {
    LOGV("================SetVideoFrame");
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    }
}

void XrSceneCloud::OnMediaReady(int nativeTexture) {
    LOGV("================OnMediaReady %d", nativeTexture);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->SetMutiviewModeTexture(nativeTexture);
}

void XrSceneCloud::OnMediaReady(int nativeTextrueLeft, int nativeTextureRight) {
    LOGV("================OnMediaReady %d %d", nativeTextrueLeft, nativeTextureRight);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->SetStereoTexture(nativeTextrueLeft, nativeTextureRight);
}

void XrSceneCloud::OnMediaReady() {
    LOGV("================OnMediaReady");
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}

void XrSceneCloud::UpdateTexture(const lark::XRVideoFrame &videoFrame) {
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    }
}

void XrSceneCloud::OnClose() {
    LOGV("================OnClose");
    loading_->set_active(true);
    menu_view_->set_active(false);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
    sky_box_->set_active(true);
    rect_texture_->ClearTexture();
#ifdef ENABLE_CLOUDXR
    cloudxr_client_->set_active(false);
#endif
}

void XrSceneCloud::OnMenuViewSelect(bool submit) {
    LOGV("OnMenuViewSelect %d", submit);
    if (submit) {
        // 用户选择退出
        OnCloseApp();
        HideMenu();
    } else {
        // 用户选择取消
        HideMenu();
    }
}

void XrSceneCloud::ShowMenu() {
    LOGV("show menu");
    glm::quat rotate = toGlm(headpose_.orientation);
    glm::vec3 position = toGlm(headpose_.position);
    fake_hmd_->set_transform(lark::Transform(rotate, position));
    menu_view_->set_active(true);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
}

void XrSceneCloud::HideMenu() {
    LOGV("hide menu menu");
    menu_view_->set_active(false);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}

void XrSceneCloud::OnCloseApp() {
    LOGV("================on close app");
    // 集中管控模式下由教师端控制退出应用。
    if (lark::AppListTask::run_mode() == lark::GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_TEACHER) {
        return;
    }
    Application::instance()->CloseAppli();
}
#ifdef ENABLE_CLOUDXR
void XrSceneCloud::SetCloudXRClient(const std::shared_ptr<CloudXRClient> &client) {
    if (!client) {
        return;
    }
    cloudxr_client_ = client;
    cloudxr_client_->set_active(false);
    AddObject(cloudxr_client_);
}

void XrSceneCloud::OnCloudXRConnected() {
    loading_->set_active(false);
    rect_texture_->ClearTexture();
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    sky_box_->set_active(false);
    cloudxr_client_->set_active(true);
}
#endif

void XrSceneCloud::SetSkyBox(int index) {
    const char *path = index == 0 ? "textures/skybox_8_2k.jpg" : "textures/skybox_9.jpg";
    sky_box_->SetTexture(path);
}
}

