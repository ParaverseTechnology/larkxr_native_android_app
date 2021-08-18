//
// Created by Administrator on 2021/7/19.
//

#include <application.h>
#include <lark_xr/xr_latency_collector.h>
#include "xms_scene_cloud.h"

XmsSceneCloud::XmsSceneCloud() {

}

XmsSceneCloud::~XmsSceneCloud() {

}

void XmsSceneCloud::OnCloseApp() {
    LOGV("================on close app");
    // TODO 关闭应用
    Application::instance()->CloseAppli();
}

void XmsSceneCloud::InitGl(Svr::SvrApplicationContext *context) {
    XmsScene::InitGl(context);

    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
    XmsScene::AddObject(sky_box_);

    loading_ = std::make_shared<Loading>(nullptr);
    loading_->Move(View::VIEW_POSITION_Y, View::VIEW_POSITION_X, View::VIEW_POSITION_Z);
    loading_->SetQuitTips(L"长按头盔OK键返回应用列表");
    XmsScene::AddObject(loading_);
}

void XmsSceneCloud::UpdateHMDPose(const svrHeadPoseState &headPoseState) {
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

    if ((input_->IsKeyDown(Svr::kSvrKey_Volume_Up) || input_->IsKeyDown(Svr::kSvrKey_Volume_Down)) &&
        input_->IsKeyDownThisFrame(Svr::kSvrKey_OK)) {
        LOGI("recenter");
        svrRecenterOrientation();
    }

    if (input_->IsLongPress(Svr::kSvrKey_OK)) {
        LOGI("okkey long press");
        OnCloseApp();
    }
}

void XmsSceneCloud::Draw(int eye) {
    XmsScene::Draw(eye);
}

void XmsSceneCloud::Submit() {
    XmsScene::Submit();
}

void XmsSceneCloud::Submit(larkxrTrackingFrame trackingFrame) {
    svrFrameParams frameParams;
    memset(&frameParams, 0, sizeof(frameParams));
    frameParams.frameIndex = context_->frameCount;

    svrHeadPoseState headPoseTrackingFrame = pose_state_;

    headPoseTrackingFrame.pose.rotation.x = -trackingFrame.tracking.rotation.x;
    headPoseTrackingFrame.pose.rotation.y = -trackingFrame.tracking.rotation.y;
    headPoseTrackingFrame.pose.rotation.z = -trackingFrame.tracking.rotation.z;
    headPoseTrackingFrame.pose.rotation.w = trackingFrame.tracking.rotation.w;

    headPoseTrackingFrame.pose.position.x = -trackingFrame.tracking.position.x;
    headPoseTrackingFrame.pose.position.y = -trackingFrame.tracking.position.y;
    headPoseTrackingFrame.pose.position.z = -trackingFrame.tracking.position.z;


    glm::quat rotation;
    rotation.w = pose_state_.pose.rotation.w;
    rotation.x = pose_state_.pose.rotation.x;
    rotation.y = pose_state_.pose.rotation.y;
    rotation.z = pose_state_.pose.rotation.z;

    lark::XRLatencyCollector::Instance().Rendered2(trackingFrame.frameIndex);
    glm::vec3 renderAng = glm::eulerAngles(trackingFrame.tracking.rotation);
    glm::vec3 trackingAng = glm::eulerAngles(rotation);
    float degree = glm::degrees(renderAng.y - trackingAng.y);
    lark::XRLatencyCollector::Instance().Submit(trackingFrame.frameIndex, degree);


    frameParams.renderLayers[0].imageHandle = frame_texture_left_;
    frameParams.renderLayers[0].imageType = kTypeTexture;
    Svr::L_CreateLayout(0.0f, 0.0f, 1.0f, 1.0f, &frameParams.renderLayers[0].imageCoords);

    frameParams.renderLayers[0].eyeMask = kEyeMaskLeft;
    frameParams.renderLayers[0].layerFlags |= kLayerFlagHeadLocked;

    frameParams.renderLayers[1].imageHandle = frame_texture_right_;
    frameParams.renderLayers[1].imageType = kTypeTexture;
    Svr::L_CreateLayout(0.0f, 0.0f, 1.0f, 1.0f, &frameParams.renderLayers[1].imageCoords);

    frameParams.renderLayers[1].eyeMask = kEyeMaskRight;
    frameParams.renderLayers[1].layerFlags |= kLayerFlagHeadLocked;

    context_->eyeBufferIndex = (context_->eyeBufferIndex + 1) % SVR_NUM_EYE_BUFFERS;

    frameParams.headPoseState = headPoseTrackingFrame;
    frameParams.minVsyncs = 1;
    svrSubmitFrame(&frameParams);
}

void XmsSceneCloud::OnMediaReady(int nativeTextrueLeft, int nativeTextureRight) {
    frame_texture_left_ = nativeTextrueLeft;
    frame_texture_right_ = nativeTextureRight;
    sky_box_->set_active(false);
}

void XmsSceneCloud::OnConnect() {
    if (loading_) {
        loading_->Enter();
    }
}

void XmsSceneCloud::OnClose() {
    frame_texture_left_ = 0;
    frame_texture_right_ = 0;
    sky_box_->set_active(true);
}
