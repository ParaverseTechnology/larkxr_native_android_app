//
// Created by Administrator on 2021/7/21.
//

#ifndef CLOUDLARKXR_PVR_XR_APPLICATION_H
#define CLOUDLARKXR_PVR_XR_APPLICATION_H

#include <glm/detail/type_quat.hpp>
#include "glm/glm.hpp"
#include <application.h>
#include "pvr_xr_scene_local.h"
#include "pvr_xr_scene_cloud.h"
#include "openxr_context.h"

class PvrXrApplication: public Application {
public:
    PvrXrApplication();
    ~PvrXrApplication();

    bool InitGL(OpenxrContext* context);
    virtual void ShutdownGL() override;

    void Update();
    void RenderFrame();

    // 进入应用
    virtual void EnterAppli(const std::string& appId) override;
    virtual void CloseAppli() override;

    // unused
    virtual bool InitGL() override { return false; };
    virtual bool InitVR(android_app* app) override { return false; };
    virtual bool InitVR() override { return false; };
    virtual void InitJava() override {};
    virtual void ShutdownVR() override {};
    virtual void HandleVrModeChange() override {};
    virtual bool OnUpdate() override { return false; };
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
private:
    bool RenderLayer(XrTime predictedDisplayTime, std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
                     XrCompositionLayerProjection& layer, const larkxrTrackingFrame& trackingFrame);

    std::shared_ptr<PvrXRSceneLocal> scene_local_ = nullptr;
    std::shared_ptr<PvrXRSceneCloud> scene_cloud_ = nullptr;
    OpenxrContext* context_ = nullptr;
    XrFrameEndInfoEXT xr_frame_end_info_ext_ = {};
    int gs_index_ = 0;
    XrViewStatePICOEXT view_state_pico_ = {};
    XrPosef hmd_view_pose[2] = {};
};


#endif //CLOUDLARKXR_PVR_XR_APPLICATION_H
