//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <env_context.h>
#include <asset_files.h>
#include <lark_xr/xr_latency_collector.h>
#include "hxr_application.h"
#include "hxr_utils.h"

#define LOG_TAG "hxr_application"

#define USE_RENDER_QUEUE 1

namespace {
    const float CLOUD_LOCALSPACE_HEIGHT_OFFSET = 1.5f;
}

namespace hxr {
HxrApplication::HxrApplication() {
    RegiseredInstance(this);
}

HxrApplication::~HxrApplication() {
    UnRegiseredInstance();
}

bool HxrApplication::InitGL(OpenxrContext *context) {
    context_ = context;

    // 开启透明同道混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    // 初始化客户端接入凭证
    InitCertificate();

    {
        // configs
        lark::XRConfig::render_width = 1920;
        lark::XRConfig::render_height = 1080;

        // lark::XRConfig::headset_desc.type = larkHeadSetType_OCULUS;
        // 3dof -> htc
        lark::XRConfig::headset_desc.type = larkHeadSetType_HTC;
#ifdef USE_RENDER_QUEUE
        lark::XRConfig::use_render_queue = true;
#else
        lark::XRConfig::use_render_queue = false;
#endif
        lark::XRConfig::fps = 60;
        // lark::XRConfig::request_pose_fps = 60;
        lark::XRConfig::request_pose_fps = lark::XRConfig::fps * 2;
        lark::XRConfig::use_multiview = true;

        for (int eye = 0; eye < 2; eye++) {
            lark::XRConfig::fov[eye].left = 46;
            lark::XRConfig::fov[eye].right = 46;
            lark::XRConfig::fov[eye].top = 46;
            lark::XRConfig::fov[eye].bottom = 46;
        }

        lark::XRConfig::QuickConfigWithDefaulSetup(lark::QuickConfigLevel_Fast);
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

    // create cloudxr before scene cloud inited
    cloudxr_client_->InitRenderParamsWithLarkXRConfig();
    cloudxr_client_->Init();
#endif

    scene_cloud_->InitGL();

    return true;
}

void HxrApplication::ShutdownGL() {
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

void HxrApplication::OnResume() {
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void HxrApplication::OnPause() {
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

void HxrApplication::Update() {
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

void HxrApplication::RenderFrame() {
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

    // LOGV("cloudmedia_ready %d", cloudmedia_ready);

    if (!has_new_frame_cloudxr && xr_client_->is_connected()) {
        has_new_frame_pxy_stream = xr_client_->Render(&trackingFrame, &xrVideoFrame);

        // LOGV("cloudmedia_ready %d has_new_frame_pxy_stream %d", cloudmedia_ready, has_new_frame_pxy_stream);

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

    xrWaitFrame(context_->session(), &waitFrameInfo, &frameState);

    // Get the HMD pose, predicted for the middle of the time period during which
    // the new eye images will be displayed. The number of frames predicted ahead
    // depends on the pipeline depth of the engine and the synthesis rate.
    // The better the prediction, the less black will be pulled in at the edges.
    XrFrameBeginInfo beginFrameDesc = {};
    beginFrameDesc.type = XR_TYPE_FRAME_BEGIN_INFO;
    beginFrameDesc.next = NULL;
    xrBeginFrame(context_->session(), &beginFrameDesc);

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
        xrLocateSpace(
                context_->head_space(), GetSelectedXRSpace(), frameState.predictedDisplayTime, &loc);

        glm::vec3 trackingAng = glm::eulerAngles(toGlm(loc.pose.orientation));
        glm::vec3 renderAng = glm::eulerAngles(trackingFrame.tracking.rotation.toGlm());
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

bool HxrApplication::RenderLayer(XrTime predictedDisplayTime,
                                 std::vector<XrCompositionLayerProjectionView> &projectionLayerViews,
                                 XrCompositionLayerProjection &layer,
                                 const larkxrTrackingFrame& trackingFrame, bool hasNewFrame) {

    std::vector<XrView> mViews;
    mViews.resize(2, {XR_TYPE_VIEW});
    //1. Locate Views

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = context_->app_space();   //mView相对于世界坐标系

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    viewState.viewStateFlags = 0;
    uint32_t viewCapacityInput = 2;
    uint32_t viewCountOutput;

    xrLocateViews(context_->session(), &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, mViews.data());
    // LOGI("after xrLocateViews viewCountOutput:%d",viewCountOutput);
    // LOGI("xrLocateViews viewStateFlags : %u", (unsigned int)viewState.viewStateFlags);

    //2. Prepare LayerViews
    projectionLayerViews.resize(viewCountOutput);

    //3. Swapchain
    // LOGI("viewCountOutput:%d",viewCountOutput);
    for (int i = 0; i < viewCountOutput; i++)   //viewCountOutput ==> 1
    {
        //3.1 Acquire Image
        uint32_t swapchainImageIndex;
        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        xrAcquireSwapchainImage(context_->swapchains()[i], &acquireInfo, &swapchainImageIndex);

        //3.2 Wait Image
        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        xrWaitSwapchainImage(context_->swapchains()[i], &waitInfo);

        //3.3 Prepare LayerViews
        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};

        if (hasNewFrame) {
//            LOGV("CLOUDXR frame ready %ld", trackingFrame.frameIndex);
            projectionLayerViews[i].pose.position = fromGlm(trackingFrame.tracking.eye[i].viewPosition.toGlm());
            projectionLayerViews[i].pose.orientation = fromGlm(trackingFrame.tracking.eye[i].viewRotation.toGlm());
        } else {
            projectionLayerViews[i].pose = mViews[i].pose;
        }

        // projectionLayerViews[i].pose = mViews[i].pose;
/*        LOGI("i = %d : pose xyz:%f,%f,%f",i,mViews[i].pose.position.x,mViews[i].pose.position.y,mViews[i].pose.position.z);
        LOGI("i = %d : pose orientation:%f,%f,%f,%f",i,mViews[i].pose.orientation.x,mViews[i].pose.orientation.y,mViews[i].pose.orientation.z,mViews[i].pose.orientation.w);
        LOGI("i = %d : fov:%f,%f,%f,%f",i,mViews[i].fov.angleUp,mViews[i].fov.angleDown,mViews[i].fov.angleLeft,mViews[i].fov.angleRight);*/
        projectionLayerViews[i].fov = mViews[i].fov;
        projectionLayerViews[i].subImage.swapchain = context_->swapchains()[i];
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {1080, 1080};
        projectionLayerViews[i].subImage.imageArrayIndex = 0;
        const XrSwapchainImageBaseHeader* const swapchainImage = (XrSwapchainImageBaseHeader*)(&(context_->swapchains_image_array()[i][swapchainImageIndex]));
        //3.4 >>>Graphic Render>>>

        if (xr_client_->is_connected()) {
            scene_cloud_->Render((lark::Object::Eye)i, projectionLayerViews[i]);
        } else {
            scene_local_->Render((lark::Object::Eye)i, projectionLayerViews[i]);
        }

        //3.5 Release Image
        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        xrReleaseSwapchainImage(context_->swapchains()[i], &releaseInfo);

    }

    //4. Make Layer
    layer.next = nullptr;
    layer.layerFlags = 0;
    layer.space = context_->app_space();
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    // LOGI("layer.viewCount:%d",layer.viewCount);
    layer.views = projectionLayerViews.data();

    return true;
}

void HxrApplication::OnConnected() {
    Application::OnConnected();
    connected_ = true;
    scene_cloud_->OnConnect();
}

void HxrApplication::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude,
                                       float duration, float frequency) {
    Application::OnHapticsFeedback(isLeft, startTime, amplitude, duration, frequency);
}

void HxrApplication::OnError(int errCode, const char* msg) {
    Application::OnError(errCode, msg);
    LOGE("on xr client error %d; msg %s;", errCode, msg);

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

void HxrApplication::OnClose(int code) {
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

void HxrApplication::OnMediaReady(int nativeTextrure) {
    Application::OnMediaReady(nativeTextrure);
    scene_cloud_->OnMediaReady(nativeTextrure);
}

void HxrApplication::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    Application::OnMediaReady(nativeTextureLeft, nativeTextureRight);
    scene_cloud_->OnMediaReady(nativeTextureLeft, nativeTextureRight);
}

void HxrApplication::OnMediaReady() {
    Application::OnMediaReady();
    scene_cloud_->OnMediaReady();
}

void HxrApplication::RequestTrackingInfo() {
    Application::RequestTrackingInfo();

    larkxrTrackingDevicePairFrame devicePairFrame = {};

    if (UpdateCloudTrackingState(devicePairFrame)) {
        xr_client_->SendDevicePair(devicePairFrame);
    }
}

void HxrApplication::OnSyncPlayerSpace(larkxrPlaySpace *playSpace) {
    Application::OnSyncPlayerSpace(playSpace);
}

#ifdef ENABLE_CLOUDXR
void
HxrApplication::OnCloudXRReady(const char* appServerIp, const char* preferOutIp) {
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

void HxrApplication::UpdateClientState(cxrClientState state, cxrStateReason reason) {
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

void HxrApplication::ReceiveUserData(const void *data, uint32_t size) {

}

void HxrApplication::GetTrackingState(cxrVRTrackingState *state) {
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

void HxrApplication::SetupSapce(Space space) {
    Application::SetupSapce(space);
    current_cloud_space_ = space;
}

void HxrApplication::SetupSkyBox(int index) {
    Application::SetupSkyBox(index);
    // sync skybox
    // only support 0:textures/skybox_8_2k.jpg 1:textures/skybox_9.jpg
    scene_local_->SetSkyBox(index);
    scene_cloud_->SetSkyBox(index);
}
bool HxrApplication::UpdateCloudTrackingState(larkxrTrackingDevicePairFrame& trackingDevicePairFrame) {
    uint64_t now = utils::GetTimestampNs();
    XrTime predictedDisplayTime = now + 1000 * 1000 * 50;
//    XrTime predictedDisplayTime = 0;
    XrSpace space = GetSelectedXRSpace();

    XrSpaceLocation spaceLocation_h{XR_TYPE_SPACE_LOCATION};
    xrLocateSpace(context_->input_state().HeadSpace, space, predictedDisplayTime, &spaceLocation_h);
    // LOGI("spaceLocation_h_position:%f,%f,%f",spaceLocation_h.pose.position.x,spaceLocation_h.pose.position.y,spaceLocation_h.pose.position.z);
    // LOGI("spaceLocation_h_orientation:%f,%f,%f,%f",spaceLocation_h.pose.orientation.x,spaceLocation_h.pose.orientation.y,spaceLocation_h.pose.orientation.z,spaceLocation_h.pose.orientation.w);
    // LOGI("spaceLocation_h_locationFlags: %u", (unsigned int)spaceLocation_h.locationFlags);
    // save hmd pose for pop up menu
    scene_cloud_->set_headpose(spaceLocation_h.pose);

    if (current_cloud_space_ == Space_Local) {
        // TODO config height
        // add fixed height for local space
        spaceLocation_h.pose.position.y += CLOUD_LOCALSPACE_HEIGHT_OFFSET;
    }

    std::vector<XrView> mViews;
    mViews.resize(2, {XR_TYPE_VIEW});
    //1. Locate Views

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = context_->app_space();   //mView相对于世界坐标系

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    viewState.viewStateFlags = 0;
    uint32_t viewCapacityInput = 2;
    uint32_t viewCountOutput;

    xrLocateViews(context_->session(), &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, mViews.data());

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;
    pose.rotation = toGlm(spaceLocation_h.pose.orientation);
    pose.position = toGlm(spaceLocation_h.pose.position);

    for (int i = 0; i < 2; i++) {
        pose.eye[i].viewPosition = toGlm(mViews[i].pose.position);
        pose.eye[i].viewRotation = toGlm(mViews[i].pose.orientation);
    }


    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    if (!scene_cloud_->IsMenuActive()) {
        devicePair.controllerState[0] = context_->input_state().ControllerState[0];
        devicePair.controllerState[1] = context_->input_state().ControllerState[1];
    }

/*
    LOGV("devicePair.controllerState[0] isConnected=%d; isValid=%d; px: %f; py: %f; pz: %f", devicePair.controllerState[0].pose.isConnected, devicePair.controllerState[0].pose.isValidPose,
         devicePair.controllerState[0].pose.position.x, devicePair.controllerState[0].pose.position.y, devicePair.controllerState[0].pose.position.z);

    LOGV("devicePair.controllerState[1] isConnected=%d; isValid=%d; px: %f; py: %f; pz: %f", devicePair.controllerState[1].pose.isConnected, devicePair.controllerState[1].pose.isValidPose,
         devicePair.controllerState[1].pose.position.x, devicePair.controllerState[1].pose.position.y, devicePair.controllerState[1].pose.position.z);
*/

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

void HxrApplication::OnNetworkAvailable() {
    Application::OnNetworkAvailable();
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void HxrApplication::OnNetworkLost() {
    Application::OnNetworkLost();
    Navigation::ShowToast("检测到系统网络断开，请检查网络");
    if (xr_client_) {
        xr_client_->OnPause();
    }
}
}

