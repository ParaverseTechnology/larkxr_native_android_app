//
// Created by fcx on 2020/5/6.
//
#include <GLES3/gl31.h>
#include <EGL/egl.h>
#include <wvr/wvr_projection.h>
#include "wvr/wvr.h"
#include "wvr/wvr_render.h"
#include <wvr/wvr_types.h>
#include <wvr/wvr_device.h>
#include "wave_application.h"
#include "env_context.h"
#include <log.h>
#include <lark_xr/xr_config.h>
#include <asset_loader.h>
#include <asset_files.h>
#include <unistd.h>
#include <utils.h>
#include <wvr/wvr_system.h>
#include "math.h"
#include "wvr_utils.h"

#define LOG_TAG "wave_app"

#define USE_RENDER_QUEUE 1;

#define DISABLE_ADAPTIVE_QUALITY 1


static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

bool WaveApplication::s_is_vive_flow_ = false;

void WaveApplication::set_is_vive_flow(bool isFlow) {
    LOGI("is_vive_flow %d", isFlow);
    s_is_vive_flow_ = isFlow;
}

bool WaveApplication::s_is_vive_flow() {
    return s_is_vive_flow_;
}

WaveApplication::WaveApplication() {
    RegiseredInstance(this);
}
WaveApplication::~WaveApplication() {
    UnRegiseredInstance();
}

bool WaveApplication::InitVR() {
    LOGENTRY();
    LOGI("MainApplication::initVR()");
    // Loading the WVR Runtime
    WVR_InitError eError = WVR_InitError_None;
    LOGI("initVR():start call WVR_Init");
    eError = WVR_Init(WVR_AppType_VRContent);
    LOGI("initVR():start call WVR_Init end");
    if (eError != WVR_InitError_None) {
        LOGE("Unable to init VR runtime: %s", WVR_GetInitErrorString(eError));
        return false;
    }

    WVR_InputAttribute inputIdAndTypes[] = {
            {WVR_InputId_Alias1_Menu, WVR_InputType_Button, WVR_AnalogType_None},
            {WVR_InputId_Alias1_Touchpad, WVR_InputType_Button | WVR_InputType_Touch | WVR_InputType_Analog, WVR_AnalogType_2D},
            {WVR_InputId_Alias1_Trigger, WVR_InputType_Button , WVR_AnalogType_None},
            {WVR_InputId_Alias1_Digital_Trigger, WVR_InputType_Button , WVR_AnalogType_None}
    };

    WVR_SetInputRequest(WVR_DeviceType_HMD, inputIdAndTypes, sizeof(inputIdAndTypes) / sizeof(*inputIdAndTypes));
    WVR_SetInputRequest(WVR_DeviceType_Controller_Right, inputIdAndTypes, sizeof(inputIdAndTypes) / sizeof(*inputIdAndTypes));
    WVR_SetInputRequest(WVR_DeviceType_Controller_Left, inputIdAndTypes, sizeof(inputIdAndTypes) / sizeof(*inputIdAndTypes));

    WVR_SetArmModel(WVR_SimulationType_Auto); //add for use arm mode.

    // Must initialize render runtime before all OpenGL code.
    WVR_RenderInitParams_t param;
    param = { WVR_GraphicsApiType_OpenGL, WVR_RenderConfig_Default };

    WVR_RenderError pError = WVR_RenderInit(&param);
    if (pError != WVR_RenderError_None) {
        LOGE("Present init failed - Error[%d]", pError);
    }

#if defined(USE_PERDIT_TIME)
    // added in 3.0.2
     float ratio = 1.1F;
     WVR_SetOverfillRatio(ratio, ratio);
#endif
    // scale width and height to ecrease the
    // black area during the processing of ATW.
    float ratio = 1.05F;
    WVR_SetOverfillRatio(ratio, ratio);

    uint32_t version = WVR_GetWaveRuntimeVersion();
    LOGV("WaveRuntimeVersion: %d", version);
    version = WVR_GetWaveSDKVersion();
    LOGV("WaveWaveSDKVersion: %d", version);

    // init scene
    scene_local_ = std::make_shared<WvrSceneLocal>();
    scene_local_->InitVR();

    // init larkxr setup.
    {
        float left, right, top, bottom;
        float aLeft, aRight, aTop, aBottom;
        WVR_GetClippingPlaneBoundary(WVR_Eye_Left, &left, &right, &top, &bottom);
        aLeft = static_cast<float>(atanf(left) * 180.0f / M_PI);
        aRight = static_cast<float>(atanf(right) * 180.0f / M_PI);
        aTop = static_cast<float>(atanf(top) * 180.0f / M_PI);
        aBottom = static_cast<float>(atan(bottom) * 180.0f / M_PI);
        LOGI("L left: %f; right: %f; top: %f; bottom: %f;aLeft: %f; aRight: %f; aTop: %f; aBotttom: %f",
             left, right, top, bottom,
             aLeft, aRight, aTop, aBottom);

        // setup left
        lark::XRConfig::fov[0] = {
                 abs(aLeft), abs(aRight), abs(aTop), abs(aBottom)
        };

        WVR_GetClippingPlaneBoundary(WVR_Eye_Right, &left, &right, &top, &bottom);
        aLeft = static_cast<float>(atanf(left) * 180.0f / M_PI);
        aRight = static_cast<float>(atanf(right) * 180.0f / M_PI);
        aTop = static_cast<float>(atanf(top) * 180.0f / M_PI);
        aBottom = static_cast<float>(atan(bottom) * 180.0f / M_PI);
        LOGI("L left: %f; right: %f; top: %f; bottom: %f;aLeft: %f; aRight: %f; aTop: %f; aBotttom: %f",
             left, right, top, bottom,
             aLeft, aRight, aTop, aBottom);
        // setup right
        lark::XRConfig::fov[1] = {
                abs(aLeft), abs(aRight), abs(aTop), abs(aBottom)
        };

        render_width_ = 0;
        render_height_ = 0;
        WVR_GetRenderTargetSize(&render_width_, &render_height_);

        if (s_is_vive_flow_) {
            render_width_ *= 0.7f;
            render_height_ *= 0.7f;
            lark::XRConfig::QuickConfigWithDefaulSetup(lark::QuickConfigLevel_Fast);
        }
        LOGI("render size width %d height %d", render_width_ * 2, render_height_);
        // setup render size.
         lark::XRConfig::render_width = render_width_ * 2;
         lark::XRConfig::render_height = render_height_;

        // frame rate
        lark::XRConfig::fps = 75;

        if (s_is_vive_flow_) {
            lark::XRConfig::fps = 60;
        }

        lark::XRConfig::headset_desc.type = larkHeadSetType_HTC;

#ifdef USE_RENDER_QUEUE
        lark::XRConfig::use_render_queue = true;
#else
        lark::XRConfig::use_render_queue = false;
#endif
    }

#ifdef ENABLE_CLOUDXR
    // create cloudxr before scene cloud inited
    cloudxr_client_ = std::make_shared<CloudXRClient>(this);
    cloudxr_client_->InitRenderParamsWithLarkXRConfig();
//    cloudxr_client_->Init();
    need_recreat_cloudxr_client_ = true;
#endif

    return true;
}

bool WaveApplication::InitGL() {
    LOGENTRY();
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
//    // 开启透明同道混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLenum glerr = glGetError();
    if (glerr != GL_NO_ERROR) {
        LOGE("glGetError() before initGL: %d", glerr);
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);

    glerr = glGetError();
    if (glerr != GL_NO_ERROR) {
        LOGE("glGetError() before initGL: %d", glerr);
    }

    // load assets.
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context, Assetlist);

    left_eye_q_ = WVR_ObtainTextureQueue(WVR_TextureTarget_2D, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, render_width_, render_height_, 0);
    for (int i = 0; i < WVR_GetTextureQueueLength(left_eye_q_); i++) {
        WvrFrameBuffer* fbo = new WvrFrameBuffer((int)(long)WVR_GetTexture(left_eye_q_, i).id, render_width_, render_height_,
                                                 true);
        if (fbo->has_error()) return false;
        left_eye_fbo_.push_back(fbo);
    }
    right_eye_q_ = WVR_ObtainTextureQueue(WVR_TextureTarget_2D, WVR_TextureFormat_RGBA, WVR_TextureType_UnsignedByte, render_width_, render_height_, 0);
    for (int i = 0; i < WVR_GetTextureQueueLength(right_eye_q_); i++) {
        WvrFrameBuffer* fbo = new WvrFrameBuffer((int)(long)WVR_GetTexture(right_eye_q_, i).id, render_width_, render_height_, true);
        if (fbo->has_error()) return false;
        right_eye_fbo_.push_back(fbo);
    }


    /// ADD SENCE HERE
    if (!scene_local_->InitGL(left_eye_q_, right_eye_q_, &left_eye_fbo_, &right_eye_fbo_))
        return false;
    scene_cloud_ = std::make_shared<WvrSceneCloud>();

#ifdef ENABLE_CLOUDXR
    scene_cloud_->SetCloudXRClient(cloudxr_client_);
#endif

    if (!scene_cloud_->InitGL(left_eye_q_, right_eye_q_, &left_eye_fbo_, &right_eye_fbo_))
        return false;

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);


#if defined(ENABLE_FOVEATED_RENDERING) && ENABLE_FOVEATED_RENDERING
    WVR_RenderFoveation(true);
#endif

#if defined(DISABLE_ADAPTIVE_QUALITY) && DISABLE_ADAPTIVE_QUALITY
    // 1. WaveVR AQ is enabled with WVR_QualityStrategy_SendQualityEvent by default from WaveVR SDK 3.2.0
    // 2. WVR_EnableAdaptiveQuality must be invoked after WVR_RenderInit().
    // 3. If you don't want to use AQ, you need to disable it.
    WVR_EnableAdaptiveQuality(false);

    /* There are some strategies in WaveVR AQ can to choose.
     *
     * EX: Use auto foveated rendering to improve performance while it is not good enough.
     * WVR_EnableAdaptiveQuality(true, WVR_QualityStrategy_AutoFoveation);
     *
     * EX: Receive recommended quailty chagned event and auto foveated rendering to improve performance while
     *     it is not good enough.
     * WVR_EnableAdaptiveQuality(true, WVR_QualityStrategy_SendQualityEvent | WVR_QualityStrategy_AutoFoveation);
     */
#endif
    return true;
}

void WaveApplication::InitJava() {
    // 初始化客户端接入凭证
    InitCertificate();

    // 初始化 cloudlark sdk
    xr_client_ = std::make_shared<lark::XRClient>();
    xr_client_->Init(Context::instance()->vm());
    xr_client_->RegisterObserver(this);

    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
        Navigation::ShowToast(xr_client_->last_error_message());
    }

    // 3dof controller system
    if (s_is_vive_flow_) {
        WVR_SetArmModel(WVR_SimulationType_ForceOn);
        WVR_SetArmSticky(true);
    }
}

void WaveApplication::ShutdownVR() {
    connected_ = false;

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }

    xr_client_->UnRegisterObserver();
    xr_client_->Release();
    xr_client_.reset();
    WVR_Quit();
}

void WaveApplication::ShutdownGL() {
    // reset all state.
    Input::ResetInput();
    Navigation::ClearToast();

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
    }
#endif

    scene_local_->ShutdownGL();
    scene_local_.reset();
    scene_cloud_->ShutdownGL();
    scene_cloud_.reset();

    if (left_eye_q_ != 0) {
        for (int i = 0; i < WVR_GetTextureQueueLength(left_eye_q_); i++) {
            delete left_eye_fbo_.at(i);
        }
        WVR_ReleaseTextureQueue(left_eye_q_);
    }
    if (right_eye_q_ != 0) {
        for (int i = 0; i < WVR_GetTextureQueueLength(right_eye_q_); i++) {
            delete right_eye_fbo_.at(i);
        }
        WVR_ReleaseTextureQueue(right_eye_q_);
    }
    lark::AssetLoader::Release();
}

bool WaveApplication::OnUpdate() {
    time_t now = time(nullptr);
    // update controler battery info every 5s;
    if (now - check_timestamp_ > 5) {
        int left = 0;
        WVR_BatteryStatus leftstatus=WVR_GetBatteryStatus(WVR_DeviceType_Controller_Left);
        if (leftstatus>0){
            left =wvr::GetBatteryPrecent(leftstatus);
        }

        int right =0;
        WVR_BatteryStatus rightstatus=WVR_GetBatteryStatus(WVR_DeviceType_Controller_Right);
        if (rightstatus>0){
            right =wvr::GetBatteryPrecent(rightstatus);
        }
        LOGV("battery level left %d right %d", left, right);
        lark::XRClient::SetControlerBatteryLevel(left, right);
        check_timestamp_ = now;
    }

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
        // update input.
        scene_cloud_->HandleInput();

        cxrFramesLatched latched;
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
            // update with local render
            scene_cloud_->Update();
            return false;
        }

        larkxrTrackingFrame trackingFrame = {};
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
                    return false;
                }
            }
        }

        scene_cloud_->Render(trackingFrame);

        cloudxr_client_->Release();
        cloudxr_client_->Stats();

        return false;
    }
#endif


    if (connected_) {
#ifdef USE_RENDER_QUEUE
        larkxrTrackingFrame trackingFrame;
        lark::XRVideoFrame xrVideoFrame(0);
        if (!xr_client_->Render(&trackingFrame, &xrVideoFrame)) {
            if (!xr_client_->media_ready()) {
                scene_cloud_->Update();
            } else {
                scene_cloud_->HandleInput();
                usleep(1000);
            }
        } else {
            scene_cloud_->HandleInput();
            scene_cloud_->Render(trackingFrame, xrVideoFrame);
            xr_client_->ReleaseRenderTexture();
        }
#else
        if (xr_client_->media_ready()) {
            scene_cloud_->HandleInput();
            if (xr_client_->HasNewFrame()) {
                larkxrTrackingFrame trackingFrame{};
                xr_client_->Render(&trackingFrame);
                scene_cloud_->Render(trackingFrame);
            } else {
                usleep(1000);
            }
        } else {
            scene_cloud_->Update();
        }
#endif
    } else {
        scene_local_->Update();
    }
    return false;
}

void WaveApplication::EnterAppliParams(const lark::EnterAppliParams &params) {
    LOGV("on enter EnterAppliParams");
    if (xr_client_) {
        xr_client_->EnterAppli(params);
//        xr_client_->EnterAppli("846813152229195776");
    }
}

void WaveApplication::CloseAppli() {
    if (xr_client_)
        xr_client_->Close();
}

void WaveApplication::OnConnected() {
    Application::OnConnected();
    LOGI("==========OnConnected============");
    connected_ = true;
    scene_cloud_->OnConnect();
}

void WaveApplication::OnError(int errCode, const char* msg) {
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
        scene_cloud_->OnClose();
        scene_local_->HomePage();
    }
    Navigation::ShowToast(msg);
}

void WaveApplication::OnClose(int code) {
    Application::OnClose(code);
    LOGV("=========on close %d", code);

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
        need_recreat_cloudxr_client_ = true;
    }
#endif

    connected_ = false;
    scene_cloud_->OnClose();
    scene_local_->HomePage();
}

// andoird lifecycle
void WaveApplication::OnCreate() {
    LOGV("*************OnCreate");
    if (xr_client_)
        xr_client_->OnCreated();
}

void WaveApplication::OnResume() {
    LOGV("*************OnResume");
    if (xr_client_)
        xr_client_->OnResume();
}

void WaveApplication::OnPause() {
    LOGV("*************OnPause");
    if (xr_client_)
        xr_client_->OnPause();
}

void WaveApplication::OnDestory() {
    LOGV("*************OnDestory");
    if (xr_client_)
        xr_client_->OnDestory();
}

void WaveApplication::OnMediaReady(int nativeTextrure) {
    scene_cloud_->OnMediaReady(nativeTextrure);
}

void WaveApplication::OnMediaReady() {
    scene_cloud_->OnMediaReady();
}

void WaveApplication::RequestTrackingInfo() {
    Application::RequestTrackingInfo();
    // send device pair
    larkxrTrackingDevicePairFrame devicePairFrame;
    scene_cloud_->UpdateAsync(&devicePairFrame);
    xr_client_->SendDevicePair(devicePairFrame);
}

void
WaveApplication::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration,
                                   float frequency) {
    Application::OnHapticsFeedback(isLeft, startTime, amplitude, duration, frequency);

    WVR_TriggerVibration(isLeft ? WVR_DeviceType_Controller_Left : WVR_DeviceType_Controller_Right,
        WVR_InputId_Alias1_Trigger, (uint32_t)(duration * 1000));
}

#ifdef ENABLE_CLOUDXR
void
WaveApplication::OnCloudXRReady(const char* appServerIp, const char* preferOutIp) {
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

void WaveApplication::UpdateClientState(cxrClientState state, cxrStateReason reason) {
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

void WaveApplication::ReceiveUserData(const void *data, uint32_t size) {

}

void WaveApplication::GetTrackingState(cxrVRTrackingState *state) {
    // send device pair
    larkxrTrackingDevicePairFrame devicePairFrame;
    scene_cloud_->UpdateAsync(&devicePairFrame);

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
}
#endif