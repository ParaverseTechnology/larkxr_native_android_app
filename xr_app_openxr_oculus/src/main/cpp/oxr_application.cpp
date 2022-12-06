//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <env_context.h>
#include <asset_files.h>
#include <lark_xr/xr_latency_collector.h>
#include "oxr_application.h"

#define LOG_TAG "oxr_application"

#define USE_RENDER_QUEUE 1

namespace {
    const float CLOUD_LOCALSPACE_HEIGHT_OFFSET = 1.5f;
}

namespace oxr {
OxrApplication::OxrApplication() {
    RegiseredInstance(this);
}

OxrApplication::~OxrApplication() {
    UnRegiseredInstance();
}

bool OxrApplication::InitGL(OpenxrContext *context) {
    context_ = context;

    // 初始化客户端接入凭证
    InitCertificate();

    {
        // configs
        LOGV("system[%s] is quest %d ", context->system_properties().systemName, OpenxrContext::SYSTEM_NAME_QUEST.compare(context->system_properties().systemName));

        lark::XRConfig::render_width = context_->frame_buffer(0).width() * 2;
        lark::XRConfig::render_height = context_->frame_buffer(0).height();

        lark::XRConfig::headset_desc.type = larkHeadSetType_OCULUS;
#ifdef USE_RENDER_QUEUE
        lark::XRConfig::use_render_queue = true;
#else
        lark::XRConfig::use_render_queue = false;
#endif
        lark::XRConfig::fps = context_->GetCurrentDisplayRefreshRate();
        lark::XRConfig::request_pose_fps = lark::XRConfig::fps * 2;
        lark::XRConfig::use_multiview = true;
    }

    xr_client_.reset();
    xr_client_ = std::make_shared<lark::XRClient>();

    // gl context not ready. init share context later.
    // init_share_context should be false
    xr_client_->Init(Context::instance()->vm(), false);

    xr_client_->InitGLShareContext();
    xr_client_->RegisterObserver(this);
    xr_client_->EnableDebugMode(true);

    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }

    // load assets.
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext asset_context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&asset_context, Assetlist);

    scene_local_ = std::make_shared<XrSceneLocal>();
    scene_local_->InitGL();

    scene_cloud_ = std::make_shared<XrSceneCloud>();

#ifdef ENABLE_CLOUDXR
    // create cloudxr before scene cloud inited
    cloudxr_client_ = std::make_shared<CloudXRClient>(this);
    // create cloudxr before scene cloud inited
    scene_cloud_->SetCloudXRClient(cloudxr_client_);
#endif

    scene_cloud_->InitGL();

    return true;
}

void OxrApplication::ShutdownGL() {
    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
    }
#endif

    // release cloudlark
    xr_client_->UnRegisterObserver();
    xr_client_->ReleaseGLShareContext();
    xr_client_->Release();
    xr_client_.reset();

    scene_local_->ReleaseGL();

    scene_cloud_->ReleaseGL();

    // reset all state.
    Input::ResetInput();
    lark::AssetLoader::Release();

    Application::ShutdownGL();
}

void OxrApplication::OnResume() {
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void OxrApplication::OnPause() {
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

void OxrApplication::Update() {
    // TODO handle sapce change when remote media ready.
#ifdef ENABLE_CLOUDXR
    bool cloud_space = xr_client_->media_ready() || cloudxr_client_->IsConnect();
#else
    bool cloud_space = xr_client_->media_ready();
#endif

    context_->input_state().UpDate(context_->session(),
                                   cloud_space ? GetSelectedXRSpace() : context_->local_space(),
                                   utils::GetTimestampNs());

    if (xr_client_->is_connected()) {
        scene_cloud_->HandleInput(context_->input_state());
    } else {
        scene_local_->HandleInput(context_->input_state());
    }
}

void OxrApplication::RenderFrame() {
    larkxrTrackingFrame trackingFrame = {};
    bool has_new_frame_pxy_stream = false;
    bool has_new_frame_cloudxr = false;

#ifdef ENABLE_CLOUDXR
    if (need_recreat_cloudxr_client_) {
        cloudxr_client_->Init();
        need_recreat_cloudxr_client_ = false;
    }

    if (need_reconnect_public_ip_ && cloudxr_client_) {
        // recreate recevier
        cloudxr_client_->Init();
        // try to connect to public ip.
        cloudxr_client_->Connect(prepare_public_ip_);

        need_reconnect_public_ip_ = false;
        prepare_public_ip_ = "";
    }
    // cloudxr progess
    if (cloudxr_client_ && cloudxr_client_->IsConnect()) {
        cxrFramesLatched latched;
//        LOGV("CLOUDXR start latched");
        cxrError error = cloudxr_client_->Latch(latched);
        if (error != cxrError_Success)
        {
            LOGV("Latching frame failed.");
            if (error == cxrError_Frame_Not_Ready)
            {
                LOGW("LatchFrame failed, frame not ready for %d ms", 150);
            }
            else
            {
                LOGE("Error in LatchFrame [%0d] = %s", error, cxrErrorString(error));
            }
            return;
        }

        {
            uint64_t frameIndex = latched.poseID;
            std::lock_guard<std::mutex> lock(tracking_frame_mutex_);
            auto it = tracking_frame_map_.find(frameIndex);
            if (it != tracking_frame_map_.end()) {
                trackingFrame = it->second;
            } else {
                if (!tracking_frame_map_.empty()) {
                    LOGW("cant find new tracking frame in map. use old. size %ld; index %ld", tracking_frame_map_.size(), frameIndex);
                    trackingFrame = tracking_frame_map_.cbegin()->second;
                } else {
                    LOGW("cant find tracking frame in map. size %ld; index %ld", tracking_frame_map_.size(), frameIndex);
                    return;
                }
            }
        }
//        LOGV("CLOUDXR frame ready %ld %ld", trackingFrame.frameIndex, latched.poseID);
        has_new_frame_cloudxr = true;
    }
#endif

#ifdef USE_RENDER_QUEUE
    bool cloudmedia_ready = xr_client_->media_ready();
    lark::XRVideoFrame xrVideoFrame(0);

    if (!has_new_frame_cloudxr && xr_client_->is_connected()) {
        has_new_frame_pxy_stream = xr_client_->Render(&trackingFrame, &xrVideoFrame);
        // wait for cloud frame
        if (cloudmedia_ready && !has_new_frame_pxy_stream) {
//            LOGV("wait for new frame");
            usleep(1000);
            return;
        }
        if (has_new_frame_pxy_stream) {
            lark::XRLatencyCollector::Instance().Rendered2(trackingFrame.frameIndex);
            scene_cloud_->UpdateTexture(xrVideoFrame);
        }
    }
#else
    bool cloudmedia_ready = xr_client_->media_ready();
    xr_client_->Render(&trackingFrame);
#endif

    // NOTE: OpenXR does not use the concept of frame indices. Instead,
    // XrWaitFrame returns the predicted display time.
    XrFrameWaitInfo waitFrameInfo = {};
    waitFrameInfo.type = XR_TYPE_FRAME_WAIT_INFO;
    waitFrameInfo.next = NULL;

    XrFrameState frameState = {};
    frameState.type = XR_TYPE_FRAME_STATE;
    frameState.next = NULL;

    OXR(xrWaitFrame(context_->session(), &waitFrameInfo, &frameState));

    // Get the HMD pose, predicted for the middle of the time period during which
    // the new eye images will be displayed. The number of frames predicted ahead
    // depends on the pipeline depth of the engine and the synthesis rate.
    // The better the prediction, the less black will be pulled in at the edges.
    XrFrameBeginInfo beginFrameDesc = {};
    beginFrameDesc.type = XR_TYPE_FRAME_BEGIN_INFO;
    beginFrameDesc.next = NULL;
    OXR(xrBeginFrame(context_->session(), &beginFrameDesc));

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;

    if (frameState.shouldRender == XR_TRUE) {
//        LOGV("RENDER %d %d %d", has_new_frame_pxy_stream, has_new_frame_cloudxr, has_new_frame_pxy_stream || has_new_frame_cloudxr);
        if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer,
                        trackingFrame,
                        has_new_frame_pxy_stream || has_new_frame_cloudxr)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo endFrameInfo = {};
    endFrameInfo.type = XR_TYPE_FRAME_END_INFO;
    endFrameInfo.displayTime = frameState.predictedDisplayTime;
    endFrameInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
//    endFrameInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND;
//    endFrameInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_ADDITIVE;
    endFrameInfo.layerCount = layers.size();
    endFrameInfo.layers = layers.data();
    xrEndFrame(context_->session(), &endFrameInfo);

#ifdef USE_RENDER_QUEUE
    if (has_new_frame_pxy_stream) {
        XrSpaceLocation loc = {};
        loc.type = XR_TYPE_SPACE_LOCATION;

        // get head pose
        OXR(xrLocateSpace(
                context_->head_space(), GetSelectedXRSpace(), frameState.predictedDisplayTime, &loc));

        glm::vec3 trackingAng = glm::eulerAngles(toGlm(loc.pose.orientation));
        glm::vec3 renderAng = glm::eulerAngles(trackingFrame.tracking.rotation);
        float degree = glm::degrees(renderAng.y - trackingAng.y);

        lark::XRLatencyCollector::Instance().Submit(trackingFrame.frameIndex, degree);
        xr_client_->ReleaseRenderTexture();
    }
#endif

#ifdef ENABLE_CLOUDXR
    if (has_new_frame_cloudxr) {
        cloudxr_client_->Release();
        cloudxr_client_->Stats();
    }
#endif
}

bool OxrApplication::RenderLayer(XrTime predictedDisplayTime,
                                 std::vector<XrCompositionLayerProjectionView> &projectionLayerViews,
                                 XrCompositionLayerProjection &layer,
                                 const larkxrTrackingFrame& trackingFrame, bool hasNewFrame) {

    XrSpace space = hasNewFrame ? GetSelectedXRSpace() : context_->local_space();
    XrPosef xfStageFromHead = {};
    XrPosef viewTransform[oxr::OpenxrContext::ovrMaxNumEyes];

    if (!GetViewTransform(space, predictedDisplayTime, viewTransform, oxr::OpenxrContext::ovrMaxNumEyes, &xfStageFromHead)) {
        return false;
    }

    for (int eye = 0; eye < oxr::OpenxrContext::ovrMaxNumEyes; eye++) {
        viewTransform[eye] = XrPosef_Inverse(viewTransform[eye]);
    }

    // TODO config eyes
    // only support 2 eyes for now.
    projectionLayerViews.resize(oxr::OpenxrContext::ovrMaxNumEyes);

    if (!config_inited_) {
        glm::vec3 position[2] = {};
        for (int eye = 0; eye < oxr::OpenxrContext::ovrMaxNumEyes; eye++) {
            position[eye] = toGlm(context_->views()[eye].pose.position);

            LOGI("fov[eye] left %f right %f top %f bottom %f",
                 context_->views()[eye].fov.angleLeft,
                 context_->views()[eye].fov.angleRight,
                 context_->views()[eye].fov.angleUp,
                 context_->views()[eye].fov.angleDown);
            lark::XRConfig::fov[eye].left = glm::abs(glm::degrees(context_->views()[eye].fov.angleLeft));
            lark::XRConfig::fov[eye].right = glm::abs(glm::degrees(context_->views()[eye].fov.angleRight));

            // lark::XRConfig::fov[eye].top = glm::abs(glm::degrees(context_->views()[eye].fov.angleUp));
            // lark::XRConfig::fov[eye].bottom = glm::abs(glm::degrees(context_->views()[eye].fov.angleDown));

            // TODO setup angle down and up ?
            // TODO top -> angleDown ? bottom -> angle up ?
            lark::XRConfig::fov[eye].top = glm::abs(glm::degrees(context_->views()[eye].fov.angleDown));
            lark::XRConfig::fov[eye].bottom = glm::abs(glm::degrees(context_->views()[eye].fov.angleUp));

            LOGI("fov[eye] radius l %f r %f t %f b %f; degrees l %f r %f t %f b %f",
                 context_->views()[eye].fov.angleLeft,
                 context_->views()[eye].fov.angleRight,
                 context_->views()[eye].fov.angleUp,
                 context_->views()[eye].fov.angleDown,
                 lark::XRConfig::fov[eye].left,
                 lark::XRConfig::fov[eye].right,
                 lark::XRConfig::fov[eye].top,
                 lark::XRConfig::fov[eye].bottom);

            LOGI("width %d height %d", context_->frame_buffer(eye).color_swapchain().Width,
                 context_->frame_buffer(eye).color_swapchain().Height);
        }

        // TODO config render width
//        lark::XRConfig::render_width = context_->frame_buffer(0).color_swapchain().Width * 2;
//        lark::XRConfig::render_height = context_->frame_buffer(0).color_swapchain().Height;

        lark::XRConfig::ipd = glm::distance(position[0], position[1]);
        LOGI("ipd %f width %d height %d", lark::XRConfig::ipd, lark::XRConfig::render_width, lark::XRConfig::render_height);

#ifdef ENABLE_CLOUDXR
        // create cloudxr before scene cloud inited
        cloudxr_client_->InitRenderParamsWithLarkXRConfig();
        cloudxr_client_->Init();
#endif
        config_inited_ = true;
    }

    XrCompositionLayerProjection projection_layer = {};
    projection_layer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
    projection_layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
    projection_layer.layerFlags |= XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    if (hasNewFrame) {
        projection_layer.space = GetSelectedXRSpace();
    } else {
        projection_layer.space = context_->local_space();
    }
    projection_layer.viewCount = projectionLayerViews.size();
    projection_layer.views = projectionLayerViews.data();

    for (int eye = 0; eye < oxr::OpenxrContext::ovrMaxNumEyes; eye++) {
        oxr::FrameBuffer frameBuffer = context_->frame_buffer(eye);

        memset(
                &projectionLayerViews[eye], 0, sizeof(XrCompositionLayerProjectionView));
        projectionLayerViews[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;

        if (hasNewFrame) {
//            LOGV("CLOUDXR frame ready %ld", trackingFrame.frameIndex);
            projectionLayerViews[eye].pose.position = fromGlm(trackingFrame.tracking.eye[eye].viewPosition);
            projectionLayerViews[eye].pose.orientation = fromGlm(trackingFrame.tracking.eye[eye].viewRotation);
        } else {
            projectionLayerViews[eye].pose = XrPosef_Inverse(viewTransform[eye]);
        }

        projectionLayerViews[eye].fov = context_->views()[eye].fov;

        memset(&projectionLayerViews[eye].subImage, 0, sizeof(XrSwapchainSubImage));
        projectionLayerViews[eye].subImage.swapchain =
                frameBuffer.color_swapchain().Handle;
        projectionLayerViews[eye].subImage.imageRect.offset.x = 0;
        projectionLayerViews[eye].subImage.imageRect.offset.y = 0;
        projectionLayerViews[eye].subImage.imageRect.extent.width =
                frameBuffer.color_swapchain().Width;
        projectionLayerViews[eye].subImage.imageRect.extent.height =
                frameBuffer.color_swapchain().Height;
        projectionLayerViews[eye].subImage.imageArrayIndex = 0;

        if (xr_client_->is_connected()) {
            scene_cloud_->Render((lark::Object::Eye)eye, projectionLayerViews[eye], frameBuffer);
        } else {
            scene_local_->Render((lark::Object::Eye)eye, projectionLayerViews[eye], frameBuffer);
        }

    }

    layer = projection_layer;

    return true;
}

void OxrApplication::OnConnected() {
    Application::OnConnected();
    connected_ = true;
    scene_cloud_->OnConnect();
}

void OxrApplication::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude,
                                       float duration, float frequency) {
    Application::OnHapticsFeedback(isLeft, startTime, amplitude, duration, frequency);
}

void OxrApplication::OnError(int errCode, const std::string &msg) {
    Application::OnError(errCode, msg);
    LOGE("on xr client error %d; msg %s;", errCode, msg.c_str());

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
        need_recreat_cloudxr_client_ = true;
    }
#endif

    if (errCode == 1) {
        // enter applifailed.
        scene_local_->HomePage();
    } else {
        connected_ = false;
        scene_local_->HomePage();
    }
    Navigation::ShowToast(msg);

    scene_cloud_->OnClose();
}

void OxrApplication::OnClose(int code) {
    LOGV("=========on close %d", code);
    Application::OnClose(code);

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
        need_recreat_cloudxr_client_ = true;
    }
#endif

    connected_ = false;
    scene_local_->HomePage();
    switch(code) {
        case LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED:
            Navigation::ShowToast("与服务器媒体连接关闭");
            break;
        case LK_RENDER_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器 TCP 连接关闭");
            break;
        case LK_PROXY_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器代理连接关闭");
            break;
    }
    scene_cloud_->OnClose();
}

void OxrApplication::OnMediaReady(int nativeTextrure) {
    Application::OnMediaReady(nativeTextrure);
    scene_cloud_->OnMediaReady(nativeTextrure);
}

void OxrApplication::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    Application::OnMediaReady(nativeTextureLeft, nativeTextureRight);
    scene_cloud_->OnMediaReady(nativeTextureLeft, nativeTextureRight);
}

void OxrApplication::OnMediaReady() {
    Application::OnMediaReady();
    scene_cloud_->OnMediaReady();
}

void OxrApplication::RequestTrackingInfo() {
    Application::RequestTrackingInfo();

    larkxrTrackingDevicePairFrame devicePairFrame = {};

    if (UpdateCloudTrackingState(devicePairFrame)) {
        xr_client_->SendDevicePair(devicePairFrame);
    }
}

void OxrApplication::OnSyncPlayerSpace(larkxrPlaySpace *playSpace) {
    Application::OnSyncPlayerSpace(playSpace);
}

#ifdef ENABLE_CLOUDXR
void
OxrApplication::OnCloudXRReady(const std::string &appServerIp, const std::string &preferOutIp) {
    Application::OnCloudXRReady(appServerIp, preferOutIp);

    prepare_public_ip_ = preferOutIp;
    cxrError error = cloudxr_client_->Connect(appServerIp);
//    cxrError error = cloudxr_client_->Connect("222.128.6.137");
    if (error != cxrError_Success) {
        const char* errorString = cxrErrorString(error);
        LOGE("Error in LatchFrame [%0d] = %s", error, errorString);
        Navigation::ShowToast(errorString);
        need_recreat_cloudxr_client_ = true;
    }
}

void OxrApplication::UpdateClientState(cxrClientState state, cxrStateReason reason) {
    LOGI("PvrXrApplication UpdateClientState state %d reason %d", state, reason);
    switch (state) {
        case cxrClientState_ReadyToConnect:
            Navigation::ShowToast("创建CloudXR客户端成功");
            break;
        case cxrClientState_ConnectionAttemptInProgress:
            Navigation::ShowToast("开始连接服务器");
            break;
        case cxrClientState_StreamingSessionInProgress:
            Navigation::ShowToast("连接服务器成功");
            if (scene_cloud_) {
                scene_cloud_->OnCloudXRConnected();
            }
            break;
        case cxrClientState_ConnectionAttemptFailed:
        {
            if (!prepare_public_ip_.empty()) {
                need_reconnect_public_ip_ = true;
            } else {
                char buff[200];
                sprintf(buff, "连接CloudXR服务器失败 reason %d", reason);
                Navigation::ShowToast(buff);
                // release resource when cloudxr connected failed.
                xr_client_->Close();
            }
        }
            break;
        case cxrClientState_Disconnected:
        {
            char buff[200];
            sprintf(buff, "与CloudXR服务器连接断开 reason %d", reason);
            Navigation::ShowToast(buff);
            // release resource when cloudxr close.
            xr_client_->Close();
        }
            break;
        default:
            break;
    }
}

void OxrApplication::ReceiveUserData(const void *data, uint32_t size) {

}

void OxrApplication::GetTrackingState(cxrVRTrackingState *state) {
    if (state == nullptr) {
        LOGW("cloudxr null tracking state");
        return;
    }
//    LOGV("cloudxr get tracking state %ld", state->hmd.poseID);

    larkxrTrackingDevicePairFrame devicePairFrame = {};

    if (!UpdateCloudTrackingState(devicePairFrame)) {
        return;
    }

    {
        larkxrTrackingFrame frame;
        frame.frameIndex = devicePairFrame.frameIndex;
        frame.fetchTime = devicePairFrame.fetchTime;
        frame.displayTime = devicePairFrame.displayTime;
        frame.tracking = devicePairFrame.devicePair.hmdPose;
        // unique_ptr
        std::lock_guard<std::mutex> lock(tracking_frame_mutex_);
        tracking_frame_map_.insert(
                std::pair<uint64_t, larkxrTrackingFrame>(frame.frameIndex, frame));
        if (tracking_frame_map_.size() > MAXIMUM_TRACKING_FRAMES) {
            tracking_frame_map_.erase(tracking_frame_map_.cbegin());
        }
    }

    *state = CloudXRClient::VRTrackingStateFrom(devicePairFrame);

//    LOGV("push tracking info %ld %ld", state->hmd.poseID, devicePairFrame.frameIndex);
}
#endif

bool OxrApplication::GetViewTransform(const XrSpace& space,
                                      const XrTime& predictedDisplayTime,
                                      XrPosef *viewTransform,
                                      int viewTransformCount,
                                      XrPosef* xfStageFromHead) {
    // only support view count 2
    assert(viewTransformCount == oxr::OpenxrContext::ovrMaxNumEyes);

    XrSpaceLocation loc = {};
    loc.type = XR_TYPE_SPACE_LOCATION;

    // get head pose
    OXR(xrLocateSpace(
            context_->head_space(), space, predictedDisplayTime, &loc));
    *xfStageFromHead = loc.pose;

    XrViewLocateInfo projectionInfo = {};
    projectionInfo.type = XR_TYPE_VIEW_LOCATE_INFO;
    projectionInfo.viewConfigurationType = context_->viewport_config().viewConfigurationType;
    projectionInfo.displayTime = predictedDisplayTime;
    projectionInfo.space = context_->head_space();

    XrViewState viewState = {XR_TYPE_VIEW_STATE, nullptr};

    uint32_t projectionCapacityInput = oxr::OpenxrContext::ovrMaxNumEyes;
    uint32_t projectionCountOutput = projectionCapacityInput;

    OXR(xrLocateViews(
            context_->session(),
            &projectionInfo,
            &viewState,
            projectionCapacityInput,
            &projectionCountOutput,
            context_->views().data()));

    // TODO config eyes
    // only support 2 eyes for now.
    assert(projectionCountOutput == oxr::OpenxrContext::ovrMaxNumEyes);

    for (int eye = 0; eye < oxr::OpenxrContext::ovrMaxNumEyes; eye++) {
        XrPosef xfHeadFromEye = context_->views()[eye].pose;
        // head pose to left and right eye
        XrPosef xfStageFromEye = XrPosef_Multiply(*xfStageFromHead, xfHeadFromEye);
        viewTransform[eye] = xfStageFromEye;
    }


    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    return true;
}

void OxrApplication::SetupFPS(int fps) {
    Application::SetupFPS(fps);
    bool support = false;
    for(auto support_fps : context_->support_display_refresh_rates()) {
        LOGW("support fps[%f] request[%d] find[%d]", support_fps, fps, support_fps == fps);
        if (support_fps == fps) {
            support = support_fps == fps;
            break;
        }
    }
    if (!support) {
        LOGW("request fps[%d] not support", fps);
        return;
    }
    context_->SetDisplayRefreshRate(fps);
    lark::XRConfig::fps = fps;
}

void OxrApplication::SetupSapce(Space space) {
    Application::SetupSapce(space);
    current_cloud_space_ = space;
}

void OxrApplication::SetupSkyBox(int index) {
    Application::SetupSkyBox(index);
    // sync skybox
    // only support 0:textures/skybox_8_2k.jpg 1:textures/skybox_9.jpg
    scene_local_->SetSkyBox(index);
    scene_cloud_->SetSkyBox(index);
}
bool OxrApplication::UpdateCloudTrackingState(larkxrTrackingDevicePairFrame& trackingDevicePairFrame) {
    uint64_t now = utils::GetTimestampNs();
    XrTime predictedDisplayTime = now + 1000 * 1000 * 40;
    XrSpace space = GetSelectedXRSpace();
    XrPosef xfStageFromHead = {};
    XrPosef viewTransform[oxr::OpenxrContext::ovrMaxNumEyes];

    if (!GetViewTransform(space, predictedDisplayTime, viewTransform, oxr::OpenxrContext::ovrMaxNumEyes, &xfStageFromHead)) {
        return {};
    }

    // save hmd pose for pop up menu
    scene_cloud_->set_headpose(xfStageFromHead);

    if (current_cloud_space_ == Space_Local) {
        // TODO config height
        // add fixed height for local space
        xfStageFromHead.position.y += CLOUD_LOCALSPACE_HEIGHT_OFFSET;
    }

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;
    pose.rotation = toGlm(xfStageFromHead.orientation);
    pose.position = toGlm(xfStageFromHead.position);

    for (int i = 0; i < 2; i++) {
        pose.eye[i].viewPosition = toGlm(viewTransform[i].position);
        pose.eye[i].viewRotation = toGlm(viewTransform[i].orientation);
    }

    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    if (!scene_cloud_->IsMenuActive()) {
        devicePair.controllerState[0] = context_->input_state().ControllerState[0];
        devicePair.controllerState[1] = context_->input_state().ControllerState[1];
    }

    if (current_cloud_space_ == Space_Local) {
        // TODO config height
        // add fixed height for local space
        devicePair.controllerState[0].pose.position.y += CLOUD_LOCALSPACE_HEIGHT_OFFSET;
        devicePair.controllerState[1].pose.position.y += CLOUD_LOCALSPACE_HEIGHT_OFFSET;
    }

    static uint64_t frame_index = 0;
    frame_index++;

    trackingDevicePairFrame = {
            frame_index,
            now,
            static_cast<double>(predictedDisplayTime),
            devicePair,
    };
    return true;
}

void OxrApplication::OnNetworkAvailable() {
    Application::OnNetworkAvailable();
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void OxrApplication::OnNetworkLost() {
    Application::OnNetworkLost();
    Navigation::ShowToast("检测到系统网络断开，请检查网络");
    if (xr_client_) {
        xr_client_->OnPause();
    }
}
}
