//
// Created by fcx@pingxingyun.com on 2021/7/15.
//

#include <svrUtil.h>
#include "xms_scene.h"
#include "log.h"
#include "svrUtil.h"
#include <common/xr_linear.h>

namespace {
    inline glm::mat4 toGlm(const XrMatrix4x4f& m) {
        return glm::mat4(
                m.m[0], m.m[1], m.m[2], m.m[3],
                m.m[4], m.m[5], m.m[6], m.m[7],
                m.m[8], m.m[9], m.m[10], m.m[11],
                m.m[12], m.m[13], m.m[14], m.m[15]
        );
    }
}

XmsScene::XmsScene() = default;

XmsScene::~XmsScene() = default;

void XmsScene::InitGl(Svr::SvrApplicationContext *context) {
    context_ = context;
    fov_0_ = context->targetEyeFovXDeg;
//    fov_0_ = 101;
    fov_1_ = context->targetEyeFovXDeg;
//    fov_1_ = 101;
    eye_buffer_width_ = context->targetEyeWidth;
    eye_buffer_height_ = context->targetEyeHeight;

    float halfW = near_z_ * tanf( fov_0_ * 0.5F * 3.14159260F /180.0F );
    float halfH = near_z_ * tanf( fov_1_ * 0.5F * 3.14159260F /180.0F );
    LOGI("FOV is %f , halfW is %f", fov_0_, halfW);
    projection_[0] = glm::frustum(-halfW,halfW,-halfH,halfH, near_z_, far_z_);
    projection_[1] = glm::frustum(-halfW,halfW,-halfH,halfH, near_z_, far_z_);

//    projection_[0] = glm::perspective(fov_0_, (float)eye_buffer_width_ / (float)eye_buffer_height_, near_z_, far_z_);
//    projection_[1] = glm::perspective(fov_1_, (float)eye_buffer_width_ / (float)eye_buffer_height_, near_z_, far_z_);
}

void XmsScene::UpdateHMDPose(const svrHeadPoseState& headPoseState) {
    Svr::SvrGetEyeViewMatrices(headPoseState, true, 0.064, 0, 0, view_[0], view_[1]);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Update();
        }
    }
    pose_state_ = headPoseState;
}

void XmsScene::Draw(int eye) {
    svrBeginEye(eye == 0 ? svrWhichEye::kLeftEye : svrWhichEye::kRightEye);
    context_->eyeBuffers[context_->eyeBufferIndex].eyeTarget[eye].Bind();
    // 开启透明同道混合
    glEnable( GL_BLEND );
    //配置混合方程式，默认为 GL_FUNC_ADD 方程
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL( glEnable( GL_SCISSOR_TEST ) );
    GL( glDepthMask( GL_TRUE ) );
    GL( glEnable( GL_DEPTH_TEST ) );
    GL( glDepthFunc( GL_LEQUAL ) );
    GL( glEnable( GL_CULL_FACE ) );
    GL( glCullFace( GL_BACK ) );
    GL( glViewport( 0, 0, eye_buffer_width_, eye_buffer_height_ ) );
    GL( glScissor( 0, 0, eye_buffer_width_, eye_buffer_height_ ) );
    GL( glClearColor( 0.125F, 0.0F, 0.125F, 1.0F ) );
    GL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );
    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(eye == 0 ? lark::Object::EYE_LEFT : lark::Object::EYE_RIGHT,
                            projection_[eye], view_[eye]);
        }
    }
    glDisable(GL_BLEND);
    context_->eyeBuffers[context_->eyeBufferIndex].eyeTarget[eye].Unbind();

    render_texture_[eye] = context_->eyeBuffers[context_->eyeBufferIndex].eyeTarget[eye].GetColorAttachment();
    svrEndEye(eye == 0 ? svrWhichEye::kLeftEye : svrWhichEye::kRightEye);
}

void XmsScene::AddObject(std::shared_ptr <lark::Object> object) {
    objects_.push_back(object);
}

void XmsScene::RemoveObject(std::shared_ptr <lark::Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void XmsScene::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}

void XmsScene::Submit() {
    svrFrameParams frameParams;
    memset(&frameParams, 0, sizeof(frameParams));
    frameParams.frameIndex = context_->frameCount;

    frameParams.renderLayers[0].imageHandle = render_texture_[0];
    frameParams.renderLayers[0].imageType = kTypeTexture;
    Svr::L_CreateLayout(0.0f, 0.0f, 1.0f, 1.0f, &frameParams.renderLayers[0].imageCoords);

    frameParams.renderLayers[0].eyeMask = kEyeMaskLeft;
    frameParams.renderLayers[0].layerFlags |= kLayerFlagHeadLocked;

    frameParams.renderLayers[1].imageHandle = render_texture_[1];
    frameParams.renderLayers[1].imageType = kTypeTexture;
    Svr::L_CreateLayout(0.0f, 0.0f, 1.0f, 1.0f, &frameParams.renderLayers[1].imageCoords);

    frameParams.renderLayers[1].eyeMask = kEyeMaskRight;
    frameParams.renderLayers[1].layerFlags |= kLayerFlagHeadLocked;

    context_->eyeBufferIndex = (context_->eyeBufferIndex + 1) % SVR_NUM_EYE_BUFFERS;

    frameParams.headPoseState = pose_state_;
    frameParams.minVsyncs = 1;
//    frameParams.fieldOfView = 105.0f;
    svrSubmitFrame(&frameParams);

    context_->frameCount++;
}
