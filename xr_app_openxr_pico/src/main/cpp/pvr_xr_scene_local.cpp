//
// Created by Administrator on 2021/7/7.
//

#include <log.h>
#include <application.h>
#include "pvr_xr_scene_local.h"
#include "pch.h"
#include "common.h"
#include "pvr_xr_utils.h"

PvrXRSceneLocal::PvrXRSceneLocal() = default;

PvrXRSceneLocal::~PvrXRSceneLocal() = default;

void PvrXRSceneLocal::InitGL(GraphicsDeviceAndroid *device) {
    PvrXRScene::InitGL(device);

    LOGV("obj size %ld", objects_.size());

    LOGV("skybox %p", sky_box_.get());

    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    PvrXRSceneLocal::AddObject(sky_box_);

    LOGV("skybox %p", sky_box_.get());

    lark::ControllerConfig controllerConfig = {};
    larkxrSystemInfo systemInfo = lark::XRClient::system_info();
    if (systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_2 ||
        systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_3) {
        controllerConfig = lark::CONTROLLER_PICO_NEO_2;
    } else  {
        controllerConfig = lark::CONTROLLER_PICO_NEO;
    }

//    controllerConfig = CONTROLLER_PICO_NEO;

    // controllers
    controller_left_ = std::make_shared<lark::Controller>(true, controllerConfig);
    controller_left_->Move(-0.3, 0, -0.3);
//     add to pvr_xr_scene;
    PvrXRSceneLocal::AddObject(controller_left_);

    controller_right_ = std::make_shared<lark::Controller>(false, controllerConfig);
    controller_right_->Move(0.3, 0, -0.3);
    // add to pvr_xr_scene;
    PvrXRSceneLocal::AddObject(controller_right_);

    // navigation.
    navigation_ = std::make_shared<Navigation>();
    PvrXRSceneLocal::AddObject(navigation_);

//    test_obj_.reset();
//    LOGV("test obj count %ld test obj %p", test_obj_.use_count(), test_obj_.get());
//    test_obj_ = std::make_shared<TestObj>();
//    test_obj_->Move(-0.3, 0, -2.3);
//    AddObject(test_obj_);

//    test_image_ = std::make_shared<Image>();
//    test_image_->SetPath("textures/skybox_8_2k.jpg", true);
//    test_image_->Move(-0.3, 0, -1.5);
//    AddObject(test_image_);
//    LOGV("test obj count %ld test obj %p", test_obj_.use_count(), test_obj_.get());
    LOGV("init gl finished");

    LOGV("obj size %ld", objects_.size());
}

void PvrXRSceneLocal::HandleInput(const InputState& input_state, const XrSession& session, const XrSpace& space) {
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
    PvrXRScene::HandleInput(input_state, session, space);
}

void PvrXRSceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}

void PvrXRSceneLocal::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}
