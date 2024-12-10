//
// Created by Administrator on 2021/7/21.
//

#include <env_context.h>
#include <asset_files.h>
#include <utils.h>
#include <log.h>
#include <lark_xr/xr_latency_collector.h>
#include "pch.h"
#include "pvr_xr_application.h"
#include "check.h"
#include "pvr_xr_utils.h"
#include "ui/localization.h"
#include "build_config.h"

#define LOG_TAG "pvr_xr_application"

#define USE_RENDER_QUEUE = 1;

namespace {
    const float CLOUD_LOCALSPACE_HEIGHT_OFFSET = 1.5f;
}

PvrXrApplication::PvrXrApplication() {
    RegiseredInstance(this);
}

PvrXrApplication::~PvrXrApplication() {
    UnRegiseredInstance();
}

bool PvrXrApplication::InitGL(OpenxrContext *context) {
    context_ = context;

    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    CHECK_XRCMD(xrGetSystemProperties(context_->instance(), context_->system_id(), &systemProperties));

    localization::Loader::load(BuildConfig::is_en);

    // load resource
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context_config = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context_config, Assetlist);

    lark::XRConfig::fps = 72;
    lark::XRConfig::request_pose_fps = 72 * 2;

    // pico4
    // fov left -0.907414 right 0.907414 up 0.907414 down -0.907414
    // fov left -0.907414 right 0.907414 up 0.907414 down -0.907414
    // ipd 0.071930
    // viewSwapchain w 1440 h 1584

    // default 49 for pico neo3
    float fov = 49;

    if (lark::XRClient::system_info().platFromType != Larkxr_Platform_PICO_NEO_3) {
        fov = 105.0f / 2.0f;
        LOGV("setup fov for [pico 4][%f]", fov);
    } else {
        LOGV("setup fov for [pico neo3][%f]", fov);
    }

    for (int eye = 0; eye < 2; eye++) {
        lark::XRConfig::fov[eye] = {
                fov, fov, fov, fov
        };
    }

    lark::XRConfig::render_width = 3664;
    lark::XRConfig::render_height = 1920;

#ifdef USE_RENDER_QUEUE
    lark::XRConfig::use_render_queue = true;
    lark::XRConfig::render_queue_size = 1;
#else
    lark::XRConfig::use_render_queue = false;
#endif
    lark::XRConfig::headset_desc.type = larkHeadSetType_PICO_3;
    lark::XRConfig::use_multiview = true;

    if (context->display_refresh_rate() > 0) {
        lark::XRConfig::fps = context->display_refresh_rate();
    }

    LOGV("fps %d", context->display_refresh_rate());

    // lark::XRConfig::foveated_rendering.enableFoveateRendering = false;
    // test force hmd to htc
    // lark::XRConfig::set_force_headset_type(larkHeadSetType_HTC);

    // 初始化客户端接入凭证
    InitCertificate();

    xr_client_.reset();
    xr_client_ = std::make_shared<lark::XRClient>();
    // gl context not ready. init share context later.
    // init_share_context should be false
    xr_client_->Init(Context::instance()->vm(), false, BuildConfig::is_en ? "en" : "zh");
    xr_client_->InitGLShareContext();
    xr_client_->RegisterObserver(this);
    xr_client_->EnableDebugMode(false);

    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }

    LOGI("view cout %ld", context_->views().size());

    for(auto view: context_->views()) {
        LOGI("fov left %f right %f up %f down %f", view.fov.angleLeft, view.fov.angleRight, view.fov.angleUp, view.fov.angleDown);
    }

    // init scene
    scene_local_ = std::make_shared<PvrXRSceneLocal>();
    scene_local_->InitGL(context_->graphics_plugin());
    scene_cloud_ = std::make_shared<PvrXRSceneCloud>();

#ifdef ENABLE_CLOUDXR
    // create cloudxr before scene cloud inited
    cloudxr_client_ = std::make_shared<CloudXRClient>(this);
    cloudxr_client_->InitRenderParamsWithLarkXRConfig();
    cloudxr_client_->Init();
    // create cloudxr before scene cloud inited
    scene_cloud_->SetCloudXRClient(cloudxr_client_);
#endif

    scene_cloud_->InitGL(context_->graphics_plugin());

    return false;
}

void PvrXrApplication::ShutdownGL() {
    connected_ = false;

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
    }
#endif

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }

    // release cloudlark
    xr_client_->UnRegisterObserver();
    xr_client_->ReleaseGLShareContext();
    xr_client_->Release();
    xr_client_.reset();

    // reset all state.
    Input::ResetInput();
    Navigation::ClearToast();

    scene_local_.reset();
    scene_cloud_.reset();
    lark::AssetLoader::Release();
}

void PvrXrApplication::Update() {
    if (xr_client_->is_connected()) {
        scene_cloud_->HandleInput(context_->input(), context_->session(),  GetSelectedXRSpace());
#if 0
// TEST datachannel
        {
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.action = context_->input().AAction;
            XrActionStateBoolean AValue{XR_TYPE_ACTION_STATE_BOOLEAN};
            CHECK_XRCMD(xrGetActionStateBoolean(context_->session(), &getInfo, &AValue));
            if (AValue.changedSinceLastSync && AValue.isActive) {
                if (AValue.currentState) {
                    xr_client_->SendData("A press down");
                } else {
                    xr_client_->SendData("A press up");
                }
            }
            getInfo.action = context_->input().BAction;
            XrActionStateBoolean BValue{XR_TYPE_ACTION_STATE_BOOLEAN};
            CHECK_XRCMD(xrGetActionStateBoolean(context_->session(), &getInfo, &BValue));
            if (BValue.changedSinceLastSync && BValue.isActive) {
                if (AValue.currentState) {
                    xr_client_->SendData("B press down");
                } else {
                    xr_client_->SendData("B press up");
                }
            }
        }
#endif
    } else {
        scene_local_->HandleInput(context_->input(), context_->session(), GetSelectedXRSpace());
    }
}

void PvrXrApplication::RenderFrame() {
    /**
     * render frame
     *
     * xrWaitFrame
     * xrBeginFrame
     * wait cloud frame
     * render cloud frame
     * xrEndFrame
     */

    auto session = context_->session();
    CHECK(session != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    CHECK_XRCMD(xrWaitFrame(session, &frameWaitInfo, &frameState));

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(session, &frameBeginInfo));

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
#endif

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
//        LOGV("RENDER %d %d %d", has_new_frame_pxy_stream, has_new_frame_cloudxr, has_new_frame_pxy_stream || has_new_frame_cloudxr);
        if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};

    // PICO 2.2.0
    // https://developer-cn.pico-interactive.com/document/native/release-notes/
    // xr_frame_end_info_ext_.type = XR_TYPE_FRAME_END_INFO;
    // frameEndInfo.next = (void *)&xr_frame_end_info_ext_;

    frameEndInfo.next = nullptr;
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = context_->environment_blend_mode();
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    CHECK_XRCMD(xrEndFrame(session, &frameEndInfo));
}

bool PvrXrApplication::RenderLayer(XrTime predictedDisplayTime,
                                   std::vector<XrCompositionLayerProjectionView> &projectionLayerViews,
                                   XrCompositionLayerProjection &layer) {

    XrResult res;
    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)context_->views().size();
    uint32_t viewCountOutput;
    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = GetSelectedXRSpace();

    res = xrLocateViews(context_->session(), &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, context_->views().data());

    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        LOGW("xrLocateViews flag not valid %d", viewState.viewStateFlags);
        return false;  // There is no valid tracking poses for the views.
    }

    // get ipd
//    float ipd = sqrt(pow(abs(context_->views()[1].pose.position.x-context_->views()[0].pose.position.x),2)
//            +pow(abs(context_->views()[1].pose.position.y-context_->views()[0].pose.position.y),2)
//            +pow(abs(context_->views()[1].pose.position.z-context_->views()[0].pose.position.z),2));

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == 2);

    projectionLayerViews.resize(viewCountOutput);

    // LOGV("render viewCountOutput %d", viewCountOutput);

    if (!config_inited_) {
        glm::vec3 position[2] = {};
        for (int eye = 0; eye < 2; eye++) {
            position[eye] = pvr::toGlm(context_->views()[eye].pose.position);

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

    XrPosef pose[Side::COUNT];
    for (uint32_t i = 0; i < 2; i++) {
        pose[i] = context_->views()[i].pose;
    }

    lark::XRVideoFrame xrVideoFrame(0);
    larkxrTrackingFrame xrTrackingFrame = {};
    bool framevaild = false;
    bool media_ready = false;
    bool has_new_frame_cloudxr = false;

#ifdef ENABLE_CLOUDXR
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
        }

        {
            uint64_t frameIndex = latched.poseID;
            // LOGV("cloudxr latched poseID %ld", frameIndex);
            std::lock_guard<std::mutex> lock(tracking_frame_mutex_);
            auto it = tracking_frame_map_.find(frameIndex);
            if (it != tracking_frame_map_.end()) {
                xrTrackingFrame = it->second;
            } else {
                if (!tracking_frame_map_.empty()) {
                    LOGW("cant find new tracking frame in map. use old. size %ld; index %ld", tracking_frame_map_.size(), frameIndex);
                    xrTrackingFrame = tracking_frame_map_.cbegin()->second;
                } else {
                    LOGW("cant find tracking frame in map. size %ld; index %ld", tracking_frame_map_.size(), frameIndex);
                }
            }
        }
//        LOGV("CLOUDXR frame ready %ld %ld", trackingFrame.frameIndex, latched.poseID);
        has_new_frame_cloudxr = true;
        framevaild = true;
    }
#endif

    if (xr_client_->is_connected() && !has_new_frame_cloudxr) {
        media_ready = xr_client_->media_ready();

        if (media_ready) {
            // block wait frame
            xr_client_->WaitFroNewFrame(500);
            framevaild = xr_client_->Render(&xrTrackingFrame, &xrVideoFrame);

            if (framevaild) {
                lark::XRLatencyCollector::Instance().Rendered2(xrTrackingFrame.frameIndex);

                scene_cloud_->SetVideoFrame(xrVideoFrame);
            } else {
                LOGV("frame not vaild");
            }
        } else {
            // update.
            framevaild = xr_client_->Render(&xrTrackingFrame, &xrVideoFrame);
            media_ready = xr_client_->media_ready();
        }
    }

    if (framevaild) {
        // LOGV("setup cloud pose frameindex[%ld] %f %f %f", xrTrackingFrame.frameIndex, xrTrackingFrame.tracking.rotation.x, xrTrackingFrame.tracking.rotation.y, xrTrackingFrame.tracking.rotation.z);
        for (uint32_t i = 0; i < 2; i++) {
            pose[i].position = pvr::fromLarkXR(xrTrackingFrame.tracking.position);
            pose[i].orientation = pvr::fromLarkXR(xrTrackingFrame.tracking.rotation);
        }
    }

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < 2; i++) {
        picoxr::FrameBuffer frameBuffer = framevaild ? context_->frame_buffer_cloud(i) : context_->frame_buffer(i);

        frameBuffer.Acquire();

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        projectionLayerViews[i].pose = pose[i];
        projectionLayerViews[i].fov = context_->views()[i].fov;
        projectionLayerViews[i].subImage.swapchain = frameBuffer.color_swapchain().Handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {frameBuffer.width(), frameBuffer.height()};

        frameBuffer.SetCurrent();

        // LOGV("render eye[%d] texture_swapchain_index=%d texture_swapchain_length=%d", i, frameBuffer.texture_swapchain_index(), frameBuffer.texture_swapchain_length());

        glViewport(0, 0, projectionLayerViews[i].subImage.imageRect.extent.width, projectionLayerViews[i].subImage.imageRect.extent.height);

        uint64_t now = utils::GetTimestampUs();
        // LOGV("start render [%ld]", now);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // 开启透明同道混合
        glEnable( GL_BLEND );
        //配置混合方程式，默认为 GL_FUNC_ADD 方程
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (scene_cloud_->IsMenuActive() || has_new_frame_cloudxr || !framevaild) {
            // LOGV("clear color");
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        if (xr_client_->is_connected()) {
            scene_cloud_->RenderView((lark::Object::Eye)i, projectionLayerViews[i]);
       } else {
            scene_local_->RenderView((lark::Object::Eye)i, projectionLayerViews[i]);
        }

        // LOGV("cost render [%ld]", utils::GetTimestampUs() - now);

        frameBuffer.SetNone();

        // context_->graphics_plugin()->Swap();
    }

    for (int i = 0; i < 2; i++) {
        picoxr::FrameBuffer frameBuffer = framevaild ? context_->frame_buffer_cloud(i) : context_->frame_buffer(i);
        frameBuffer.Release();
    }

    if (framevaild && !has_new_frame_cloudxr) {
        XrSpaceLocation loc = {};
        loc.type = XR_TYPE_SPACE_LOCATION;

        // get head pose
        OXR(xrLocateSpace(
                context_->head_space(), GetSelectedXRSpace(), predictedDisplayTime, &loc));

        glm::vec3 trackingAng = glm::eulerAngles(pvr::toGlm(loc.pose.orientation));
        glm::vec3 renderAng = glm::eulerAngles(xrTrackingFrame.tracking.rotation.toGlm());

        float degree = glm::degrees(renderAng.y - trackingAng.y);

        lark::XRLatencyCollector::Instance().Submit(xrTrackingFrame.frameIndex, degree);
        xr_client_->ReleaseRenderTexture();
    }

#ifdef ENABLE_CLOUDXR
    if (has_new_frame_cloudxr) {
        cloudxr_client_->Release();
        cloudxr_client_->Stats();
    }
#endif

    layer.space = GetSelectedXRSpace();
    layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT;
    // layer.layerFlags = 0;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();

    return true;
}

//

void PvrXrApplication::OnClose(int code) {
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

void PvrXrApplication::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude,
                                         float duration, float frequency) {
    XrHapticVibration vibration{XR_TYPE_HAPTIC_VIBRATION};
    vibration.amplitude = amplitude;
    vibration.duration = duration;
    vibration.frequency = XR_TYPE_HAPTIC_VIBRATION;

    XrHapticActionInfo hapticActionInfo{XR_TYPE_HAPTIC_ACTION_INFO};
    hapticActionInfo.action = context_->input().vibrateAction;
    hapticActionInfo.subactionPath = context_->input().handSubactionPath[isLeft ? Side::LEFT : Side::RIGHT];
    CHECK_XRCMD(xrApplyHapticFeedback(context_->session(), &hapticActionInfo, (XrHapticBaseHeader*)&vibration));
}

void PvrXrApplication::OnConnected() {
    Application::OnConnected();
    connected_ = true;

    scene_cloud_->OnConnect();

}

void PvrXrApplication::OnError(int errCode, const char* msg) {
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

void PvrXrApplication::OnMediaReady(int nativeTextrure) {
    Application::OnMediaReady(nativeTextrure);

    scene_cloud_->OnMediaReady(nativeTextrure);

}

void PvrXrApplication::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    Application::OnMediaReady(nativeTextureLeft, nativeTextureRight);

    scene_cloud_->OnMediaReady(nativeTextureLeft, nativeTextureRight);

}

void PvrXrApplication::OnMediaReady() {
    Application::OnMediaReady();

    scene_cloud_->OnMediaReady();

}

void PvrXrApplication::RequestTrackingInfo() {
    Application::RequestTrackingInfo();

    uint64_t now = utils::GetTimestampNs();
    XrTime predictedDisplayTime = now + 1000 * 1000 * 40;

    XrSpaceVelocity velocity{XR_TYPE_SPACE_VELOCITY};
    XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION, &velocity};
    XrResult res = xrLocateSpace(context_->head_space(), GetSelectedXRSpace(), predictedDisplayTime, &spaceLocation);
    CHECK_XRRESULT(res, "xrLocateSpace");

    if (current_cloud_space_ == Space_Local) {
        spaceLocation.pose.position.y += CLOUD_LOCALSPACE_HEIGHT_OFFSET;
    }

    // save hmd pose for pop up menu
    scene_cloud_->set_headpose(spaceLocation.pose);

    static uint64_t frameIndex = 0;
    frameIndex++;

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;

    pose.rotation = pvr::toLarkXR(spaceLocation.pose.orientation);
    pose.position = pvr::toLarkXR(spaceLocation.pose.position);

    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        devicePair.controllerState[hand] = scene_cloud_->GetControllerState(hand);
    }

    larkxrTrackingDevicePairFrame devicePairFrame = {
            frameIndex,
            0,
            static_cast<double>(predictedDisplayTime),
            devicePair,
    };

    xr_client_->SendDevicePair(devicePairFrame);
}

void PvrXrApplication::OnSyncPlayerSpace(larkxrPlaySpace *playSpace) {
    Application::OnSyncPlayerSpace(playSpace);
}

void PvrXrApplication::OnResume() {
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void PvrXrApplication::OnPause() {
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

#ifdef ENABLE_CLOUDXR
void
PvrXrApplication::OnCloudXRReady(const char* appServerIp, const char* preferOutIp) {
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

void PvrXrApplication::UpdateClientState(cxrClientState state, cxrStateReason reason) {
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

void PvrXrApplication::ReceiveUserData(const void *data, uint32_t size) {

}

void PvrXrApplication::GetTrackingState(cxrVRTrackingState *state) {
    if (state == nullptr) {
        LOGW("cloudxr null tracking state");
        return;
    }

    uint64_t now = utils::GetTimestampNs();
    XrTime predictedDisplayTime = now + 1000 * 1000 * 40;

    XrSpaceVelocity velocity{XR_TYPE_SPACE_VELOCITY};
    XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION, &velocity};
    XrResult res = xrLocateSpace(context_->head_space(), GetSelectedXRSpace(), predictedDisplayTime, &spaceLocation);
    CHECK_XRRESULT(res, "xrLocateSpace");

    if (current_cloud_space_ == Space_Local) {
        spaceLocation.pose.position.y += CLOUD_LOCALSPACE_HEIGHT_OFFSET;
    }

    // save hmd pose for pop up menu
    scene_cloud_->set_headpose(spaceLocation.pose);

    static uint64_t frameIndex = 0;
    frameIndex++;

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;

    pose.rotation = pvr::toLarkXR(spaceLocation.pose.orientation);
    pose.position = pvr::toLarkXR(spaceLocation.pose.position);

    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        devicePair.controllerState[hand] = scene_cloud_->GetControllerState(hand);
    }

    larkxrTrackingDevicePairFrame devicePairFrame = {
            frameIndex,
            0,
            static_cast<double>(predictedDisplayTime),
            devicePair,
    };

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

//    LOGV("push tracking info %ld %ld %f %f %f", state->hmd.poseID, devicePairFrame.frameIndex,
//         state->hmd.pose.rotation.x,
//         state->hmd.pose.rotation.y,
//         state->hmd.pose.rotation.z);
}
#endif

void PvrXrApplication::SetupFPS(int fps) {
    Application::SetupFPS(fps);
    // TODO
    // PICO SDK 2.2.0

//    if (context_) {
//        LOGV("current fps %f", context_->GetFPS());
//        context_->SetFPS(fps);
//        LOGV("set fps %f", context_->GetFPS());
//    }
}

void PvrXrApplication::SetupSapce(Application::Space space) {
    Application::SetupSapce(space);
    current_cloud_space_ = space;
    scene_local_->SetupSapce(space == Space_Local);
    scene_cloud_->SetupSapce(space == Space_Local);
}

void PvrXrApplication::SetupSkyBox(int index) {
    Application::SetupSkyBox(index);

    // sync skybox
    // only support 0:textures/skybox_8_2k.jpg 1:textures/skybox_9.jpg
    scene_local_->SetSkyBox(index);
    scene_cloud_->SetSkyBox(index);
}

void PvrXrApplication::OnDataChannelOpen() {
    Application::OnDataChannelOpen();
    LOGV("***************OnDataChannelOpen");
}
