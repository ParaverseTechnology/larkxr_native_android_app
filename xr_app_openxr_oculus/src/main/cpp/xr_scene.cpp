//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <openxr/openxr_oculus_helpers.h>
#include "xr_scene.h"

namespace oxr {
XrScene::XrScene() {
}

XrScene::~XrScene() {
}

void XrScene::InitGL() {
}

void XrScene::ReleaseGL() {
}

void XrScene::Render(lark::Object::Eye eye, const XrCompositionLayerProjectionView &layerView,
                     FrameBuffer &frameBuffer) {
    frameBuffer.Acquire();

    frameBuffer.SetCurrent();

    // 开启透明同道混合
    GL( glEnable(GL_BLEND) );
    GL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
//
    GL( glEnable(GL_DEPTH_TEST) );
    GL( glDepthFunc(GL_LEQUAL) );
    GL( glDepthMask(true) );

    GL(glEnable(GL_SCISSOR_TEST));
    GL(glDepthMask(GL_TRUE));
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LEQUAL));
    GL(glDisable(GL_CULL_FACE));
    GL( glCullFace( GL_BACK ) );
    GL(glViewport(0, 0, frameBuffer.width(), frameBuffer.height()));
    GL(glScissor(0, 0, frameBuffer.width(), frameBuffer.height()));
    GL(glClearColor(0, 0, 0, 1.0f));
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    XrMatrix4x4f proj;
    XrMatrix4x4f_CreateProjectionFov(&proj, GRAPHICS_OPENGL_ES, layerView.fov, 0.05f, 100.0f);

    XrPosef pose = XrPosef_Inverse(layerView.pose);
    XrMatrix4x4f view = XrMatrix4x4f_CreateFromRigidTransform(&pose);

    glm::mat4 g_proj = toGlm(proj);
    glm::mat4 g_view = toGlm(view);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            it->get()->Draw(eye, g_proj, g_view);
        }
    }

    frameBuffer.Resolve();

    frameBuffer.Release();

    frameBuffer.SetNone();
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