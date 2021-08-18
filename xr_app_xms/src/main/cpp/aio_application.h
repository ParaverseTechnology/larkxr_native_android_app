//
// Created by fcx on 2020/5/6.
//

#ifndef CLOUDLARKXR_AIO_APPLICATION_H
#define CLOUDLARKXR_AIO_APPLICATION_H

#include <lark_xr/types.h>
#include <lark_xr/xr_tracking_frame.h>
#include <svrApi.h>
#include <svrApplication.h>
#include "application.h"
#include "lark_xr/xr_config.h"
#include "xms_scene_local.h"
#include "xms_scene_cloud.h"

class AioApplication: public Application, public Svr::SvrApplication {
public:
    AioApplication();
    ~AioApplication();

    virtual void LoadConfiguration() override {};

    virtual void Initialize() override;

    virtual void Update() override;

    virtual void Render() override;

    virtual void Shutdown() override;

    // andoird lifecycle
    void OnResume();
    void OnPause();
    void OnCreate();
    void OnDestory();

    virtual bool InitVR() override { return true; };
    virtual bool InitVR(android_app* app) override { return true; };
    virtual bool InitGL() override { return true; };
    virtual void InitJava() override {};
    virtual void ShutdownVR() override {};
    virtual void ShutdownGL() override {};

    //  callback from android native activity.
    virtual void HandleVrModeChange() override{};
    virtual bool OnUpdate() override {};
    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    virtual void CloseAppli() override;

    // xr client callback
    virtual void OnConnected() override;

    virtual void OnClose(int code) override;
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override{};
    // hw decoder callback textrue.
   // virtual void OnMediaReady(int nativeTextrure) override{};
    virtual void OnMediaReady(int nativeTextrureLefe,int nativeTextrureRight) override;

    virtual void RequestTrackingInfo() override;
private:
    void updateAsync();

    svrHeadPoseState mPoseState;
    larkxrTrackingDevicePairFrame mTrackingState;

    std::shared_ptr<XmsSceneLocal> scene_local_;
    std::shared_ptr<XmsSceneCloud> scene_cloud_;
};
#endif //CLOUDLARKXR_AIO_APPLICATION_H
