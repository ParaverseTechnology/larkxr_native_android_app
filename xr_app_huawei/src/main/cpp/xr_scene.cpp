//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include "xr_scene.h"
#include "matrix_functions.h"
#include "hxr_utils.h"

namespace hxr {
XrScene::XrScene() {
}

XrScene::~XrScene() {
}

void XrScene::InitGL() {
}

void XrScene::ReleaseGL() {
}

void XrScene::Render(lark::Object::Eye eye, const XrCompositionLayerProjectionView &layerView) {

    XrQuaternionf xrquat;
    XrVector3f xrvec;
    xrquat.x = layerView.pose.orientation.x;
    xrquat.y = layerView.pose.orientation.y;
    xrquat.z = layerView.pose.orientation.z;
    xrquat.w = layerView.pose.orientation.w;

    xrvec.x = layerView.pose.position.x;
    xrvec.y = layerView.pose.position.y;
    xrvec.z = layerView.pose.position.z;


    XrMatrix4x4f eyeViewMatrix = XrMatrix4x4f_CreateFromQuaternion_4(&xrquat, &xrvec);

    const float tanAngleLeft = tanf(layerView.fov.angleLeft);
    const float tanAngleRight = tanf(layerView.fov.angleRight);
    const float tanAngleUp = tanf(layerView.fov.angleUp);
    float const tanAngleDown = tanf(layerView.fov.angleDown);

    XrMatrix4x4f projectionMatrix = XrMatrix4x4f_CreateProjection(tanAngleLeft, tanAngleRight, tanAngleUp, tanAngleDown, 0.1f, 200.0f);

    glm::mat4 g_proj = toGlm(projectionMatrix);
    glm::mat4 g_view = toGlm(eyeViewMatrix);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(eye, g_proj, g_view);
        }
    }
}

void XrScene::AddObject(std::shared_ptr<lark::Object> object) {
    objects_.push_back(object);
}

void XrScene::RemoveObject(std::shared_ptr<lark::Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void XrScene::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}

void XrScene::HandleInput(const InputState &input_state) {
    for(auto & object : objects_) {
        if (object->active()) {
            object->Update();
        }
    }
}
}