//
// Created by fcx on 2020/5/6.
//

#ifndef CLOUDLARKXR_WAVE_APPLICATION_H
#define CLOUDLARKXR_WAVE_APPLICATION_H

#include "application.h"
#include "wvr_scene_local.h"
#include "wvr_scene_cloud.h"
#ifdef ENABLE_CLOUDXR
#include <cloudxr_client.h>
#endif

class WaveApplication: public Application
#ifdef ENABLE_CLOUDXR
        ,public CloudXRClientObserver
#endif
{
public:
    static void set_is_vive_flow(bool isFlow);
    static bool s_is_vive_flow();

    WaveApplication();
    ~WaveApplication();

    // andoird lifecycle
    void OnCreate();
    void OnResume();
    void OnPause();
    void OnDestory();
    //
    virtual bool InitVR(android_app* app) override { return false; };
    virtual bool InitVR() override;
    virtual bool InitGL() override;
    virtual void InitJava() override;
    virtual void ShutdownVR() override;
    virtual void ShutdownGL() override;

    //  callback from android native activity.
    virtual void HandleVrModeChange() override {};
    virtual bool OnUpdate() override;
    // 可选区域id等参数进入应用
    virtual void EnterAppliParams(const lark::EnterAppliParams& params) override;

    virtual void CloseAppli() override;
    // xr client callback
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void OnError(int errCode, const char* msg) override;
    // soft decoder
    virtual void OnMediaReady() override;
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override;
    virtual void OnTrackingFrame(std::unique_ptr<lark::XRTrackingFrame>&& frame) override {};
    virtual void OnStreamingDisconnect() override {};
    // hw decoder callback without textrue.
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override {};
    //
    virtual void RequestTrackingInfo() override;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override;

#ifdef ENABLE_CLOUDXR
    virtual void OnCloudXRReady(const char* appServerIp, const char* preferOutIp) override;
#endif

#ifdef ENABLE_CLOUDXR
    // cloudxr callback
    virtual void UpdateClientState(cxrClientState state, cxrStateReason reason) override;
    virtual void ReceiveUserData(const void* data, uint32_t size) override;
    virtual void GetTrackingState(cxrVRTrackingState *state) override;
#endif
private:
    std::shared_ptr<WvrSceneLocal> scene_local_ = {nullptr};
    std::shared_ptr<WvrSceneCloud> scene_cloud_ = {nullptr};

    time_t check_timestamp_ = {};

    // render buffer
    uint32_t render_width_ = 0;
    uint32_t render_height_ = 0;
    void* left_eye_q_ = nullptr;
    void* right_eye_q_ = nullptr;
    std::vector<WvrFrameBuffer*> left_eye_fbo_{};
    std::vector<WvrFrameBuffer*> right_eye_fbo_{};

#ifdef ENABLE_CLOUDXR
    static const int MAXIMUM_TRACKING_FRAMES = 50;
    typedef std::map<uint64_t, larkxrTrackingFrame> TRACKING_FRAME_MAP;
    TRACKING_FRAME_MAP tracking_frame_map_{};
    std::mutex tracking_frame_mutex_{};

    uint64_t pre_controller_state[2] = {};
    std::shared_ptr<CloudXRClient> cloudxr_client_ = nullptr;
    std::string prepare_public_ip_ = "";
    bool need_reconnect_public_ip_ = false;
    bool need_recreat_cloudxr_client_ = false;
#endif

    static bool s_is_vive_flow_;
};
#endif //CLOUDLARKXR_WAVE_APPLICATION_H
