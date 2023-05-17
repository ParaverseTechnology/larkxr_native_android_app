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
//    HideMenu();
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

void PvrXRSceneCloud::InitGL(GraphicsDeviceAndroid *device) {
    PvrXRScene::InitGL(device);
    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    PvrXRScene::AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    PvrXRScene::AddObject(loading_);

    rect_texture_ = std::make_shared<RectTexture>();
    PvrXRScene::AddObject(rect_texture_);

    lark::ControllerConfig controllerConfig = lark::CONTROLLER_PICO_NEO_3;

    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
    controller_left_->Move(-0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrXRScene::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
    controller_right_->Move(0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrXRScene::AddObject(controller_right_);

    fake_hmd_ = std::make_shared<lark::Object>();
    PvrXRScene::AddObject(fake_hmd_);

    menu_view_ = std::make_shared<MenuView>(this);
    menu_view_->Move(-0.75, -0.75, -1.8);
    menu_view_->set_active(false);
    PvrXRScene::AddObject(menu_view_);
    fake_hmd_->AddChild(menu_view_);
}

void PvrXRSceneCloud::HandleInput(const InputState &input_state, XrSession const &session,
                                  XrSpace const &space) {
    PvrXRScene::HandleInput(input_state, session, space);

    bool backButtonDownThisFrame[Input::RayCast_Count] = {false, false};
    bool triggerDownThisFrame[Input::RayCast_Count] = {false, false};
    bool enterButtonDownThisFrame[Input::RayCast_Count] = {false, false};

    // 0->left, 1->right, 2->hmd
    lark::Ray rays[Input::RayCast_Count] = {};
    Input::InputState * inputState = Input::GetInputState();

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        bool isLeft = hand == Side::LEFT;
        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrResult res;

        // WARNING xrLocateSpace time must valid timestamp or 0
        res = xrLocateSpace(input_state.handSpace[hand], space, 0, &spaceLocation);

//        LOGI("hand xrLocateSpace hand %d res %d locationflags %ld active %d px %f py %f pz %f rx %f ry %f rz %f rw %f",
//             hand, res, spaceLocation.locationFlags, input_state.handActive[hand],
//             spaceLocation.pose.position.x, spaceLocation.pose.position.y, spaceLocation.pose.position.z,
//             spaceLocation.pose.orientation.x, spaceLocation.pose.orientation.y, spaceLocation.pose.orientation.z, spaceLocation.pose.orientation.w);

        CHECK_XRRESULT(res, "xrLocateSpace");
        if (!XR_UNQUALIFIED_SUCCESS(res)) {
            // Tracking loss is expected when the hand is not active so only log a message
            // if the hand is active.
            if (input_state.handActive[hand] == XR_TRUE) {
                const char* handName[] = {"left", "right"};
                Log::Write(Log::Level::Verbose,
                           Fmt("Unable to locate %s hand action space in app space: %d", handName[hand], res));
            } else {
//                if (isLeft) {
//                    controller_left_->set_active(false);
//                } else {
//                    controller_right_->set_active(false);
//                }
            }

            // save controller state;
            controller_state_[hand].inputState.isConnected = false;
            controller_state_[hand].pose.isValidPose = false;
            LOGI("xrLocateSpace failed hand %d res %d active %d", hand, res, input_state.handActive[hand]);

            continue;
        }


        if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
            (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {

            lark::Transform transform(pvr::toGlm(spaceLocation.pose.orientation), pvr::toGlm(spaceLocation.pose.position));
            transform.Rotate(glm::pi<float>() / 5, glm::vec3(-1, 0, 0));

            lark::Ray *ray = nullptr;
            if (isLeft) {
                controller_left_->set_transform(transform);
//                controller_left_->set_active(true);
                ray = &rays[0];
            } else {
                controller_right_->set_transform(transform);
//                controller_right_->set_active(true);
                ray = &rays[1];
            }
            ray->ori = transform.GetPosition();
            ray->dir = transform.Forward();
        } else {
            LOGV("pose not valid");
            continue;
        }

        Input::RayCastType rayCastType = isLeft ? Input::RayCast_left : Input::RayCast_Right;
        // reset all
        inputState[rayCastType] = {};

        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = input_state.grabAction;
        getInfo.subactionPath = input_state.handSubactionPath[hand];

        // button-> y
        getInfo.action = input_state.YAction;
        XrActionStateBoolean YValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &YValue));

        // button->b
        getInfo.action = input_state.BAction;
        XrActionStateBoolean BValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &BValue));
//        LOGI("BValue %d YValue %d", BValue.currentState, YValue.currentState);

        // button->x
        getInfo.action = input_state.XAction;
        XrActionStateBoolean XValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &XValue));

        // button->a
        getInfo.action = input_state.AAction;
        XrActionStateBoolean AValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &AValue));
//        LOGI("XValue %d AValue %d", XValue.currentState, AValue.currentState);

        getInfo.action = input_state.triggerAction;
        XrActionStateFloat triggerValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &triggerValue));
        if (triggerValue.isActive == XR_TRUE) {
//            float trigger = triggerValue.currentState;
//            if(trigger > 0.1f) {
//                //test controller Vibrate by  trigger key
//                pxr::Pxr_VibrateController(trigger, 20, hand);
//            }
            triggerDownThisFrame[rayCastType] = triggerValue.currentState == 1.0f;
        }
//        LOGI("trigger value %f type %d changed %d time %ld", triggerValue.currentState, triggerValue.type, triggerValue.changedSinceLastSync, triggerValue.lastChangeTime);

        backButtonDownThisFrame[rayCastType] = BValue.currentState || YValue.currentState;
        enterButtonDownThisFrame[rayCastType] = AValue.currentState || XValue.currentState;

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

        // save controller state;
        controller_state_[hand].deviceType = hand == Side::LEFT ? Larkxr_Controller_Left : Larkxr_Controller_Right;
        controller_state_[hand].pose = {};
        controller_state_[hand].pose.device = hand == Side::LEFT ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;
        controller_state_[hand].pose.isValidPose = true;
        controller_state_[hand].pose.isConnected = true;
        controller_state_[hand].pose.is6Dof = true;
        controller_state_[hand].pose.position = pvr::toGlm(spaceLocation.pose.position);
        controller_state_[hand].pose.rotation = pvr::toGlm(spaceLocation.pose.orientation);
        controller_state_[hand].rotateAxis = glm::vec3(-1, 0, 0);
        controller_state_[hand].rotateDeg = glm::half_pi<float>() / 2.5F;

        controller_state_[hand].inputState = pvr::toLarkvrInputState(hand, session, input_state);

        if (IsMenuActive()) {
            controller_state_[hand].pose.isConnected = false;
            controller_state_[hand].inputState.isConnected = false;
        }

//        LOGV("request left1 %d %d %d %f %f %f %f request right p %f %f %f r %f %f %f %f;h %f; %d %d",
//             controller_state_[hand].deviceType, controller_state_[hand].pose.isConnected, controller_state_[hand].inputState.isConnected,
//             controller_state_[hand].pose.rotation.x, controller_state_[hand].pose.rotation.y, controller_state_[hand].pose.rotation.z, controller_state_[hand].pose.rotation.w,
//             spaceLocation.pose.position.x, spaceLocation.pose.position.y, spaceLocation.pose.position.z,
//             spaceLocation.pose.orientation.x, spaceLocation.pose.orientation.y, spaceLocation.pose.orientation.z, spaceLocation.pose.orientation.w,
//             1.5f, controller_state_[hand].deviceType, controller_state_[hand].deviceType);
    }

    // update ui ray.
    PvrXRScene::HandleInput(input_state, session, space);

    if (menu_view_->active()) {
        // update input state.
        menu_view_->Update();
        menu_view_->HandleInput(rays, 2);
    }

    // LOGV("controller left %d right %d", controller_left_->active(), controller_right_->active());
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

void PvrXRSceneCloud::UpdateTexture(const lark::XRVideoFrame &videoFrame) {
    if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Multiview) {
        rect_texture_->SetMutiviewModeTexture(videoFrame.texture());
    } else if (videoFrame.frame_type() == lark::XRVideoFrame::FrameType::kNative_Stereo) {
        rect_texture_->SetStereoTexture(videoFrame.texture_left(), videoFrame.texture_right());
    }
}

void PvrXRSceneCloud::OnMenuViewSelect(bool submit) {
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

void PvrXRSceneCloud::ShowMenu() {
    LOGV("show menu");

    // PICO 2.2.0
    // https://developer-cn.pico-interactive.com/document/native/release-notes/
    // glm::quat rotate = pvr::toGlm(view_state_.headpose.orientation);
    // glm::vec3 position = pvr::toGlm(view_state_.headpose.position);
    glm::quat rotate = pvr::toGlm(headpose_.orientation);
    glm::vec3 position = pvr::toGlm(headpose_.position);

    fake_hmd_->set_transform(lark::Transform(rotate, position));
    menu_view_->set_active(true);
    controller_left_->set_active(true);
    controller_right_->set_active(true);
}

void PvrXRSceneCloud::HideMenu() {
    LOGV("hide menu menu");
    menu_view_->set_active(false);
    controller_left_->set_active(false);
    controller_right_->set_active(false);
}

#ifdef ENABLE_CLOUDXR
void PvrXRSceneCloud::SetCloudXRClient(const std::shared_ptr<CloudXRClient> &client) {
    if (!client) {
        return;
    }
    cloudxr_client_ = client;
    cloudxr_client_->set_active(false);
    AddObject(cloudxr_client_);
}

void PvrXRSceneCloud::OnCloudXRConnected() {
    loading_->set_active(false);
    rect_texture_->ClearTexture();
    controller_left_->set_active(false);
    controller_right_->set_active(false);
    sky_box_->set_active(false);
    cloudxr_client_->set_active(true);
}
#endif

void PvrXRSceneCloud::SetSkyBox(int index) {
    const char *path = index == 0 ? "textures/skybox_8_2k.jpg" : "textures/skybox_9.jpg";
    sky_box_->SetTexture(path);
}
