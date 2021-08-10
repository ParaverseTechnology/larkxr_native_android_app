//
// Created by Administrator on 2021/7/7.
//
#include "pch.h"
#include <lark_xr/xr_client.h>
#include <log.h>
#include <lark_xr/app_list_task.h>
#include <application.h>
#include "pvr_xr_scene_cloud.h"
#include "check.h"
#include "pvr_xr_utils.h"

PvrXRSceneCloud::PvrXRSceneCloud() {

}

PvrXRSceneCloud::~PvrXRSceneCloud() {

}

void PvrXRSceneCloud::OnConnect() {
    LOGV("================OnConnect");
    if (loading_) {
        loading_->Enter();
    }
}

void PvrXRSceneCloud::OnMediaReady(int nativeTexture) {
    LOGV("================OnMediaReady %d", nativeTexture);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->SetMutiviewModeTexture(nativeTexture);
}

void PvrXRSceneCloud::OnMediaReady(int nativeTextrueLeft, int nativeTextureRight) {
    LOGV("================OnMediaReady %d %d", nativeTextrueLeft, nativeTextureRight);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    rect_texture_->SetStereoTexture(nativeTextrueLeft, nativeTextureRight);
}

void PvrXRSceneCloud::OnMediaReady() {
    LOGV("================OnMediaReady");
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}

void PvrXRSceneCloud::OnClose() {
    LOGV("================OnClose");
    controller_left_->set_active(true);
    controller_right_->set_active(true);
    rect_texture_->ClearTexture();
}

void PvrXRSceneCloud::InitGL(GraphicsDeviceAndroid *device) {
    PvrXRScene::InitGL(device);
    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    PvrXRScene::AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    PvrXRScene::AddObject(loading_);

    lark::ControllerConfig controllerConfig = {};
    larkxrSystemInfo systemInfo = lark::XRClient::system_info();
    if (systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_2 ||
        systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_3) {
        controllerConfig = lark::CONTROLLER_PICO_NEO_2;
    } else  {
        controllerConfig = lark::CONTROLLER_PICO_NEO;
    }
    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
    controller_left_->Move(-0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrXRScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
    controller_right_->Move(0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrXRScene::AddObject(controller_right_);

    rect_texture_ = std::make_shared<RectTexture>();
    PvrXRScene::AddObject(rect_texture_);
}

void PvrXRSceneCloud::HandleInput(const InputState &input_state, XrSession const &session,
                                  XrSpace const &space) {
    PvrXRScene::HandleInput(input_state, session, space);
    // 0->left, 1->right, 2->hmd
    lark::Ray rays[Input::RayCast_Count] = {};
    Input::InputState * inputState = Input::GetInputState();

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        bool isLeft = hand == Side::LEFT;
        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrResult res;
        res = xrLocateSpace(input_state.handSpace[hand], space, 10, &spaceLocation);
        CHECK_XRRESULT(res, "xrLocateSpace");
        if (!XR_UNQUALIFIED_SUCCESS(res)) {
            // Tracking loss is expected when the hand is not active so only log a message
            // if the hand is active.
            if (input_state.handActive[hand] == XR_TRUE) {
                const char* handName[] = {"left", "right"};
                Log::Write(Log::Level::Verbose,
                           Fmt("Unable to locate %s hand action space in app space: %d", handName[hand], res));
            } else {
                if (isLeft) {
                    controller_left_->set_active(false);
                } else {
                    controller_right_->set_active(false);
                }
            }
            continue;
        }
        if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
            (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {

            lark::Transform transform(pvr::toGlm(spaceLocation.pose.orientation), pvr::toGlm(spaceLocation.pose.position));

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
        }

        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        // reset all
        inputState[rayCastType] = {};

        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = input_state.grabAction;
        getInfo.subactionPath = input_state.handSubactionPath[hand];

        getInfo.action = input_state.BYAction;
        XrActionStateBoolean BYValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &BYValue));
        if (BYValue.isActive == XR_TRUE) {
            if (BYValue.currentState != XR_TRUE) {
                if (BYValue.changedSinceLastSync == XR_TRUE) {
                    inputState[rayCastType].backShortPressed = true;
                }
            } else {
                inputState[rayCastType].backButtonDown = true;
            }
        }

        getInfo.action = input_state.triggerAction;
        XrActionStateFloat triggerValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &triggerValue));
        if (triggerValue.isActive == XR_TRUE) {
            if (triggerValue.currentState == 255.0f) {
                if (triggerValue.changedSinceLastSync) {
                    // click
                    inputState[rayCastType].triggerShortPressed = true;
                } else {
                    inputState[rayCastType].triggerButtonDown = true;
                }
            }
        }

        getInfo.action = input_state.touchpadAction;
        XrActionStateBoolean touchpadValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &touchpadValue));
        if (touchpadValue.isActive == XR_TRUE) {
            if (touchpadValue.currentState != XR_TRUE) {
                if (touchpadValue.changedSinceLastSync == XR_TRUE) {
                    inputState[rayCastType].enterShortPressed = true;
                }
            } else {
                inputState[rayCastType].enterButtonDown = true;
            }
        }

        getInfo.action = input_state.AXAction;
        XrActionStateBoolean AXValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &AXValue));
        if (AXValue.isActive == XR_TRUE) {
            if (AXValue.currentState != XR_TRUE) {
                if (AXValue.changedSinceLastSync == XR_TRUE) { // click
                    inputState[rayCastType].enterShortPressed = true;
                }
            } else {
                inputState[rayCastType].enterButtonDown = true;
            }
        }

        //             call after pressup.
        if (inputState[rayCastType].triggerButtonDown && inputState[rayCastType].backShortPressed)
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
    PvrXRScene::HandleInput(input_state, session, space);
}

void PvrXRSceneCloud::OnCloseApp() {
    LOGV("================on close app");
    // 集中管控模式下由教师端控制退出应用。
    if (lark::AppListTask::run_mode() == lark::GetVrClientRunMode::ClientRunMode::CLIENT_RUNMODE_TEACHER) {
        return;
    }
    Application::instance()->CloseAppli();
}

void PvrXRSceneCloud::SetVideoFrame(const lark::XRVideoFrame &videoFrame) {
    LOGV("================SetVideoFrame");
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    }
}
