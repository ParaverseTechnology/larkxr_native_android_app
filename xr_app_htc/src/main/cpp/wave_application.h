//
// Created by fcx on 2020/5/6.
//

#ifndef CLOUDLARKXR_WAVE_APPLICATION_H
#define CLOUDLARKXR_WAVE_APPLICATION_H

#include "application.h"
#include "wvr_scene_local.h"
#include "wvr_scene_cloud.h"

class WaveApplication: public Application {
public:
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
    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    virtual void CloseAppli() override;
    // xr client callback
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void OnError(int errCode, const std::string& msg) override;
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
};
#endif //CLOUDLARKXR_WAVE_APPLICATION_H
