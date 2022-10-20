//
// Created by fcx@pingxingyung.com on 2022/6/6.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKAR_CLOUDXR_CLIENT_H
#define LARKAR_CLOUDXR_CLIENT_H

#include "CloudXRClient.h"
#include "CloudXRInputEvents.h"
#include "CloudXRClientOptions.h"
#include "CloudXRMatrixHelpers.h"
#include "log.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <oboe/Oboe.h>
#include <lark_xr/types.h>
#include "object.h"

#define CLOUDXR_BUTTON_FLAG(input) (1ULL << input)

class CloudXRLaunchOptions : public CloudXR::ClientOptions {
public:
    float res_factor_;

    CloudXRLaunchOptions();
};

class CloudXRClientObserver {
public:
    virtual void UpdateClientState(cxrClientState state, cxrStateReason reason) = 0;
    virtual void ReceiveUserData(const void* data, uint32_t size) = 0;
    virtual void GetTrackingState(cxrVRTrackingState *state) = 0;
};

class CloudXRClient: public oboe::AudioStreamDataCallback, public lark::Object  {
public:
    static cxrVRTrackingState VRTrackingStateFrom(const larkxrTrackingDevicePairFrame& devicePairFrame);

    enum FrameMask {
        FrameMask_Left = cxrFrameMask_Left,
        FrameMask_Right = cxrFrameMask_Right,
        FrameMask_Special = cxrFrameMask_Special,
        FrameMask_All = cxrFrameMask_All,
    };

    CloudXRClient(CloudXRClientObserver* ob);
    ~CloudXRClient();

    // CloudXR interface callbacks
    void TriggerHaptic(const cxrHapticFeedback*) {}
    void GetTrackingState(cxrVRTrackingState* state);
    cxrBool RenderAudio(const cxrAudioFrame *audioFrame);
    void ReceiveUserData(const void* data, uint32_t size);
    void UpdateClientState(cxrClientState state, cxrStateReason reason);

    cxrDeviceDesc GetDeviceDesc();
    bool Init();
    cxrError Connect(const std::string& serverIp);
    void Teardown();
    void InitRenderParamsWithLarkXRConfig();
    void SetProjectionMatrix(int eye, const glm::mat4& projection);
    void SetProjectionMatrix(int eye, const larkxrRenderFov& larkxrFov);
    void SetFps(int fps);
    int DetermineOffset() const;

    cxrError Latch(cxrFramesLatched& framesLatched);
    bool Render(FrameMask eye);
    // object draw
    virtual void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view) override;
    virtual void DrawMultiview(const glm::mat4& projection, const glm::mat4& view) override;

    cxrError Release();

    void Stats();

    /// AudioStreamDataCallback interface
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames) override;

    bool HandleLaunchOptions(std::string &cmdline);

    // this is used to tell the client what the display/surface resolution is.
    // here, we can apply a factor to reduce what we tell the server our desired
    // video resolution should be.
    void SetStreamRes(uint32_t w, uint32_t h);
    // Send a touch event along to the server/host application
    void HandleTouch(float x, float y);

    inline bool IsRunning() const { return cloudxr_receiver_; };

    inline void SetArgs(const std::string &args) {
        launch_options_.ParseString(args);
    };

    inline std::string GetServerAddr() {
        return launch_options_.mServerIP;
    };

    inline const CloudXRLaunchOptions& GetLaunchOptions() {
        return launch_options_;
    };

    inline bool IsConnectStarted() {
        return client_state_ == cxrClientState_ConnectionAttemptInProgress || client_state_ == cxrClientState_StreamingSessionInProgress;
    }

    inline bool IsConnect() {
        return client_state_ == cxrClientState_StreamingSessionInProgress;
    };

    inline const cxrFramesLatched& frames_latched() {
        return frames_latched_;
    }
    inline bool latched() {
        return latched_;
    }
    inline void set_ipd(float ipd) { ipd_ = ipd; }
    inline void set_client_observer(CloudXRClientObserver* ob) { ob_ = ob; };
    inline bool inited() { return inited_; }
private:
    static constexpr int kQueueLen = 16;

    void SetPoseMatrix(const glm::mat4& pose_mat);

    cxrReceiverHandle cloudxr_receiver_ = nullptr;

    CloudXRLaunchOptions launch_options_;

    uint32_t stream_width_ = 1920;
    uint32_t stream_height_ = 1080;

    cxrFramesLatched frames_latched_ = {};
    bool latched_ = false;

    std::mutex state_mutex_;
    cxrMatrix34 pose_matrix_[kQueueLen] = {};
    cxrDeviceDesc device_desc_ = {};
    int current_idx_ = 0;

    float ipd_ = 0.064f;
    int fps_ = 72;
    float pred_offset_ = 0.0f;

    cxrConnectionStats stats_ = {};
    int frames_until_stats_ = 60;

    cxrClientState client_state_ = cxrClientState_ReadyToConnect;

    CloudXRClientObserver* ob_;

    std::shared_ptr<oboe::AudioStream> recording_stream_ = nullptr;
    std::shared_ptr<oboe::AudioStream> playback_stream_ = nullptr;

    bool inited_ = false;
};


#endif //LARKAR_CLOUDXR_CLIENT_H
