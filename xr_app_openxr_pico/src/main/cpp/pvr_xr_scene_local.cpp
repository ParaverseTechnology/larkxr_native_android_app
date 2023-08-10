//
// Created by Administrator on 2021/7/7.
//

#include <log.h>
#include <application.h>
#include <utils.h>
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

    lark::ControllerConfig controllerConfig = lark::CONTROLLER_PICO_NEO_3;

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
    navigation_ = std::make_shared<Navigation>(true);
//    navigation_->set_transform({
//        1, 0, 0, 0,
//        0, 1, 0, 0,
//        0, 0, 1, 0,
//        0, 0, -1,1,
//    });
//    navigation_->Move(0, 0, -1);
//    navigation_->Scale(0.2f);
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

        // WARNING xrLocateSpace time must valid timestamp
        uint64_t now = utils::GetTimestampNs();
        res = xrLocateSpace(input_state.handSpace[hand], space, now, &spaceLocation);

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
                if (isLeft) {
                    controller_left_->set_active(false);
                } else {
                    controller_right_->set_active(false);
                }
            }
            continue;
            LOGI("xrLocateSpace failed hand %d res %d active %d", hand, res, input_state.handActive[hand]);
        }


        if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
            (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {

            lark::Transform transform(pvr::toGlm(spaceLocation.pose.orientation), pvr::toGlm(spaceLocation.pose.position));
             transform.Rotate(glm::pi<float>() / 5, glm::vec3(-1, 0, 0));

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

        // LOGI("hand[%d] current state A=%d X=%d B=%d Y=%d", hand, AValue.currentState, XValue.currentState, BValue.currentState, YValue.currentState);

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

void PvrXRSceneLocal::SetSkyBox(int index) {
    const char *path = index == 0 ? "textures/skybox_8_2k.jpg" : "textures/skybox_9.jpg";
    sky_box_->SetTexture(path);
}
