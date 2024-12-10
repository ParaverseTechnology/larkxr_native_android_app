//
// Created by Administrator on 2021/7/6.
//

#include <common/xr_linear.h>
#include "pvr_xr_scene.h"
#include "pvr_xr_utils.h"

namespace {
    constexpr float DarkSlateGray[] = {0.184313729f, 0.309803933f, 0.309803933f, 1.0f};
}

PvrXRScene::PvrXRScene() {
}

void PvrXRScene::InitGL(GraphicsDeviceAndroid *device) {
    device_ = device;
}

void PvrXRScene::ReleaseGL() {
    device_ = nullptr;
}

void PvrXRScene::RenderView(lark::Object::Eye eye, const XrCompositionLayerProjectionView& layerView) {
    if (device_ == nullptr) {
        return;
    }

    const auto& pose = layerView.pose;
    XrMatrix4x4f proj = {};
    XrMatrix4x4f_CreateProjectionFov(&proj, GRAPHICS_OPENGL_ES, layerView.fov, 0.05f, 100.0f);
    XrMatrix4x4f toView = {};
    XrVector3f scale{1.f, 1.f, 1.f};
    XrMatrix4x4f_CreateTranslationRotationScale(&toView, &pose.position, &pose.orientation, &scale);
    XrMatrix4x4f view = {};
    XrMatrix4x4f_InvertRigidBody(&view, &toView);

    glm::mat4 g_proj = pvr::toGlm(proj);
    glm::mat4 g_view = pvr::toGlm(view);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(eye, g_proj, g_view);
        }
    }
}

void PvrXRScene::AddObject(std::shared_ptr<lark::Object> object) {
    objects_.push_back(object);
}

void PvrXRScene::RemoveObject(std::shared_ptr<lark::Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void PvrXRScene::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}

void PvrXRScene::HandleInput(const InputState &input_state, XrSession const &session,
                             XrSpace const &space) {
    for(auto & object : objects_) {
        if (object->active()) {
            // TODO eye config
            object->Update();
        }
    }
}

