//
// Created by Administrator on 2021/7/15.
//

#include <log.h>
#include <lark_xr/xr_client.h>
#include <application.h>
#include "xms_scene_local.h"

XmsSceneLocal::XmsSceneLocal() {

}

XmsSceneLocal::~XmsSceneLocal() {

}

void XmsSceneLocal::InitGl(Svr::SvrApplicationContext *context) {
    XmsScene::InitGl(context);

    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    XmsScene::AddObject(sky_box_);

    // navigation.
    navigation_ = std::make_shared<Navigation>();
    navigation_->SetLoadingTips(L"长按头盔OK键返回应用列表");
    XmsScene::AddObject(navigation_);
    LOGV("init gl finished");
}

void XmsSceneLocal::UpdateHMDPose(const svrHeadPoseState &headPoseState) {
    XmsScene::UpdateHMDPose(headPoseState);
    lark::Ray rays[Input::RayCast_Count] = {};
    for (int i = 0; i < Input::RayCast_Count; i ++) {
        if (i == Input::RayCast_left || i == Input::RayCast_Right) {
            continue;
        }
        Input::RayCastType rayCastType = (Input::RayCastType)i;
        glm::quat rotation(headPoseState.pose.rotation.w,
                -headPoseState.pose.rotation.x,
                -headPoseState.pose.rotation.y,
                -headPoseState.pose.rotation.z);
        glm::vec3 position(-headPoseState.pose.position.x,
                -headPoseState.pose.position.y,
                -headPoseState.pose.position.z);

        lark::Transform transform(rotation, position);

        rays[rayCastType].ori = transform.GetPosition();
        rays[rayCastType].dir = transform.Forward();
    }

    // update global input state.
    Input::InputState * inputState = Input::GetInputState();

    // reset all state.
    inputState[Input::RayCast_Hmd] = {};

    bool is_long_pressed = false;
    if (input_->IsKeyUpThisFrame(Svr::kSvrKey_OK, &is_long_pressed)) {
        if (!is_long_pressed) {
            LOGI("okkey up");
            inputState[Input::RayCast_Hmd].enterShortPressed = true;
        } else {
            LOGI("okkey long press up trigged");
        }
    }

    if (input_->IsLongPress(Svr::kSvrKey_OK)) {
        LOGI("okkey long press");
        OnCloseApp();
    }

    if (input_->IsKeyUpThisFrame(Svr::kSvrKey_Volume_Up, &is_long_pressed)) {
        LOGI("volume key up");
    }

    if (input_->IsKeyUpThisFrame(Svr::kSvrKey_Volume_Down, &is_long_pressed)) {
        LOGI("volume key down");
    }
    if ((input_->IsKeyDown(Svr::kSvrKey_Volume_Up) || input_->IsKeyDown(Svr::kSvrKey_Volume_Down)) &&
        input_->IsKeyDownThisFrame(Svr::kSvrKey_OK)) {
        LOGI("recenter");
        svrRecenterOrientation();
    }    // update ui ray.
    navigation_->HandelInput(rays, Input::RayCast_Count);
}

void XmsSceneLocal::Draw(int eye) {
    XmsScene::Draw(eye);
    GLenum err = glGetError();
    if (err == GL_OUT_OF_MEMORY) {
        LOGE("glGetError(): GL_OUT_OF_MEMORY");
    } else if (err != GL_NO_ERROR) {
        LOGE("glGetError(): %d", err);
    } else {
//        LOGV("gl no error");
    }
}

void XmsSceneLocal::HomePage() {
    LOGV("================HomePage");
    navigation_->SetRouter(Navigation::HOME);
}

void XmsSceneLocal::HandleInput() {

}

void XmsSceneLocal::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}

void XmsSceneLocal::Submit() {
    XmsScene::Submit();
}
