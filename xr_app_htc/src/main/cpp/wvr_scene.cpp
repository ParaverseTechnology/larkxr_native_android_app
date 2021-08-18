//
// Created by fcx on 2020/5/7.
//

#include <log.h>
#include <wvr/wvr_render.h>
#include <wvr/wvr_device.h>
#include "wvr/wvr_projection.h"
#include <utils.h>
#include <ui/component/base.h>
#include "wvr_scene.h"
#include "wvr_utils.h"

static void dumpMatrix(const char * name, const glm::mat4& mat) {
    LOGV("%s =\n"
         " ⎡%+6f  %+6f  %+6f  %+6f⎤\n"
         " ⎢%+6f  %+6f  %+6f  %+6f⎥\n"
         " ⎢%+6f  %+6f  %+6f  %+6f⎥\n"
         " ⎣%+6f  %+6f  %+6f  %+6f⎦\n",
         name,
         mat[0][0], mat[1][0], mat[2][0],  mat[3][0],
         mat[0][1], mat[1][1], mat[2][1],  mat[3][1],
         mat[0][2], mat[1][2], mat[2][2],  mat[3][2],
         mat[0][3], mat[1][3], mat[2][3],  mat[3][3]);
}

WvrScene::WvrScene()
{
};

WvrScene::~WvrScene() = default;

bool WvrScene::InitGL(void* left_eye_queue, void* right_eye_qeue,
                      std::vector<WvrFrameBuffer*>* left_eye_fbo, std::vector<WvrFrameBuffer*>* right_eye_fbo) {
    if (left_eye_queue == nullptr || right_eye_qeue == nullptr || left_eye_fbo == nullptr || right_eye_fbo == nullptr) {
        return false;
    }

    left_eye_q_ = left_eye_queue;
    right_eye_q_ = right_eye_qeue;
    left_eye_fbo_ = left_eye_fbo;
    right_eye_fbo_ = right_eye_fbo;

    near_clip_ = 0.1f;
    far_clip_ = 50.0f;
    // setup cameras.
    SetupCameras();

    index_left_ = 0;
    index_right_ = 0;

    // Setup stereo render targets
//    WVR_GetRenderTargetSize(&render_width_, &render_height_);
//    LOGD("Recommended size is %ux%u", render_width_, render_height_);
//    if (render_width_ == 0 || render_height_ == 0) {
//        LOGE("Please check server configure");
//        return false;
//    }

//    left_eye_q_ = WVR_ObtainTextureQueue(WVR_TextureTarget_2D, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, render_width_, render_height_, 0);
//    for (int i = 0; i < WVR_GetTextureQueueLength(left_eye_q_); i++) {
//        WvrFrameBuffer* fbo = new WvrFrameBuffer((int)(long)WVR_GetTexture(left_eye_q_, i).id, render_width_, render_height_, true);
//        if (fbo->has_error()) return false;
//        left_eye_fbo_.push_back(fbo);
//    }
//    right_eye_q_ = WVR_ObtainTextureQueue(WVR_TextureTarget_2D, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, render_width_, render_height_, 0);
//    for (int i = 0; i < WVR_GetTextureQueueLength(right_eye_q_); i++) {
//        WvrFrameBuffer* fbo = new WvrFrameBuffer((int)(long)WVR_GetTexture(right_eye_q_, i).id, render_width_, render_height_, true);
//        if (fbo->has_error()) return false;
//        right_eye_fbo_.push_back(fbo);
//    }
    return true;
}

bool WvrScene::ShutdownGL() {
    LOGENTRY();
    left_eye_q_ = nullptr;
    right_eye_q_ = nullptr;
    left_eye_fbo_ = nullptr;
    right_eye_fbo_ = nullptr;
//    if (left_eye_q_ != 0) {
//        for (int i = 0; i < WVR_GetTextureQueueLength(left_eye_q_); i++) {
//            delete left_eye_fbo_.at(i);
//        }
//        WVR_ReleaseTextureQueue(left_eye_q_);
//    }
//    if (right_eye_q_ != 0) {
//        for (int i = 0; i < WVR_GetTextureQueueLength(right_eye_q_); i++) {
//            delete right_eye_fbo_.at(i);
//        }
//        WVR_ReleaseTextureQueue(right_eye_q_);
//    }
    return true;
}

bool WvrScene::HandleInput() {
    // handle recenter.
    return true;
}

void WvrScene::Update() {
    UpdateHMDMatrixPose();
    HandleInput();
    // call objecs Update.
    for(auto & object : objects_) {
        if (object.get()->active())
            object.get()->Update();
    }
    Render();
}

bool WvrScene::Render() {
    index_left_ = WVR_GetAvailableTextureIndex(left_eye_q_);
    index_right_ = WVR_GetAvailableTextureIndex(right_eye_q_);
    RenderStereoTargets();

    // Left eye
    WVR_TextureParams_t leftEyeTexture = WVR_GetTexture(left_eye_q_, index_left_);
    WVR_SubmitError e;
    e = WVR_SubmitFrame(WVR_Eye_Left, &leftEyeTexture, &vr_device_pairs_[WVR_DEVICE_HMD].pose);
    if (e != WVR_SubmitError_None) return true;

    // Right eye
    WVR_TextureParams_t rightEyeTexture = WVR_GetTexture(right_eye_q_, index_right_);
    e = WVR_SubmitFrame(WVR_Eye_Right, &rightEyeTexture, &vr_device_pairs_[WVR_DEVICE_HMD].pose);
    if (e != WVR_SubmitError_None) return true;
    UpdateTime();
    // Clear
    {
        // We want to make sure the glFinish waits for the entire present to complete, not just the submission
        // of the command. So, we do a clear here right here so the glFinish will wait fully for the swap.
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    return true;
}

void WvrScene::RenderStereoTargets() {
    glClearColor(0.30f, 0.30f, 0.37f, 1.0f); // nice background color, but not black
    WvrFrameBuffer * fbo = NULL;

    // Left Eye
    fbo = left_eye_fbo_->at(index_left_);
    fbo->BindFrameBuffer(false);
    fbo->GlViewportFull();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderScene(WVR_Eye_Left);
    fbo->UnbindFrameBuffer(false);

    // Right Eye
    fbo = right_eye_fbo_->at(index_right_);
    fbo->BindFrameBuffer(false);
    fbo->GlViewportFull();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderScene(WVR_Eye_Right);
    fbo->UnbindFrameBuffer(false);
}

void WvrScene::RenderScene(WVR_Eye nEye) {
    WVR_RenderMask(nEye);
    lark::Object::Eye objEye{};
    glm::mat4* project = nullptr;
    glm::mat4 eyeView{};
    if (nEye == WVR_Eye_Left) {
        objEye = lark::Object::EYE_LEFT;
        project = &projection_left_;
        eyeView = eye_pos_left_ * hmd_pose_;
    } else {
        objEye = lark::Object::EYE_RIGHT;
        project = &projection_right_;
        eyeView = eye_pos_right_ * hmd_pose_;
    }
    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(objEye, *project, eyeView);
        }
    }
}

void WvrScene::UpdateHMDMatrixPose() {
    WVR_GetSyncPose(WVR_PoseOriginModel_OriginOnHead, vr_device_pairs_, WVR_DEVICE_COUNT_LEVEL_1);

    valid_pose_count_ = 0;
    pose_classes_ = "";
    for (int nDevice = 0; nDevice < WVR_DEVICE_COUNT_LEVEL_1; ++nDevice) {
        if (vr_device_pairs_[nDevice].pose.isValidPose) {
            valid_pose_count_++;
            device_pose_array_[nDevice] = wvr::toGlm(vr_device_pairs_[nDevice].pose.poseMatrix);

            if (dev_class_char_[nDevice]==0) {
                switch (WVR_DeviceType_HMD + nDevice) {
                    case WVR_DeviceType_HMD:
                        dev_class_char_[nDevice] = 'H';
                        break;
                    case WVR_DeviceType_Controller_Right:
                        dev_class_char_[nDevice] = 'R';
                        break;
                    case WVR_DeviceType_Controller_Left:
                        dev_class_char_[nDevice] = 'L';
                        break;
                    default:
                        dev_class_char_[nDevice] = '?';
                        break;
                }
            }
            pose_classes_ += dev_class_char_[nDevice];
        }
    }

    if (vr_device_pairs_[WVR_DEVICE_HMD].pose.isValidPose) {
        UpdateEyeToHeadMatrix(vr_device_pairs_[WVR_DEVICE_HMD].pose.is6DoFPose);
        glm::mat4 hmd = device_pose_array_[WVR_DEVICE_HMD];

        // The controller make the sample not simple.  If you don't have
        // a controller, we just need invert the hmd pose.

        // When the head turn left, acturally the object turn right.
        // When the head move left, acturally the object move right.
        // So we need invert the hmd matrix.
        hmd_pose_ = glm::inverse<4, 4, float, glm::defaultp>(hmd);
    }
}

void WvrScene::UpdateEyeToHeadMatrix(bool is6DoF) {
    if (is6DoF != is_6dof_pose_) {
        if(is6DoF) {
            eye_pos_left_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(WVR_GetTransformFromEyeToHead(WVR_Eye_Left, WVR_NumDoF_6DoF)));
            eye_pos_right_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(WVR_GetTransformFromEyeToHead(WVR_Eye_Right, WVR_NumDoF_6DoF)));
        } else {
            eye_pos_left_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(WVR_GetTransformFromEyeToHead(WVR_Eye_Left, WVR_NumDoF_3DoF)));
            eye_pos_right_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(WVR_GetTransformFromEyeToHead(WVR_Eye_Right, WVR_NumDoF_3DoF)));
        }

    }
    is_6dof_pose_ = is6DoF;
}

void WvrScene::SetupCameras() {
    LOGI("**SetupCameras**");
    projection_left_ = wvr::toGlm(WVR_GetProjection(WVR_Eye_Left, near_clip_, far_clip_));
    projection_right_ = wvr::toGlm(WVR_GetProjection(WVR_Eye_Right, near_clip_, far_clip_));

    eye_pos_left_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(WVR_GetTransformFromEyeToHead(WVR_Eye_Left)));
    eye_pos_right_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(WVR_GetTransformFromEyeToHead(WVR_Eye_Right)));

    dumpMatrix("ProjectionLeft", projection_left_);
    dumpMatrix("ProjectionRight", projection_right_);
    dumpMatrix("EyePosLeft", eye_pos_left_);
    dumpMatrix("EyePosRight", eye_pos_right_);

    // Initial position need a little backward and upper to avoid been in a cube.
    world_translation_ = glm::translate(world_translation_,
            glm::vec3(component::CAMERA_ORI_POSITION.x, component::CAMERA_ORI_POSITION.y, component::CAMERA_ORI_POSITION.z));

    gettimeofday(&rtc_time_, nullptr);

    {
        float left, right, top, bottom;
        float aLeft, aRight, aTop, aBottom;
        WVR_GetClippingPlaneBoundary(WVR_Eye_Left, &left, &right, &top, &bottom);
        aLeft = static_cast<float>(atanf(left) * 180.0f / M_PI);
        aRight = static_cast<float>(atanf(right) * 180.0f / M_PI);
        aTop = static_cast<float>(atanf(top) * 180.0f / M_PI);
        aBottom = static_cast<float>(atan(bottom) * 180.0f / M_PI);
        LOGV("L left: %f; right: %f; top: %f; bottom: %f;aLeft: %f; aRight: %f; aTop: %f; aBotttom: %f",
             left, right, top, bottom,
             aLeft, aRight, aTop, aBottom);

        WVR_GetClippingPlaneBoundary(WVR_Eye_Right, &left, &right, &top, &bottom);
        aLeft = static_cast<float>(atanf(left) * 180.0f / M_PI);
        aRight = static_cast<float>(atanf(right) * 180.0f / M_PI);
        aTop = static_cast<float>(atanf(top) * 180.0f / M_PI);
        aBottom = static_cast<float>(atan(bottom) * 180.0f / M_PI);
        LOGV("L left: %f; right: %f; top: %f; bottom: %f;aLeft: %f; aRight: %f; aTop: %f; aBotttom: %f",
             left, right, top, bottom,
             aLeft, aRight, aTop, aBottom);
    }
    LOGI("**SetupCamerasFinish**");
}

void WvrScene::UpdateTime() {
    // Process time variable.
    struct timeval now;
    gettimeofday(&now, nullptr);

    clock_count_++;
    if (rtc_time_.tv_usec > now.tv_usec)
        clock_count_ = 0;
    if (clock_count_ >= VR_MAX_CLOCKS)
        clock_count_--;

    uint32_t timeDiff = timeval_subtract(now, rtc_time_);
    time_accumulator_2s_ += timeDiff;
    rtc_time_ = now;
    frame_count_++;
    if (time_accumulator_2s_ > 2000000) {
        fps_ = frame_count_ / (time_accumulator_2s_ / 1000000.0f);
        LOGI("WvrScene FPS %3.0f", fps_);

        frame_count_ = 0;
        time_accumulator_2s_ = 0;
    }
}

void WvrScene::ProcessVREvent(const WVR_Event_t & event) {
    switch(event.common.type) {
        case WVR_EventType_DeviceConnected:
        {
            LOGD("Device %u attached. Setting up controller cube.\n", event.device.deviceType);
        }
            break;
        case WVR_EventType_DeviceDisconnected:
        {
            LOGD("Device %u detached.\n", event.device.deviceType);
        }
            break;
        case WVR_EventType_DeviceStatusUpdate:
        {
            LOGD("Device %u updated.\n", event.device.deviceType);
        }
            break;
        case WVR_EventType_IpdChanged:
        {
            WVR_RenderProps_t props;
            bool ret = WVR_GetRenderProps(&props);
            float ipd = 0;
            if (ret) {
                ipd = props.ipdMeter;
            }
            LOGI("IPD is changed (%.4f) and renew the transform from eye to head.", ipd);
            projection_left_ = wvr::toGlm(
                    WVR_GetProjection(WVR_Eye_Left, near_clip_, far_clip_));
            projection_right_ = wvr::toGlm(
                    WVR_GetProjection(WVR_Eye_Right, near_clip_, far_clip_));

            eye_pos_left_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(
                    WVR_GetTransformFromEyeToHead(WVR_Eye_Left)));
            eye_pos_right_ = glm::inverse<4, 4, float, glm::defaultp>(wvr::toGlm(
                    WVR_GetTransformFromEyeToHead(WVR_Eye_Right)));

            dumpMatrix("ProjectionLeft", projection_left_);
            dumpMatrix("ProjectionRight", projection_right_);
            dumpMatrix("EyePosLeft", eye_pos_left_);
            dumpMatrix("EyePosRight", eye_pos_right_);
        }
            break;
    }
}

void WvrScene::AddObject(std::shared_ptr<lark::Object> object) {
    objects_.push_back(object);
}

void WvrScene::RemoveObject(std::shared_ptr<lark::Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void WvrScene::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}