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
//    test_obj_ = std::make_shared<TestObj>();
//    test_obj_->Move(0, 0, -5);

//    sky_box_ = std::make_shared<lark::SkyBox>("textures/skybox_8_2k.jpg");
}

void PvrXRScene::ReleaseGL() {
    device_ = nullptr;
//    test_obj_.reset();
//    sky_box_.reset();
}

void PvrXRScene::RenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
                            int64_t swapchainFormat) {
//    CHECK(layerView.subImage.imageArrayIndex == 0);  // Texture arrays not supported.
    UNUSED_PARM(swapchainFormat);                    // Not used in this function for now.
    if (device_ == nullptr) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, device_->swapchain_framebuffer());

    const uint32_t colorTexture = reinterpret_cast<const XrSwapchainImageOpenGLESKHR*>(swapchainImage)->image;

    glViewport(static_cast<GLint>(layerView.subImage.imageRect.offset.x),
               static_cast<GLint>(layerView.subImage.imageRect.offset.y),
               static_cast<GLsizei>(layerView.subImage.imageRect.extent.width),
               static_cast<GLsizei>(layerView.subImage.imageRect.extent.height));

//        glFrontFace(GL_CW);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    // 开启透明同道混合
    glEnable( GL_BLEND );
    //配置混合方程式，默认为 GL_FUNC_ADD 方程
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const uint32_t depthTexture = device_->GetDepthTexture(colorTexture);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // Clear swapchain and depth buffer.
    glClearColor(DarkSlateGray[0], DarkSlateGray[1], DarkSlateGray[2], DarkSlateGray[3]);
    glClearDepthf(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    const auto& pose = layerView.pose;
    XrMatrix4x4f proj;
    XrMatrix4x4f_CreateProjectionFov(&proj, GRAPHICS_OPENGL_ES, layerView.fov, 0.05f, 100.0f);
    XrMatrix4x4f toView;
    XrVector3f scale{1.f, 1.f, 1.f};
    XrMatrix4x4f_CreateTranslationRotationScale(&toView, &pose.position, &pose.orientation, &scale);
    XrMatrix4x4f view;
    XrMatrix4x4f_InvertRigidBody(&view, &toView);

    glm::mat4 g_proj = pvr::toGlm(proj);
    glm::mat4 g_view = pvr::toGlm(view);

    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            // TODO eye config
            it->get()->Draw(lark::Object::EYE_LEFT, g_proj, g_view);
        }
    }

//    const float tanLeft = tanf(layerView.fov.angleLeft);
//    const float tanRight = tanf(layerView.fov.angleRight);
//
//    const float tanDown = tanf(layerView.fov.angleDown);
//    const float tanUp = tanf(layerView.fov.angleUp);

//        glm::mat4 g_proj = glm::frustum(-tanLeft, tanRight, -tanDown , tanUp, 0.1f, 100.0f);
//        LOGV("fov %f %f %f %f; %f %f %f %f",
//                layerView.fov.angleLeft, layerView.fov.angleRight, layerView.fov.angleDown, layerView.fov.angleUp,
//                tanLeft, tanRight, tanDown , tanUp);
//    glm::vec3 g_positon(pose.position.x, pose.position.y, pose.position.z);
//    glm::quat g_rotation;
//    g_rotation.x = pose.orientation.x;
//    g_rotation.y = pose.orientation.y;
//    g_rotation.z = pose.orientation.z;
//    g_rotation.w = pose.orientation.w;
//    glm::mat4 g_view = glm::mat4_cast(g_rotation);
//    g_view = glm::inverse(g_view);

//    sky_box_->Draw(lark::Object::EYE_LEFT, g_proj, g_view);
//    test_obj_->Draw(lark::Object::EYE_LEFT, g_proj, g_view);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    device_->Swap();
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
    for(auto it = objects_.begin(); it != objects_.end(); it ++) {
        if (it->get()->active()) {
            // TODO eye config
            it->get()->Update();
        }
    }
}

