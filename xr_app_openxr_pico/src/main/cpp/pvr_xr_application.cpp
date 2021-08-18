//
// Created by Administrator on 2021/7/21.
//

#include <env_context.h>
#include <asset_files.h>
#include <utils.h>
#include <log.h>
#include "pch.h"
#include "pvr_xr_application.h"
#include "check.h"
#include "pvr_xr_utils.h"

PvrXrApplication::PvrXrApplication() {
    RegiseredInstance(this);
}

PvrXrApplication::~PvrXrApplication() {
    UnRegiseredInstance();
}

bool PvrXrApplication::InitGL(OpenxrContext *context) {
    context_ = context;

    // load resource
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context_config = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context_config, Assetlist);
    // init scene
    scene_local_ = std::make_shared<PvrXRSceneLocal>();
    scene_local_->InitGL(context_->graphics_plugin());
    scene_cloud_ = std::make_shared<PvrXRSceneCloud>();
    scene_cloud_->InitGL(context_->graphics_plugin());

    // init xrconfig
    lark::XRConfig::fps = 72;
    for (int eye = 0; eye < 2; eye++) {
        lark::XRConfig::fov[eye] = {
                101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F
        };
    }

    lark::XRConfig::render_width = 2560;
    lark::XRConfig::render_height = 1440;
#ifdef USE_RENDER_QUEUE
    lark::XRConfig::use_render_queue = true;
#else
    lark::XRConfig::use_render_queue = false;
#endif
    lark::XRConfig::headset_desc.type = larkHeadSetType_PICO_3;
//    lark::XRConfig::use_multiview = false;

    // 初始化客户端接入凭证
    InitCertificate();

    xr_client_.reset();
    xr_client_ = std::make_shared<lark::XRClient>();
    // gl context not ready. init share context later.
    // init_share_context should be false
    xr_client_->Init(Context::instance()->vm(), false);
    xr_client_->InitGLShareContext();
    xr_client_->RegisterObserver(this);
#ifdef LARK_SDK_SECRET
    // 初始化 cloudlark sdk
                    std::string timestamp = utils::GetTimestampMillStr();
                    std::string signature = utils::GetSignature(LARK_SDK_ID, LARK_SDK_SECRET, timestamp);
                    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID, signature, timestamp)) {
                        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
                        Navigation::ShowToast(xr_client_->last_error_message());
                    }
#else
    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }
#endif

    LOGV("view cout %ld", context_->views().size());
    XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
    XrResult res;
    res = xrLocateSpace(context_->app_space(), context_->app_space(), 10, &spaceLocation);
    if (XR_UNQUALIFIED_SUCCESS(res)) {
        LOGV("xr head space x %f y %f z %f; rx %f ry %f rz %f rw %f",
                spaceLocation.pose.position.x,spaceLocation.pose.position.x, spaceLocation.pose.position.x,
                spaceLocation.pose.orientation.x, spaceLocation.pose.orientation.y, spaceLocation.pose.orientation.z, spaceLocation.pose.orientation.w);
    } else {
        LOGV("xr head space failed %d", res);
    }
    // init client
    // test loader
    std::string inputfile = std::string(Context::instance()->native_activity()->internalDataPath)
            + "/model/oculus_quest_controller_left/oculus_quest_controller_left.obj";

    lark::CopyAssetToInternalPath(Context::instance()->native_activity(),
                                  "model/oculus_quest_controller_left/oculus_quest_controller_left.obj");
    return false;
}

void PvrXrApplication::ShutdownGL() {
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
    }
}

void PvrXrApplication::CloseAppli() {
    LOGV("on close appli");
    if (xr_client_) {
        xr_client_->Close();
    }
}

void PvrXrApplication::Update() {
    if (connected_) {
        scene_cloud_->HandleInput(context_->input(), context_->session(), context_->app_space());
    } else {
        scene_local_->HandleInput(context_->input(), context_->session(), context_->app_space());
    }
}

void PvrXrApplication::RenderFrame() {
    if (xr_client_->is_connected() && xr_client_->media_ready() && !xr_client_->HasNewFrame()) {
        // update view
        XrResult res;

        auto views = context_->views();
        auto input = context_->input();
        auto app_space = context_->app_space();
        auto session = context_->session();
        auto config_views = context_->config_views();
        auto swapchain_images = context_->swapchain_images();
        auto swapchains = context_->swapchains();
        auto color_swapchain_format = context_->color_swapchain_format();

        XrViewState viewState{XR_TYPE_VIEW_STATE};
        uint32_t viewCapacityInput = (uint32_t)views.size();
        uint32_t viewCountOutput;

        XrViewLocateInfo viewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
        viewLocateInfo.viewConfigurationType = context_->view_config_type();
        viewLocateInfo.displayTime = 10;
        viewLocateInfo.space = context_->app_space();
        XrViewStatePICOEXT  xrViewStatePICOEXT;
        viewState.next=(void *)&xrViewStatePICOEXT;
        res = xrLocateViews(session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, views.data());
        gs_index_ = xrViewStatePICOEXT.gsIndex;
        view_state_pico_ = xrViewStatePICOEXT;

        if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
            (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
            return;  // There is no valid tracking poses for the views.
        }

//        LOGV("get pico pose pico %f %f %f; view[0] %f %f %f; view[1] %f %f %f",
//             xrViewStatePICOEXT.headpose.position.x, xrViewStatePICOEXT.headpose.position.y, xrViewStatePICOEXT.headpose.position.z,
//             views[0].pose.position.x, views[0].pose.position.y, views[0].pose.position.z,
//             views[1].pose.position.x, views[1].pose.position.y, views[1].pose.position.y);
        hmd_view_pose[0] = views[0].pose;
        hmd_view_pose[1] = views[1].pose;
        usleep(1);
        return;
    }

    bool is_connect = xr_client_->is_connected();
    bool cloudmedia_ready = xr_client_->media_ready();
    larkxrTrackingFrame trackingFrame;
    bool has_new_frame = xr_client_->Render(&trackingFrame);

    auto session = context_->session();
    CHECK(session != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    CHECK_XRCMD(xrWaitFrame(session, &frameWaitInfo, &frameState));

    if(frameState.predictedDisplayTime <= 0)
        frameState.predictedDisplayTime = 10;

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    CHECK_XRCMD(xrBeginFrame(session, &frameBeginInfo));

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
        if (RenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer, trackingFrame)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    xr_frame_end_info_ext_.type = XR_TYPE_FRAME_END_INFO;

    if (cloudmedia_ready && has_new_frame) {
//        LOGV("has new frame %d", trackingFrame.tracking.status);
        xr_frame_end_info_ext_.useHeadposeExt = 1;
        xr_frame_end_info_ext_.gsIndex = trackingFrame.tracking.rawPosition.x;
        xr_frame_end_info_ext_.poseStatus = trackingFrame.tracking.status;
        xr_frame_end_info_ext_.poseTimeStampNs = trackingFrame.tracking.timestamp;
        xr_frame_end_info_ext_.poseFetchTimeNs = trackingFrame.tracking.poseFetchTime;
        xr_frame_end_info_ext_.expectedDisplayTimeNs = trackingFrame.tracking.expectedDisplayTime;
        xr_frame_end_info_ext_.headpose.position = pvr::fromGlm(trackingFrame.tracking.position);
        xr_frame_end_info_ext_.headpose.orientation = pvr::fromGlm(trackingFrame.tracking.rotation);
    } else {
        xr_frame_end_info_ext_.useHeadposeExt = 1;
        xr_frame_end_info_ext_.gsIndex = gs_index_;
    }

    frameEndInfo.next = (void *)&xr_frame_end_info_ext_;
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = context_->environment_blend_mode();
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    CHECK_XRCMD(xrEndFrame(session, &frameEndInfo));
}

bool PvrXrApplication::RenderLayer(XrTime predictedDisplayTime,
                                   std::vector<XrCompositionLayerProjectionView> &projectionLayerViews,
                                   XrCompositionLayerProjection &layer, const larkxrTrackingFrame& trackingFrame) {
    XrResult res;

    auto views = context_->views();
    auto input = context_->input();
    auto app_space = context_->app_space();
    auto session = context_->session();
    auto config_views = context_->config_views();
    auto swapchain_images = context_->swapchain_images();
    auto swapchains = context_->swapchains();
    auto color_swapchain_format = context_->color_swapchain_format();

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
    gs_index_ = xrViewStatePICOEXT.gsIndex;
    view_state_pico_ = xrViewStatePICOEXT;

//    LOGV("get pico pose pico %f %f %f; view[0] %f %f %f; view[1] %f %f %f",
//            xrViewStatePICOEXT.headpose.position.x, xrViewStatePICOEXT.headpose.position.y, xrViewStatePICOEXT.headpose.position.z,
//            views[0].pose.position.x, views[0].pose.position.y, views[0].pose.position.z,
//            views[1].pose.position.x, views[1].pose.position.y, views[1].pose.position.y);
//    LOGV("get pico pose pico %f %f %f %f ; view[0] %f %f %f %f; view[1] %f %f %f %f",
//            xrViewStatePICOEXT.headpose.orientation.x, xrViewStatePICOEXT.headpose.orientation.y, xrViewStatePICOEXT.headpose.orientation.z, xrViewStatePICOEXT.headpose.orientation.w,
//            views[0].pose.orientation.x, views[0].pose.orientation.y, views[0].pose.orientation.z, views[0].pose.orientation.w,
//            views[1].pose.orientation.x, views[1].pose.orientation.y, views[1].pose.orientation.y, views[0].pose.orientation.w);

    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == config_views.size());
    CHECK(viewCountOutput == swapchains.size());

    projectionLayerViews.resize(viewCountOutput);

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = swapchains[i];

        XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};

        uint32_t swapchainImageIndex;
        CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

        XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        waitInfo.timeout = XR_INFINITE_DURATION;
        CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

        projectionLayerViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};

        if (xr_client_->is_connected() && xr_client_->media_ready()) {
            projectionLayerViews[i].pose.position = pvr::fromGlm(trackingFrame.tracking.eye[i].viewPosition);
            projectionLayerViews[i].pose.orientation = pvr::fromGlm(trackingFrame.tracking.eye[i].viewRotation);
        } else {
            projectionLayerViews[i].pose = views[i].pose;
        }
        projectionLayerViews[i].fov = views[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = {0, 0};
        projectionLayerViews[i].subImage.imageRect.extent = {viewSwapchain.width, viewSwapchain.height};

        const XrSwapchainImageBaseHeader* const swapchainImage = swapchain_images[viewSwapchain.handle][swapchainImageIndex];
//            m_graphicsPlugin->RenderView(projectionLayerViews[i], swapchainImage, m_colorSwapchainFormat, cubes);
        if (xr_client_->is_connected()) {
            scene_cloud_->RenderView(projectionLayerViews[i], swapchainImage, color_swapchain_format);
        } else {
            scene_local_->RenderView(projectionLayerViews[i], swapchainImage, color_swapchain_format);
        }

        XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));

//            projectionLayerViews[i].pose = {};
    }

    layer.space = app_space;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}

//

void PvrXrApplication::OnClose(int code) {
    LOGV("=========on close %d", code);
    connected_ = false;
//    has_new_frame_ = false;
//    cloud_tracking_ = {};
//    scene_cloud_->OnClose();
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
    if (errCode == 1) {
        // enter applifailed.
        scene_local_->HomePage();
    } else {
        connected_ = false;
        scene_local_->HomePage();
    }
    Navigation::ShowToast(msg);
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

    larkxrTrackedPose pose = {};
    pose.device = Larkxr_Device_Type_HMD;
    pose.isConnected = true;
    pose.is6Dof = true;
    pose.isValidPose = true;
    pose.rotation = pvr::toGlm(view_state_pico_.headpose.orientation);
    pose.position = pvr::toGlm(view_state_pico_.headpose.position);
    // timestamp
    pose.timestamp = view_state_pico_.poseTimeStampNs;
    pose.poseFetchTime = view_state_pico_.poseFetchTimeNs;
    pose.expectedDisplayTime = view_state_pico_.expectedDisplayTimeNs;
    pose.status = view_state_pico_.poseStatus;
    pose.rawPosition.x = gs_index_;

    for (int i = 0; i < 2; i++) {
        pose.eye[i].viewPosition = pvr::toGlm(hmd_view_pose[0].position);
        pose.eye[i].viewRotation = pvr::toGlm(hmd_view_pose[0].orientation);
    }

//    LOGV("update pose %f %f %f; %f %f %f",
//            view_state_pico_.headpose.position.x, view_state_pico_.headpose.position.y, view_state_pico_.headpose.position.z,
//            hmd_view_pose[0].position.x, hmd_view_pose[0].position.y, hmd_view_pose[0].position.z);

    larkxrDevicePair devicePair = {};
    devicePair.hmdPose = pose;

    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        devicePair.controllerState[hand].deviceType = (larkxrControllerType)(hand + 1);
        devicePair.controllerState[hand].pose.device = (larkxrDeviceType)(hand + 1);
        devicePair.controllerState[hand].inputState.deviceType = (larkxrDeviceType)(hand + 1);

        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrResult res;
        res = xrLocateSpace(context_->input().handSpace[hand], context_->app_space(), 10, &spaceLocation);
        if (!XR_UNQUALIFIED_SUCCESS(res)) {
            LOGV("get controller space failed.");
            devicePair.controllerState[hand].inputState.isConnected = false;
            devicePair.controllerState[hand].pose.isValidPose = false;
            continue;
        }
        devicePair.controllerState[hand].inputState.isConnected = true;
        devicePair.controllerState[hand].pose.isValidPose = true;
        devicePair.controllerState[hand].pose.isConnected = true;
        devicePair.controllerState[hand].pose.is6Dof = true;
        devicePair.controllerState[hand].pose.position = pvr::toGlm(spaceLocation.pose.position);
        devicePair.controllerState[hand].pose.rotation = pvr::toGlm(spaceLocation.pose.orientation);
//        LOGV("get controller space success. %f %f %f", spaceLocation.pose.position.x, spaceLocation.pose.position.y, spaceLocation.pose.position.z);
    }

    larkxrTrackingDevicePairFrame devicePairFrame = {
            0, 0, 0,
            devicePair,
    };
    xr_client_->SendDevicePair(devicePairFrame);
}

void PvrXrApplication::OnSyncPlayerSpace(larkxrPlaySpace *playSpace) {
    Application::OnSyncPlayerSpace(playSpace);
}
