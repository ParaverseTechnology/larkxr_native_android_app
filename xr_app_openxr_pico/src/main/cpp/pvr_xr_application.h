//
// Created by Administrator on 2021/7/21.
//

#ifndef CLOUDLARKXR_PVR_XR_APPLICATION_H
#define CLOUDLARKXR_PVR_XR_APPLICATION_H

#include <glm/detail/type_quat.hpp>
#include "glm/glm.hpp"
#include <application.h>
#ifdef ENABLE_CLOUDXR
#include <cloudxr_client.h>
#endif
#include "pvr_xr_scene_local.h"
#include "pvr_xr_scene_cloud.h"
#include "openxr_context.h"

class PvrXrApplication: public Application
#ifdef ENABLE_CLOUDXR
        ,public CloudXRClientObserver
#endif
{
public:
    PvrXrApplication();
    ~PvrXrApplication();

    void OnResume();
    void OnPause();

    bool InitGL(OpenxrContext* context);
    virtual void ShutdownGL() override;

    void Update();
    void RenderFrame();

    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    // 可选区域id等参数进入应用
    virtual void EnterAppliParams(const lark::EnterAppliParams& params) override;
    virtual void CloseAppli() override;

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
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override;
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override;
    virtual void OnMediaReady(int nativeTextureLeft, int nativeTextureRight) override;
    virtual void OnMediaReady() override;
    virtual void RequestTrackingInfo() override;
    virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override;
    virtual void OnDataChannelOpen() override;
#ifdef ENABLE_CLOUDXR
    virtual void OnCloudXRReady(const std::string& appServerIp, const std::string& preferOutIp) override;
#endif

#ifdef ENABLE_CLOUDXR
    // cloudxr callback
    virtual void UpdateClientState(cxrClientState state, cxrStateReason reason) override;
    virtual void ReceiveUserData(const void* data, uint32_t size) override;
    virtual void GetTrackingState(cxrVRTrackingState *state) override;
#endif
private:
    bool RenderLayer(XrTime predictedDisplayTime, std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer, const larkxrTrackingFrame& trackingFrame, bool hasNewFrame);

    bool GetViewTransform(const XrSpace& space,
                          const XrTime& predictedDisplayTime,
                          XrPosef *viewTransform,
                          int viewTransformCount,
                          XrPosef* xfStageFromHead);

    inline XrSpace GetSelectedXRSpace() { return current_cloud_space_ == Space_Local ? context_->local_space() : context_->app_space(); }

    std::shared_ptr<PvrXRSceneLocal> scene_local_ = nullptr;
    std::shared_ptr<PvrXRSceneCloud> scene_cloud_ = nullptr;
    OpenxrContext* context_ = nullptr;

    // PICO 2.2.0
    // https://developer-cn.pico-interactive.com/document/native/release-notes/
    // XrFrameEndInfoEXT xr_frame_end_info_ext_ = {};

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


#endif //CLOUDLARKXR_PVR_XR_APPLICATION_H
