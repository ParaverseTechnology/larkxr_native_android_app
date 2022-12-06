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
#define LOG_TAG "pvr_xr_application"

#define USE_RENDER_QUEUE = 1;
//#define OPENXR_FOV_SETUP 1

const float ROOM_HEIGHT = 1.5f;

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

    // vendorId 42 pico neo4
    // systemProperties.vendorId;
    // Instance RuntimeName=Pico(XRT) by Pico et al 'de86b93' RuntimeVersion=3.0.1

    // neo3
    // Instance RuntimeName=Pico(XRT) by Pico et al '050e640' RuntimeVersion=3.0.1
    // System Properties: Name=Pico: PICO HMD VendorId=42
    // V/XrSystem: device name Pico Neo 3 MANUFACTURER Pico
    // device name Pico Neo 3 MANUFACTURER Pico
    //     MODEL Pico Neo 3
    //     BRAND Pico
    //     BOARD kona
    //     DEVICE PICOA7H10
    //     USER smartcm
    //     ID 4.8.18
    //     DISPLAY 4.8.18
    //     BOOTLOADER unknown
    //     PRODUCT A7H10
    //     HOST build20
    //     TAGS dev-keys
    //     FINGERPRINT Pico/A7H10/PICOA7H10:10/4.8.18/smartcm.1660654404:user/dev-keys
    //     HARDWARE qcom


    // load resource
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context_config = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context_config, Assetlist);

    // init xrconfig
    if (context_ && context_->GetFPS() != 0) {
        lark::XRConfig::fps = context_->GetFPS();
    } else {
        lark::XRConfig::fps = 72;
    }
    lark::XRConfig::request_pose_fps = 72 * 2;

    // pico4
    // fov left -0.907414 right 0.907414 up 0.907414 down -0.907414
    // fov left -0.907414 right 0.907414 up 0.907414 down -0.907414
    // ipd 0.071930
    // viewSwapchain w 1440 h 1584
#ifdef OPENXR_FOV_SETUP
    for (int eye = 0; eye < 2; eye++) {
//        lark::XRConfig::fov[eye] = {
//                47.4999, 47.4999, 47.4999, 47.4999
//        };
//        lark::XRConfig::fov[eye] = {
//                51.99148, 51.99148, 51.99148, 51.99148
//        };
        lark::XRConfig::fov[eye] = {
                52.5, 52.5, 52.5, 52.5
        };
    }

//    lark::XRConfig::render_width = 1440 * 2;
//    lark::XRConfig::render_height = 1584;
//    lark::XRConfig::render_width = 4320 * 2;
//    lark::XRConfig::render_height = 2160;
//    lark::XRConfig::render_width = 3664;
//    lark::XRConfig::render_height = 1920;

    lark::XRConfig::render_width = 3664;
    lark::XRConfig::render_height = 1687;
#else
    // default 49 for pico neo3
    float fov = 49;

    // vendorId 42 pico 4?
//    if (systemProperties.vendorId == 42) {
//        fov = 105.0f / 2.0f;
//    }

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
#endif

#ifdef USE_RENDER_QUEUE
    lark::XRConfig::use_render_queue = true;
#else
    lark::XRConfig::use_render_queue = false;
#endif
    lark::XRConfig::ipd = 0.0694f;
//    lark::XRConfig::ipd = 0.071930f;
    lark::XRConfig::headset_desc.type = larkHeadSetType_PICO_3;
    lark::XRConfig::use_multiview = true;
    lark::XRConfig::request_pose_fps = 72;
    // test force hmd to htc
    // lark::XRConfig::set_force_headset_type(larkHeadSetType_HTC);

    // 初始化客户端接入凭证
    InitCertificate();

    xr_client_.reset();
    xr_client_ = std::make_shared<lark::XRClient>();
    // gl context not ready. init share context later.
    // init_share_context should be false
    xr_client_->Init(Context::instance()->vm(), false);
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

    XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
    XrResult res;
    res = xrLocateSpace(context_->app_space(), context_->app_space(), 0, &spaceLocation);
    if (XR_UNQUALIFIED_SUCCESS(res)) {
        LOGV("xr head space x %f y %f z %f; rx %f ry %f rz %f rw %f",
                spaceLocation.pose.position.x,spaceLocation.pose.position.x, spaceLocation.pose.position.x,
                spaceLocation.pose.orientation.x, spaceLocation.pose.orientation.y, spaceLocation.pose.orientation.z, spaceLocation.pose.orientation.w);
    } else {
        LOGV("xr head space failed %d", res);
    }

    //    lark::CopyAssetToInternalPath(Context::instance()->native_activity(),
    //                                  "model/oculus_quest_controller_left/oculus_quest_controller_left.obj");

//    localization::Loader::load(true);

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

void PvrXrApplication::EnterAppli(const std::string &appId) {
    LOGV("on enter appli");
    if (xr_client_) {
        xr_client_->EnterAppli(appId);
//        xr_client_->EnterAppli("846813152229195776");
    }
}

void PvrXrApplication::EnterAppliParams(const lark::EnterAppliParams &params) {
    LOGV("on enter EnterAppliParams");
    if (xr_client_) {
        xr_client_->EnterAppli(params);
//        xr_client_->EnterAppli("846813152229195776");
    }
}

void PvrXrApplication::CloseAppli() {
    LOGV("on close appli");
    if (xr_client_) {
        xr_client_->Close();
    }
}

void PvrXrApplication::Update() {
    if (xr_client_->is_connected()) {
        scene_cloud_->HandleInput(context_->input(), context_->session(), context_->app_space());
    } else {
        scene_local_->HandleInput(context_->input(), context_->session(), context_->app_space());
    }
}

void PvrXrApplication::RenderFrame() {
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

    auto session = context_->session();
    CHECK(session != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    CHECK_XRCMD(xrWaitFrame(session, &frameWaitInfo, &frameState));

    if(frameState.predictedDisplayTime <= 0)
        frameState.predictedDisplayTime = 0;

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(session, &frameBeginInfo));

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
//        LOGV("RENDER %d %d %d", has_new_frame_pxy_stream, has_new_frame_cloudxr, has_new_frame_pxy_stream || has_new_frame_cloudxr);
        if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer, trackingFrame,
                        has_new_frame_pxy_stream || has_new_frame_cloudxr)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    xr_frame_end_info_ext_.type = XR_TYPE_FRAME_END_INFO;

    frameEndInfo.next = (void *)&xr_frame_end_info_ext_;
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = context_->environment_blend_mode();
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    CHECK_XRCMD(xrEndFrame(session, &frameEndInfo));

#ifdef USE_RENDER_QUEUE
    if (has_new_frame_pxy_stream) {
        auto views = context_->views();

        XrViewState viewState{XR_TYPE_VIEW_STATE};
        uint32_t viewCapacityInput = (uint32_t)views.size();
        uint32_t viewCountOutput;

        XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = context_->view_config_type();
        viewLocateInfo.displayTime = 0;
        viewLocateInfo.space = context_->app_space();
        XrViewStatePICOEXT  xrViewStatePICOEXT;
        viewState.next=(void *)&xrViewStatePICOEXT;
        xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, views.data());

        glm::vec3 trackingAng = glm::eulerAngles(pvr::toGlm(xrViewStatePICOEXT.headpose.orientation));
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

bool PvrXrApplication::RenderLayer(XrTime predictedDisplayTime,
                                   std::vector<XrCompositionLayerProjectionView> &projectionLayerViews,
                                   XrCompositionLayerProjection &layer, const larkxrTrackingFrame& trackingFrame, bool hasNewFrame) {
    XrResult res;

    auto views = context_->views();
    auto input = context_->input();
    auto app_space = context_->app_space();
    auto session = context_->session();
    auto config_views = context_->config_views();

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = context_->view_config_type();
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = app_space;
    XrViewStatePICOEXT  xrViewStatePICOEXT;
    viewState.next=(void *)&xrViewStatePICOEXT;
    res = xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, views.data());

//    LOGV("get pico pose pico %f %f %f; view[0] %f %f %f; view[1] %f %f %f",
//            xrViewStatePICOEXT.headpose.position.x, xrViewStatePICOEXT.headpose.position.y, xrViewStatePICOEXT.headpose.position.z,
//            views[0].pose.position.x, views[0].pose.position.y, views[0].pose.position.z,
//            views[1].pose.position.x, views[1].pose.position.y, views[1].pose.position.y);
//    LOGV("get pico pose pico %f %f %f %f ; view[0] %f %f %f %f; view[1] %f %f %f %f",
//            xrViewStatePICOEXT.headpose.orientation.x, xrViewStatePICOEXT.headpose.orientation.y, xrViewStatePICOEXT.headpose.orientation.z, xrViewStatePICOEXT.headpose.orientation.w,
//            views[0].pose.orientation.x, views[0].pose.orientation.y, views[0].pose.orientation.z, views[0].pose.orientation.w,
//            views[1].pose.orientation.x, views[1].pose.orientation.y, views[1].pose.orientation.y, views[0].pose.orientation.w);

//    for(auto view: views) {
//        LOGI("fov left %f right %f up %f down %f", view.fov.angleLeft, view.fov.angleRight, view.fov.angleUp, view.fov.angleDown);
//    }
//
      if (!config_inited_) {
        if (views.size() == 2) {
            glm::vec3 left(views[0].pose.position.x, views[0].pose.position.y, views[0].pose.position.z);
            glm::vec3 right(views[1].pose.position.x, views[1].pose.position.y, views[1].pose.position.z);
            LOGI("ipd %f", glm::distance(left, right));
            lark::XRConfig::ipd = glm::distance(left, right);
        }
        config_inited_ = true;
      }

    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == config_views.size());

    projectionLayerViews.resize(viewCountOutput);

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        picoxr::FrameBuffer frameBuffer = context_->frame_buffer(i);

        // Each view has a separate swapchain which is acquired, rendered to, and released.
        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};

        if (hasNewFrame) {
//            LOGV("CLOUDXR frame ready %ld", trackingFrame.frameIndex);
            projectionLayerViews[i].pose.position = pvr::fromGlm(trackingFrame.tracking.eye[i].viewPosition);
            projectionLayerViews[i].pose.orientation = pvr::fromGlm(trackingFrame.tracking.eye[i].viewRotation);
        } else {
            projectionLayerViews[i].pose = views[i].pose;
        }

        projectionLayerViews[i].fov = views[i].fov;

        projectionLayerViews[i].subImage.swapchain =
                frameBuffer.color_swapchain().Handle;
        projectionLayerViews[i].subImage.imageRect.offset.x = 0;
        projectionLayerViews[i].subImage.imageRect.offset.y = 0;
        projectionLayerViews[i].subImage.imageRect.extent.width =
                frameBuffer.color_swapchain().Width;
        projectionLayerViews[i].subImage.imageRect.extent.height =
                frameBuffer.color_swapchain().Height;
        projectionLayerViews[i].subImage.imageArrayIndex = 0;

//        LOGI("viewSwapchain w %d h %d", viewSwapchain.width, viewSwapchain.height);

        if (xr_client_->is_connected()) {
            scene_cloud_->RenderView(static_cast<lark::Object::Eye>(i),  projectionLayerViews[i], frameBuffer);
        } else {
            scene_local_->RenderView(static_cast<lark::Object::Eye>(i), projectionLayerViews[i], frameBuffer);
        }
    }

    layer.space = app_space;
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
//    ResetTracking(pvr::PvrTrackingOrigin_EyeLevel);
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

void PvrXrApplication::OnError(int errCode, const std::string &msg) {
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

//    LOGV("RequestTrackingInfo %ld %f", utils::GetTimestampUs(), utils::GetTimeInSeconds());

    // scene_cloud_->HandleInput(context_->input(), context_->session(), context_->app_space());

    auto views = context_->views();
    auto session = context_->session();

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = context_->view_config_type();

    // xr time
//    struct timespec now;
//    clock_gettime( CLOCK_MONOTONIC, &now );
//    XrTime xr_now = {};
//    xrConvertTimespecTimeToTimeKHR(context_->instance(), &now, &xr_now);
    uint64_t now = utils::GetTimestampNs();

    // TODO predit pose time
    viewLocateInfo.displayTime = now + 1000 * 1000 * 50;
//    viewLocateInfo.displayTime = utils::GetTimestampUs() + 20;
//    viewLocateInfo.displayTime = 0;
    viewLocateInfo.space = context_->app_space();
    XrViewStatePICOEXT  xrViewStatePICOEXT;
    viewState.next=(void *)&xrViewStatePICOEXT;
    xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, views.data());

    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return;  // There is no valid tracking poses for the views.
    }

    // save hmd pose for pop up menu
    scene_cloud_->set_view_state(xrViewStatePICOEXT);

//    LOGV("get pico pose pico %f %f %f; view[0] %f %f %f; view[1] %f %f %f",
//         xrViewStatePICOEXT.headpose.orientation.x, xrViewStatePICOEXT.headpose.orientation.y, xrViewStatePICOEXT.headpose.orientation.z,
//         views[0].pose.orientation.x, views[0].pose.orientation.y, views[0].pose.orientation.z,
//         views[1].pose.orientation.x, views[1].pose.orientation.y, views[1].pose.orientation.y);

//    LOGV("xrViewStatePICOEXT xr_now[%ld] displayTime[%ld] poseTimeStampNs[%ld] poseFetchTimeNs[%ld] expectedDisplayTimeNs[%ld]",
//            now,
//            viewLocateInfo.displayTime,
//            xrViewStatePICOEXT.poseTimeStampNs,
//            xrViewStatePICOEXT.poseFetchTimeNs,
//            xrViewStatePICOEXT.expectedDisplayTimeNs
//         );

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;
    pose.rotation = pvr::toGlm(xrViewStatePICOEXT.headpose.orientation);
    pose.position = pvr::toGlm(xrViewStatePICOEXT.headpose.position);
    // TODO handel room height
    pose.position.y += ROOM_HEIGHT;
    // timestamp
    pose.timestamp = xrViewStatePICOEXT.poseTimeStampNs;
    pose.poseFetchTime = xrViewStatePICOEXT.poseFetchTimeNs;
    pose.expectedDisplayTime = xrViewStatePICOEXT.expectedDisplayTimeNs;
    pose.status = xrViewStatePICOEXT.poseStatus;
    pose.rawPosition.x = xrViewStatePICOEXT.gsIndex;

    for (int i = 0; i < 2; i++) {
        pose.eye[i].viewPosition = pvr::toGlm(views[i].pose.position);
        pose.eye[i].viewRotation = pvr::toGlm(views[i].pose.orientation);
    }

//    LOGV("update pose %f %f %f; %f %f %f",
//            view_state_pico_.headpose.position.x, view_state_pico_.headpose.position.y, view_state_pico_.headpose.position.z,
//            hmd_view_pose[0].position.x, hmd_view_pose[0].position.y, hmd_view_pose[0].position.z);

    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        devicePair.controllerState[hand] = scene_cloud_->GetControllerState(hand);
        devicePair.controllerState[hand].pose.position.y += ROOM_HEIGHT;

//        devicePair.controllerState[hand].pose.position = {hand * 0.5, 0, 0};
//        devicePair.controllerState[hand].pose.position = {0, 0, 0};
//        devicePair.controllerState[hand].pose.rotation = {};

//        devicePair.controllerState[hand].deviceType = hand == Side::LEFT ? Larkxr_Controller_Left : Larkxr_Controller_Right;
//        devicePair.controllerState[hand].pose.device = hand == Side::LEFT ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;
//        devicePair.controllerState[hand].pose.isConnected = true;
//        devicePair.controllerState[hand].pose.isValidPose = true;
//        devicePair.controllerState[hand].inputState.deviceType = hand == Side::LEFT ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;
//        devicePair.controllerState[hand].inputState.isConnected = true;

    }

//    devicePair.controllerState[0].pose.position = {-0.4, ROOM_HEIGHT, 0};
//    devicePair.controllerState[0].pose.rotation = {};
//    devicePair.controllerState[1].pose.position = {0.4, ROOM_HEIGHT, 0};
//    devicePair.controllerState[1].pose.rotation = {};

//    LOGV("request left2 %d %d %d %f %f %f  %frequest right %d %d %d %f %f %f %f;h %f; %d %d",
//         devicePair.controllerState[0].deviceType, devicePair.controllerState[0].pose.isConnected, devicePair.controllerState[0].inputState.isConnected,
//         devicePair.controllerState[0].pose.rotation.x, devicePair.controllerState[0].pose.rotation.y, devicePair.controllerState[0].pose.rotation.z, devicePair.controllerState[0].pose.rotation.w,
//         devicePair.controllerState[1].deviceType, devicePair.controllerState[1].pose.isConnected, devicePair.controllerState[1].inputState.isConnected,
//         devicePair.controllerState[1].pose.rotation.x, devicePair.controllerState[1].pose.rotation.y, devicePair.controllerState[1].pose.rotation.z, devicePair.controllerState[1].pose.rotation.w,
//         ROOM_HEIGHT, devicePair.controllerState[0].deviceType, devicePair.controllerState[1].deviceType);

    larkxrTrackingDevicePairFrame devicePairFrame = {
            static_cast<uint64_t>(xrViewStatePICOEXT.gsIndex),
            xrViewStatePICOEXT.poseFetchTimeNs,
            static_cast<double>(xrViewStatePICOEXT.expectedDisplayTimeNs),
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
PvrXrApplication::OnCloudXRReady(const std::string &appServerIp, const std::string &preferOutIp) {
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
//    LOGV("cloudxr get tracking state %ld", state->hmd.poseID);

    static uint64_t frameIndex = 0;
    frameIndex++;

    auto views = context_->views();
    auto session = context_->session();

    XrViewState viewState{XR_TYPE_VIEW_STATE};
    uint32_t viewCapacityInput = (uint32_t)views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    viewLocateInfo.viewConfigurationType = context_->view_config_type();
    viewLocateInfo.displayTime = utils::GetTimestampUs() + 20;
//    viewLocateInfo.displayTime = 10;
    viewLocateInfo.space = context_->app_space();
    XrViewStatePICOEXT  xrViewStatePICOEXT;
    viewState.next=(void *)&xrViewStatePICOEXT;
    xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, views.data());

    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return;  // There is no valid tracking poses for the views.
    }

    // save hmd pose for pop up menu
    scene_cloud_->set_view_state(xrViewStatePICOEXT);

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;
    pose.rotation = pvr::toGlm(xrViewStatePICOEXT.headpose.orientation);
    pose.position = pvr::toGlm(xrViewStatePICOEXT.headpose.position);
    // TODO handel room height
    pose.position.y += ROOM_HEIGHT;
    // timestamp
    pose.timestamp = xrViewStatePICOEXT.poseTimeStampNs;
    pose.poseFetchTime = xrViewStatePICOEXT.poseFetchTimeNs;
    pose.expectedDisplayTime = xrViewStatePICOEXT.expectedDisplayTimeNs;
    pose.status = xrViewStatePICOEXT.poseStatus;
    pose.rawPosition.x = xrViewStatePICOEXT.gsIndex;

    for (int i = 0; i < 2; i++) {
        pose.eye[i].viewPosition = pvr::toGlm(views[i].pose.position);
        pose.eye[i].viewRotation = pvr::toGlm(views[i].pose.orientation);
    }

    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        devicePair.controllerState[hand] = scene_cloud_->GetControllerState(hand);
        devicePair.controllerState[hand].pose.position.y += ROOM_HEIGHT;
    }

    larkxrTrackingDevicePairFrame devicePairFrame = {
            frameIndex,
            xrViewStatePICOEXT.poseFetchTimeNs,
            static_cast<double>(xrViewStatePICOEXT.expectedDisplayTimeNs),
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

//    LOGV("push tracking info %ld %ld", state->hmd.poseID, devicePairFrame.frameIndex);
}
#endif

void PvrXrApplication::SetupFPS(int fps) {
    Application::SetupFPS(fps);
    if (context_) {
        LOGV("current fps %f", context_->GetFPS());
        context_->SetFPS(fps);
        LOGV("set fps %f", context_->GetFPS());
    }
}

void PvrXrApplication::SetupSapce(Application::Space space) {
    Application::SetupSapce(space);
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
