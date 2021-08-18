//
// Created by fcx@pingxingyun.com on 2019/10/25.
//

#include <memory>
// ovr
#include <VrApi_Input.h>
#include <VrApi_SystemUtils.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>
#include <VrApi_Types.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ovr_scene.h"
#include "log.h"
#include "egl_utils.h"
#include "utils.h"
#include "ovr_utils.h"

#define LOG_TAG "ovr_scene"

static const int NUM_MULTI_SAMPLES	= 4;
using namespace lark;

OvrScene::OvrScene():
    frame_index_(0),
    display_time_(0),
    light_dir_(-0.8F, 0.45, 0.4, 0.40),
//    inited_(false),
//    use_multiview_(false),
//    java_(nullptr),
    num_buffers_(2),
    // SwapInterval = 1
    // ExtraLatencyMode = off
    // Expected single-threaded simulation latency = 33 milliseconds
    swap_interval_(1),
    objects_()
{
};

OvrScene::~OvrScene() {
//    if (inited_) {
//        DestoryFrameBuffer();
//    }
    ClearObject();
};

bool OvrScene::InitGL(OvrFrameBuffer *frame_buffer, int num_buffers) {
    frame_buffer_ = frame_buffer;
    num_buffers_ = num_buffers;
    return true;
}

//bool OvrScene::InitJava(OvrFrameBuffer *frame_buffer, int num_buffers) {
////    java_ = java;
////    use_multiview_ = useMultiView;
////    CreateFrameBuffer(java, useMultiView);
////    inited_ = java_ != nullptr;
////    inited_ = true;
//    frame_buffer_ = frame_buffer;
//    num_buffers_ = num_buffers;
//    return false;
//}

bool OvrScene::ShutdownGL() {
//    inited_ = false;
    return true;
}

bool OvrScene::Update(ovrMobile * ovr) {
//    LOGENTRY();
    // Update predicted first.
    frame_index_++;
    const double predictedDisplayTime = vrapi_GetPredictedDisplayTime(ovr, frame_index_);
    display_time_ = predictedDisplayTime;
    HandleInput(ovr);
    // call objecs Update.
    for(auto & object : objects_) {
        if (object->active())
            object->Update();
    }
    Render(ovr);
    return true;
}

bool OvrScene::HandleInput(ovrMobile * ovr) {
//    if (!inited_) {
//        // not inited_.
//        return false;
//    }
    return true;
}

bool OvrScene::Render(ovrMobile * ovr) {
//    if (!inited_) {
//        // not inited_.
//        return false;
//    }
//    LOGENTRY();
    const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovr, display_time_);
//    onTestRenderLoading(ovr);
    const ovrLayerProjection2 worldLayer = RenderFrame(&tracking, ovr);
    const ovrLayerHeader2 * layers[] =
            {
                    &worldLayer.Header
            };

    ovrSubmitFrameDescription2 frameDesc = { 0 };
    frameDesc.Flags = 0;
    frameDesc.SwapInterval = swap_interval_;
    frameDesc.FrameIndex = frame_index_;
    frameDesc.DisplayTime = display_time_;
    frameDesc.LayerCount = 1;
    frameDesc.Layers = layers;

    // Hand over the eye images to the time warp.
    vrapi_SubmitFrame2( ovr, &frameDesc );
    return true;
}

void OvrScene::UpdateHMDMatrixPose() {
}

void OvrScene::UpdateEyeToHeadMatrix(bool is6DoF) {
}

void OvrScene::SetupCameras() {
}

void OvrScene::UpdateTime() {
}

void OvrScene::UpdateControllerDegreeOfFreedom() {
}

void OvrScene::OnTestRenderLoading(ovrMobile * ovrMobile1) {
    // Show a loading icon.
    uint32_t frameFlags = 0;
    frameFlags |= VRAPI_FRAME_FLAG_FLUSH;

    ovrLayerProjection2 blackLayer = vrapi_DefaultLayerBlackProjection2();
    blackLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_INHIBIT_SRGB_FRAMEBUFFER;

    ovrLayerLoadingIcon2 iconLayer = vrapi_DefaultLayerLoadingIcon2();
    iconLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_INHIBIT_SRGB_FRAMEBUFFER;

    ovrLayerCube2 cubeLayer = vrapi_DefaultLayerCube2();
//    cubeLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_INHIBIT_SRGB_FRAMEBUFFER;
    ovrVector4f color = { 0.2F, 0.3F, 0.4F, 1};
    ovrLayerProjection2 colorLayer = vrapi_DefaultLayerSolidColorProjection2(&color);

    ovrLayerCylinder2 culinderLayer = vrapi_DefaultLayerCylinder2();
    const ovrLayerHeader2 * layers[] =
            {
                    &colorLayer.Header,
                    &iconLayer.Header,
            };

    ovrSubmitFrameDescription2 frameDesc = { 0 };
    frameDesc.Flags = frameFlags;
    frameDesc.SwapInterval = 1;
    frameDesc.FrameIndex = frame_index_;
    frameDesc.DisplayTime = display_time_;
    frameDesc.LayerCount = 2;
    frameDesc.Layers = layers;

    vrapi_SubmitFrame2( ovrMobile1, &frameDesc );
}

void OvrScene::OnTestRender(ovrMobile *ovrMobile1) {
    // Render eye images and setup the primary layer using ovrTracking2.
    const ovrLayerProjection2 worldLayer = {};

    const ovrLayerHeader2 * layers[] =
            {
                    &worldLayer.Header
            };

    ovrSubmitFrameDescription2 frameDesc = { 0 };
    frameDesc.Flags = 0;
//    frameDesc.SwapInterval = appState.SwapInterval;
    frameDesc.FrameIndex = frame_index_;
    frameDesc.DisplayTime = display_time_;
    frameDesc.LayerCount = 1;
    frameDesc.Layers = layers;

    // Hand over the eye images to the time warp.
    vrapi_SubmitFrame2( ovrMobile1, &frameDesc );
}

void OvrScene::ClearFrameBuffer() {
//    for(auto & frameBuffer : frame_buffer_)
//    {
//        frameBuffer.clear();
//    }
//    num_buffers_ = VRAPI_FRAME_LAYER_EYE_MAX;
}

void OvrScene::CreateFrameBuffer(const ovrJava *java, const bool useMultiview) {
    num_buffers_ = useMultiview ? 1 : VRAPI_FRAME_LAYER_EYE_MAX;

    // Create the frame buffers.
    for ( int eye = 0; eye < num_buffers_; eye++ )
    {
        frame_buffer_[eye].create( useMultiview,
                                  GL_RGBA8,
//                                  vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH ),
                                  vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_DISPLAY_PIXELS_WIDE ) / 2,
//                                  vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT ),
                                  vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_DISPLAY_PIXELS_WIDE ),
                                  NUM_MULTI_SAMPLES);
    }
}

void OvrScene::DestoryFrameBuffer() {
    for ( int eye = 0; eye < num_buffers_; eye++ )
    {
        frame_buffer_[eye].destory();
    }
    ClearFrameBuffer();
}

ovrLayerProjection2 OvrScene::RenderFrame(const ovrTracking2 *tracking, ovrMobile *ovr) {
    ovrLayerProjection2 layer = vrapi_DefaultLayerProjection2();
    layer.HeadPose = tracking->HeadPose;
    for ( int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; eye++ )
    {
        OvrFrameBuffer * frameBuffer = &frame_buffer_[num_buffers_ == 1 ? 0 : eye];
        layer.Textures[eye].ColorSwapChain = frameBuffer->getOvrTextureSwapChain();
        layer.Textures[eye].SwapChainIndex = frameBuffer->getTextureSwapChainIndex();
        layer.Textures[eye].TexCoordsFromTanAngles = ovrMatrix4f_TanAngleMatrixFromProjection( &tracking->Eye[eye].ProjectionMatrix );
    }
    layer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;

    // Render the eye images.
    for ( int eye = 0; eye < num_buffers_; eye++ )
    {
        // NOTE: In the non-mv case, latency can be further reduced by updating the sensor prediction
        // for each eye (updates orientation, not position)
        OvrFrameBuffer * frameBuffer = &frame_buffer_[eye];
        frameBuffer->setCurrent();

        GL( glEnable( GL_SCISSOR_TEST ) );
        GL( glDepthMask( GL_TRUE ) );
        GL( glEnable( GL_DEPTH_TEST ) );
        GL( glDepthFunc( GL_LEQUAL ) );
        GL( glEnable( GL_CULL_FACE ) );
        GL( glCullFace( GL_BACK ) );
        GL( glViewport( 0, 0, frameBuffer->getWidth(), frameBuffer->getHeight() ) );
        GL( glScissor( 0, 0, frameBuffer->getWidth(), frameBuffer->getHeight() ) );
        GL( glClearColor( 0.125F, 0.0F, 0.125F, 1.0F ) );
        GL( glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) );

        Object::Eye objEye = eye == VRAPI_EYE_LEFT ? Object::EYE_LEFT : Object::EYE_RIGHT;
        glm::mat4 project = ovr::toGlm(tracking->Eye[eye].ProjectionMatrix);;
        glm::mat4 eyeView = ovr::toGlm(tracking->Eye[eye].ViewMatrix);

        for(auto & object : objects_) {
            if (object->active())
                object->Draw(objEye, project, eyeView);
        }

        GL( glBindVertexArray( 0 ) );
        GL( glUseProgram( 0 ) );

        // Explicitly clear the border texels to black when GL_CLAMP_TO_BORDER is not available.
        if (!egl_utils::glExtensions.EXT_texture_border_clamp)
        {
            // Clear to fully opaque black.
            GL( glClearColor( 0.0F, 0.0F, 0.0F, 1.0F ) );
            // bottom
            GL( glScissor( 0, 0, frameBuffer->getHeight(), 1 ) );
            GL( glClear( GL_COLOR_BUFFER_BIT ) );
            // top
            GL( glScissor( 0, frameBuffer->getHeight() - 1, frameBuffer->getWidth(), 1 ) );
            GL( glClear( GL_COLOR_BUFFER_BIT ) );
            // left
            GL( glScissor( 0, 0, 1, frameBuffer->getHeight() ) );
            GL( glClear( GL_COLOR_BUFFER_BIT ) );
            // right
            GL( glScissor( frameBuffer->getWidth() - 1, 0, 1, frameBuffer->getHeight() ) );
            GL( glClear( GL_COLOR_BUFFER_BIT ) );
        }

        frameBuffer->resolve();
        frameBuffer->advance();
    }
    GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );

    return layer;
}

void OvrScene::AddObject(std::shared_ptr<Object> object) {
//    objects_.insert(std::pair<std::shared_ptr<Object>, std::shared_ptr<Object>>(object, object));
    objects_.push_back(object);
}

void OvrScene::RemoveObject(std::shared_ptr<Object> object) {
    // TODO objects manager
    for (auto it = objects_.begin(); it != objects_.end(); it++) {
        if (object->id() == it->get()->id()) {
            objects_.erase(it);
        }
    }
}

void OvrScene::ClearObject() {
    if (objects_.empty()) {
        return;
    }
    objects_.clear();
}