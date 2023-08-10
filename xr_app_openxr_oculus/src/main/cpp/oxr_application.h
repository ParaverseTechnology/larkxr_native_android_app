//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_OXR_APPLICATION_H
#define LARKXR_OXR_APPLICATION_H

#include <android_native_app_glue.h>
#ifdef ENABLE_CLOUDXR
#include <cloudxr_client.h>
#endif
#include "utils.h"
#include "xr_scene_local.h"
#include "xr_scene_cloud.h"
#include "lark_xr/xr_client.h"
#include "application.h"
#include "openxr_context.h"

#define TEST_ENTER_APPLI 1

namespace oxr {
class OxrApplication: public Application
#ifdef ENABLE_CLOUDXR
        ,public CloudXRClientObserver
#endif
{
public:
    OxrApplication();
    ~OxrApplication();

    void OnResume();
    void OnPause();

    bool InitGL(OpenxrContext* context);
    virtual void ShutdownGL() override;

    void Update();
    void RenderFrame();

#ifdef TEST_ENTER_APPLI
    virtual void EnterAppli(const std::string& appId);
    // 可选区域id等参数进入应用
    virtual void EnterAppliParams(const lark::EnterAppliParams& params);
#endif

    // call by ui
    // ui 设置帧率
    virtual void SetupFPS(int fps) override;
    // ui 设置云渲染使用的空间
    // app space -> 使用app设置的地面高度和位置原点
    // local space -> 使用固定的地面高度和初始位置原点
    virtual void SetupSapce(Space space) override;
    // ui 设置天空盒, 更新场景中的天空盒
    virtual void SetupSkyBox(int index) override;

    //
    // xr client callback
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void OnError(int errCode, const char* msg) override;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override;
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void OnMediaReady() override;
    virtual void RequestTrackingInfo() override;
    virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override;

#ifdef ENABLE_CLOUDXR
    virtual void OnCloudXRReady(const char* appServerIp, const char* preferOutIp) override;

    // cloudxr callback
    virtual void UpdateClientState(cxrClientState state, cxrStateReason reason) override;
    virtual void ReceiveUserData(const void* data, uint32_t size) override;
    virtual void GetTrackingState(cxrVRTrackingState *state) override;
#endif
    // handle network change
    virtual void OnNetworkAvailable() override;
    virtual void OnNetworkLost() override;
private:
    bool RenderLayer(XrTime predictedDisplayTime,
                     std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer,
                     const larkxrTrackingFrame& trackingFrame, bool hasNewFrame);

    bool GetViewTransform(const XrSpace& space,
                                 const XrTime& predictedDisplayTime,
                                 XrPosef *viewTransform,
                                 int viewTransformCount,
                                 XrPosef* xfStageFromHead);

    bool UpdateCloudTrackingState(larkxrTrackingDevicePairFrame& trackingDevicePairFrame);

    inline XrSpace GetSelectedXRSpace() { return current_cloud_space_ == Space_Local ? context_->local_space() : context_->app_space(); }

    OpenxrContext* context_ = nullptr;

    // local ui
    std::shared_ptr<XrSceneLocal> scene_local_ = {};

    // cloud render
    std::shared_ptr<XrSceneCloud> scene_cloud_ = {};

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

    bool config_inited_ = false;

    Space current_cloud_space_ = Space_Local;
};
}

#endif //LARKXR_OXR_APPLICATION_H
