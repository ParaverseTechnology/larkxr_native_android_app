//
// Created by fcx on 2020/5/6.
//
#include <GLES3/gl31.h>
#include <EGL/egl.h>
#include "aio_application.h"
#include "env_context.h"
//#include "xr_config.h"
#include <unistd.h>
#include "math.h"
#include <jni.h>
#include <utils.h>
#include <asset_files.h>
#include <build_config.h>
#include <lark_xr/xr_latency_collector.h>
#include "log.h"
#include "xms_scene.h"

#define LOG_TAG "AioApplication"

AioApplication::AioApplication() {
    RegiseredInstance(this);
}
AioApplication::~AioApplication() {
    UnRegiseredInstance();
}

void AioApplication::EnterAppli(const std::string &appId) {
    LOGI("==========EnterAppli %s", appId.c_str());
    xr_client_->EnterAppli(appId);
}

void AioApplication::CloseAppli() {
    xr_client_->Close();
}

void AioApplication::OnConnected() {
    LOGI("==========OnConnected============");
    connected_ = true;
}

void AioApplication::OnError(int errCode, const std::string &msg) {
     LOGE("---------on xr client error %d; msg %s;", errCode, msg.c_str());
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

void AioApplication::OnClose(int code) {
   LOGI("=========on close %d", code);
    connected_ = false;
    scene_cloud_->OnClose();
    scene_local_->HomePage();
    switch(code) {
        case LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED:
            Navigation::ShowToast("与服务器媒体传输通道连接关闭");
            break;
        case LK_RENDER_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器 TCP 连接关闭");
            break;
        case LK_PROXY_SERVER_CLOSE:
            Navigation::ShowToast("与渲染服务器代理连接关闭");
            break;
    }
}

// andoird lifecycle
void AioApplication::OnCreate() {
   LOGI("*************OnCreate");
   if (xr_client_) {
       xr_client_->OnCreated();
   }
}

void AioApplication::OnResume() {
    LOGV("*************OnResume");
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void AioApplication::OnPause() {
   LOGI("*************OnPause");
   if (xr_client_) {
       xr_client_->OnPause();
   }
}

void AioApplication::OnDestory() {
    LOGV("*************OnDestory");
    if (xr_client_) {
        xr_client_->OnDestory();
    }
}

void AioApplication::OnMediaReady(int nativeTextrureLeft,int nativeTextrureRight) {
    LOGI("*************OnMediaReady %d %d", nativeTextrureLeft, nativeTextrureRight);
    if (scene_cloud_) {
        scene_cloud_->OnMediaReady(nativeTextrureLeft, nativeTextrureRight);
    }
}

void AioApplication::RequestTrackingInfo() {
    Application::RequestTrackingInfo();

    //Application::RequestTrackingInfo();
    // send device pair
   // larkxrTrackingDevicePairFrame devicePairFrame;
    updateAsync();
    xr_client_->SendDevicePair(mTrackingState);
}


// ------------------------------------//
// ------- aio get tracking state  ----//
// ------------------------------------//

void AioApplication::updateAsync(){
//    float predictedTimeMs = svrGetPredictedDisplayTime();
//    mPoseState = svrGetPredictedHeadPose(predictedTimeMs);
    mPoseState = svrGetPredictedHeadPose(0);
    mTrackingState.devicePair.hmdPose.timestamp = mPoseState.poseTimeStampNs;
    mTrackingState.devicePair.hmdPose.poseFetchTime = mPoseState.poseFetchTimeNs;
    mTrackingState.devicePair.hmdPose.expectedDisplayTime =mPoseState.expectedDisplayTimeNs;

    mTrackingState.devicePair.hmdPose.device = Larkxr_Device_Type_HMD;

    mTrackingState.frameIndex = 0;
    mTrackingState.fetchTime = 0;
    mTrackingState.displayTime =0;

    glm::vec3 position;
    glm::quat rotation;

    rotation.x = -mPoseState.pose.rotation.x;
    rotation.y = -mPoseState.pose.rotation.y;
    rotation.z = -mPoseState.pose.rotation.z;
    rotation.w = mPoseState.pose.rotation.w;

    position.x = -mPoseState.pose.position.x;
    /// TODO get height
    position.y = -mPoseState.pose.position.y + 1.5f;
    position.z = -mPoseState.pose.position.z;

    mTrackingState.devicePair.hmdPose.isConnected = true;
    mTrackingState.devicePair.hmdPose.is6Dof = true;
    mTrackingState.devicePair.hmdPose.isValidPose = true;
    mTrackingState.devicePair.hmdPose.rotation = rotation;
    mTrackingState.devicePair.hmdPose.position = position;
}

void AioApplication::Initialize() {
    SvrSetMaxPredictTime(500);
    SvrSetCustomConfig(1);

    SvrApplication::Initialize();

    LOGV("tracking mode %d %d", svrGetTrackingMode(), svrGetSupportedTrackingModes());

    mAppContext.cpuPerfLevel = kPerfMaximum;
    mAppContext.gpuPerfLevel = kPerfMaximum;

    SvrSetMaxPredictTime(100);

    // load resource
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context, Assetlist);

    {
        lark::XRConfig::use_multiview = false;
        lark::XRConfig::fps = 60;
        lark::XRConfig::flip_draw = true;
        lark::XRConfig::render_width = 2048;
        lark::XRConfig::render_height = 1024;
        lark::XRConfig::bitrate = 20 * 1000;
        lark::XRConfig::stream_type = larkStreamType_UDP;
        lark::XRConfig::use_render_queue = false;
        lark::XRConfig::foveated_rendering.enableFoveateRendering = true;

        lark::XRConfig::fov[0] = {101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F};
        lark::XRConfig::fov[1] = {101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F, 101.0F * 0.5F};
        // 初始化 cloudlark sdk
        xr_client_ = std::make_shared<lark::XRClient>();
        xr_client_->Init(Context::instance()->vm());
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

        // fot test
//        std::string ip = "192.168.31.15";
//        Application::SetServerAddr(ip, 8181);
        // 切水果
//        EnterAppli("854773101450231808");
    }

    LOGI("inint gl w %d h %d; pw %d ph %d", mAppContext.targetEyeWidth, mAppContext.targetEyeHeight,
            mAppContext.physicalWidth, mAppContext.physicalHeight);

    // defautl use hmd for input
    Input::SetCurrentRayCastType(Input::RayCast_Hmd);

    scene_local_ = std::make_shared<XmsSceneLocal>();
    scene_local_->InitGl(&mAppContext);
    scene_local_->set_input(&mInput);

    scene_cloud_ = std::make_shared<XmsSceneCloud>();
    scene_cloud_->InitGl(&mAppContext);
    scene_cloud_->set_input(&mInput);
}

void AioApplication::Update() {
    SvrApplication::Update();
}

void AioApplication::Render() {
    if (connected_) {
        updateAsync();
        scene_cloud_->UpdateHMDPose(mPoseState);
        if (xr_client_->media_ready()) {
            larkxrTrackingFrame trackingFrame;
            if (xr_client_->Render(&trackingFrame)) {
                // has new frame
                scene_cloud_->Submit(trackingFrame);
            } else {
                // skip submit.
                usleep(1);
            }
        } else {
            for (int i = 0; i < 2; i ++) {
                scene_cloud_->Draw(i);
            }
            scene_cloud_->Submit();
        }
    } else{
        updateAsync();
        scene_local_->UpdateHMDPose(mPoseState);
        for (int i = 0; i < 2; i ++) {
            scene_local_->Draw(i);
        }
        scene_local_->Submit();
    }
    // update key state after use.
    GetInput().Update();
}

void AioApplication::Shutdown() {
    LOGI("on shutdown start");
    scene_local_.reset();
    scene_cloud_.reset();
    Input::ResetInput();
    Navigation::ClearToast();
    lark::AssetLoader::Release();
    if (xr_client_) {
        xr_client_->OnDestory();
        xr_client_->UnRegisterObserver();
        xr_client_->Release();
        xr_client_.reset();
    }
    SvrApplication::Shutdown();
    LOGI("on shutdonwn finish");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_pxy_xr_1app_1xms_SvrNativeActivity_init(JNIEnv *env, jobject thiz, jobject am,
                                                 jstring internal_data_path,
                                                 jstring external_data_path) {
    const char *internalDataPath = env->GetStringUTFChars(internal_data_path, 0);
    const char *externalDataPath = env->GetStringUTFChars(external_data_path, 0);

    LOGI("MainActivity_init: call  Context::getInstance()->init");
    JavaVM* vm = nullptr;
    env->GetJavaVM(&vm);
    Context::Init(vm, env, am, thiz, internalDataPath, externalDataPath);

    env->ReleaseStringUTFChars(internal_data_path, internalDataPath);
    env->ReleaseStringUTFChars(internal_data_path, externalDataPath);

    BuildConfig::init(env, "com/pxy/xr_app_xms/BuildConfig");
}