//
// Created by fcx@pingxingyung.com on 2022/6/6.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include "cloudxr_client.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <lark_xr/xr_config.h>

#define LOG_TAG "cloudxr_client"

CloudXRLaunchOptions::CloudXRLaunchOptions() :
        ClientOptions(),
        // default to 0.75 reduced size, as many devices can't handle full throughput.
        // 0.75 chosen as WAR value for steamvr buffer-odd-size bug, works on galaxytab s6 + pixel 2
        res_factor_(1.0f)
{
    AddOption("res-factor", "rf", true, "Adjust client resolution sent to server, reducing res by factor. Range [0.5-1.0].",
              HANDLER_LAMBDA_FN
                      {
                          float factor = std::stof(tok);
                          if (factor >= 0.5f && factor <= 1.0f)
                          res_factor_ = factor;
                          LOGI("Resolution factor = %0.2f", res_factor_);
                          return ParseStatus_Success;
                      });
};


CloudXRClient::CloudXRClient(CloudXRClientObserver* ob): ob_(ob) {
};

CloudXRClient::~CloudXRClient() {
    Teardown();
}

void CloudXRClient::GetTrackingState(cxrVRTrackingState *state) {
    if (ob_) {
        ob_->GetTrackingState(state);
    }
}

cxrBool CloudXRClient::RenderAudio(const cxrAudioFrame *audioFrame) {
    LOGI("RenderAudio %ld %p", audioFrame->streamSizeBytes, playback_stream_.get());

    if (!playback_stream_)
    {
        return cxrFalse;
    }

    const uint32_t timeout = audioFrame->streamSizeBytes / CXR_AUDIO_BYTES_PER_MS;
    const uint32_t numFrames = timeout * CXR_AUDIO_SAMPLING_RATE / 1000;
    auto result = playback_stream_->write(audioFrame->streamBuffer, numFrames,
                            timeout * oboe::kNanosPerMillisecond);

    LOGI("RenderAudio write result %d %d", result.value(), result.error());

    return cxrTrue;
}

void CloudXRClient::ReceiveUserData(const void* data, uint32_t size) {
    LOGI("ReceiveUserData");
    if (ob_) {
        ob_->ReceiveUserData(data, size);
    }
}

void CloudXRClient::UpdateClientState(cxrClientState state, cxrStateReason reason) {
    LOGI("UpdateClientState state %d; reason %d;", state, reason);
    client_state_ = state;
    if (ob_) {
        ob_->UpdateClientState(state, reason);
    }
}

cxrDeviceDesc CloudXRClient::GetDeviceDesc() {
    device_desc_.deliveryType = cxrDeliveryType_Stereo_RGB;
    device_desc_.width = stream_width_;
    device_desc_.height = stream_height_;
    device_desc_.maxResFactor = 1.0f; // leave alone, don't extra oversample on server.
    device_desc_.fps = static_cast<float>(fps_);
    device_desc_.ipd = ipd_;
    device_desc_.predOffset = pred_offset_;
    device_desc_.receiveAudio = true;
    // TODO Add send audio
    device_desc_.sendAudio = false;
    device_desc_.posePollFreq = static_cast<float>(fps_) * 2;
//    device_desc_.receiveAudio = launch_options_.mReceiveAudio;
//    device_desc_.sendAudio = launch_options_.mSendAudio;

    device_desc_.disablePosePrediction = true;
    device_desc_.angularVelocityInDeviceSpace = true;
    device_desc_.foveatedScaleFactor = 0;
//    device_desc_.ctrlType = cxrControllerType_ValveIndex;
    // TODO SETUP controller type
    device_desc_.ctrlType = cxrControllerType_OculusTouch;

    float scale = 1.5f * 0.5f;

    // cxrChaperone chap;
    device_desc_.chaperone.universe = cxrUniverseOrigin_Standing;
    device_desc_.chaperone.origin.m[0][0] = 1.f;
    device_desc_.chaperone.origin.m[0][1] = 0.f;
    device_desc_.chaperone.origin.m[0][2] = 0.f;
    device_desc_.chaperone.origin.m[0][3] = 0.f;
    device_desc_.chaperone.origin.m[1][0] = 0.f;
    device_desc_.chaperone.origin.m[1][1] = 1.f;
    device_desc_.chaperone.origin.m[1][2] = 0.f;
    device_desc_.chaperone.origin.m[1][3] = 0.f;
    device_desc_.chaperone.origin.m[2][0] = 0.f;
    device_desc_.chaperone.origin.m[2][1] = 0.f;
    device_desc_.chaperone.origin.m[2][2] = 1.f;
    device_desc_.chaperone.origin.m[2][3] = 0.f;
    device_desc_.chaperone.playArea.v[0] = 2.f * scale;
    device_desc_.chaperone.playArea.v[1] = 2.f * scale;
    LOGV("Setting play area to %0.2f x %0.2f", device_desc_.chaperone.playArea.v[0], device_desc_.chaperone.playArea.v[1]);
    return device_desc_;
}


bool CloudXRClient::Init() {
    if (cloudxr_receiver_) {
        LOGI("receiver already exits. delete old.");
        Teardown();
    }

    LOGI("create receviver");

    cxrGraphicsContext context{cxrGraphicsContext_GLES};
    context.egl.display = eglGetCurrentDisplay();
    context.egl.context = eglGetCurrentContext();
    auto device_desc = GetDeviceDesc();

    cxrClientCallbacks clientProxy = { nullptr };
    clientProxy.GetTrackingState = [](void* context, cxrVRTrackingState* trackingState)
    {
        reinterpret_cast<CloudXRClient*>(context)->GetTrackingState(trackingState);
    };
    clientProxy.TriggerHaptic = [](void* context, const cxrHapticFeedback* haptic)
    {
        reinterpret_cast<CloudXRClient*>(context)->TriggerHaptic(haptic);
    };
    clientProxy.RenderAudio = [](void* context, const cxrAudioFrame *audioFrame)
    {
        return reinterpret_cast<CloudXRClient*>(context)->RenderAudio(audioFrame);
    };
    clientProxy.ReceiveUserData = [](void* context, const void* data, uint32_t size)
    {
        reinterpret_cast<CloudXRClient*>(context)->ReceiveUserData(data, size);
    };
    clientProxy.UpdateClientState = [](void* context, cxrClientState state, cxrStateReason reason){
        reinterpret_cast<CloudXRClient*>(context)->UpdateClientState(state, reason);
    };

    if (device_desc.receiveAudio) {
        // Initialize audio playback
        oboe::AudioStreamBuilder playbackStreamBuilder;
        playbackStreamBuilder.setDirection(oboe::Direction::Output);
        playbackStreamBuilder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
        playbackStreamBuilder.setSharingMode(oboe::SharingMode::Exclusive);
        playbackStreamBuilder.setFormat(oboe::AudioFormat::I16);
        playbackStreamBuilder.setChannelCount(oboe::ChannelCount::Stereo);
        playbackStreamBuilder.setSampleRate(CXR_AUDIO_SAMPLING_RATE);

        oboe::Result r = playbackStreamBuilder.openStream(playback_stream_);
        if (r != oboe::Result::OK) {
            LOGE("Failed to open playback stream. Error: %s", oboe::convertToText(r));
            return cxrError_Failed;
        }

        int bufferSizeFrames = playback_stream_->getFramesPerBurst() * 2;
        r = playback_stream_->setBufferSizeInFrames(bufferSizeFrames);
        if (r != oboe::Result::OK) {
            LOGE("Failed to set playback stream buffer size to: %d. Error: %s",
                 bufferSizeFrames, oboe::convertToText(r));
            return cxrError_Failed;
        }

        r = playback_stream_->start();
        if (r != oboe::Result::OK) {
            LOGE("Failed to start playback stream. Error: %s", oboe::convertToText(r));
            return cxrError_Failed;
        }
    }

    if (device_desc.sendAudio) {
        // Initialize audio recording
        oboe::AudioStreamBuilder recordingStreamBuilder;
        recordingStreamBuilder.setDirection(oboe::Direction::Input);
        recordingStreamBuilder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
        recordingStreamBuilder.setSharingMode(oboe::SharingMode::Exclusive);
        recordingStreamBuilder.setFormat(oboe::AudioFormat::I16);
        recordingStreamBuilder.setChannelCount(oboe::ChannelCount::Stereo);
        recordingStreamBuilder.setSampleRate(CXR_AUDIO_SAMPLING_RATE);
        recordingStreamBuilder.setInputPreset(oboe::InputPreset::VoiceCommunication);
        recordingStreamBuilder.setDataCallback(this);

        oboe::Result r = recordingStreamBuilder.openStream(recording_stream_);
        if (r != oboe::Result::OK) {
            LOGE("Failed to open recording stream. Error: %s", oboe::convertToText(r));
            return cxrError_Failed;
        }

        r = recording_stream_->start();
        if (r != oboe::Result::OK) {
            LOGE("Failed to start recording stream. Error: %s", oboe::convertToText(r));
            return cxrError_Failed;
        }
    }


    LOGI("Audio support: receive [%s], send [%s]", device_desc.receiveAudio?"on":"off", device_desc.sendAudio?"on":"off");
    LOGI("device des w %d h %d fps %f ipd %f", device_desc.width, device_desc.height, device_desc.fps, device_desc.ipd);
    LOGI("device proj %f %f %f %f; %f %f %f %f", device_desc.proj[0][0], device_desc.proj[0][1], device_desc.proj[0][2], device_desc.proj[0][3],
         device_desc.proj[1][0], device_desc.proj[1][1], device_desc.proj[1][2], device_desc.proj[1][3]);

    cxrReceiverDesc desc = { 0 };
    desc.requestedVersion = CLOUDXR_VERSION_DWORD;
    desc.deviceDesc = device_desc;
    desc.clientCallbacks = clientProxy;
    desc.clientContext = this;
    desc.shareContext = &context;
    desc.numStreams = CXR_NUM_VIDEO_STREAMS_XR;
    desc.receiverMode = cxrStreamingMode_XR;
//    desc.debugFlags = cxrDebugFlags_EnableAImageReaderDecoder | cxrDebugFlags_OutputLinearRGBColor;
//    desc.debugFlags = cxrDebugFlags_EnableSXRDecoder | cxrDebugFlags_OutputLinearRGBColor;
    desc.debugFlags = cxrDebugFlags_OutputLinearRGBColor;
//    desc.debugFlags = 0;
    desc.logMaxSizeKB = CLOUDXR_LOG_MAX_DEFAULT;
    desc.logMaxAgeDays = CLOUDXR_LOG_MAX_DEFAULT;
    cxrError err = cxrCreateReceiver(&desc, &cloudxr_receiver_);
    if (err != cxrError_Success)
    {
        LOGE("Failed to create CloudXR receiver. Error %d, %s. egl display %p egl context %p", err, cxrErrorString(err),
             context.egl.display, context.egl.context);
        return err;
    }
    LOGI("create cloudxr receive success; egl display %p egl context %p", context.egl.display, context.egl.context);
    inited_ = true;
    return true;
}

cxrError CloudXRClient::Connect(const std::string& serverIp) {
    if (client_state_ == cxrClientState_ConnectionAttemptInProgress)
        return cxrError_Success; // already connected, no error? TODO

    LOGI("Connecting to CloudXR at %s...", serverIp.c_str());

    cxrConnectionDesc connectionDesc = {};
    connectionDesc.async = cxrTrue;
    connectionDesc.maxVideoBitrateKbps = 100 * 1000;
    connectionDesc.clientNetwork = cxrNetworkInterface_WiFi5Ghz;
//    connectionDesc.clientNetwork = cxrNetworkInterface_Unknown;
    connectionDesc.topology = cxrNetworkTopology_WAN;
//    connectionDesc.topology = cxrNetworkTopology_Unknown;
    cxrError err = cxrConnect(cloudxr_receiver_, serverIp.c_str(), &connectionDesc);
    if (err != cxrError_Success)
    {
        LOGE("Failed to connect to CloudXR server at %s. Error %d, %s. receiver %p", serverIp.c_str(), (int)err, cxrErrorString(err), cloudxr_receiver_);
        Teardown();
        return err;
    }

    // else, good to go.
    LOGI("Receiver created!");

    // AR shouldn't have an arena, should it?  Maybe something large?
    //LOGI("Setting default 1m radius arena boundary.", result);
    //cxrSetArenaBoundary(Receiver, 10.f, 0, 0);

    return cxrError_Success;
}

void CloudXRClient::HandleTouch(float x, float y) {
    if (!IsRunning()) return;

    cxrInputEvent input;
    input.type = cxrInputEventType_Touch;
    input.event.touchEvent.type = cxrTouchEventType_FINGERUP;
    input.event.touchEvent.x = x;
    input.event.touchEvent.y = y;
    cxrSendInputEvent(cloudxr_receiver_, &input);
}

void CloudXRClient::SetStreamRes(uint32_t w, uint32_t h) {
    // apply the res factor to width and height, and make sure they are even for stream res.
    stream_width_ = ((uint32_t)round((float)w * launch_options_.res_factor_)) & ~1;
    stream_height_ = ((uint32_t)round((float)h * launch_options_.res_factor_)) & ~1;
    LOGI("SetStreamRes: Display res passed = %dx%d", w, h);
    LOGI("SetStreamRes: Stream res set = %dx%d [factor %0.2f]", stream_width_, stream_height_, launch_options_.res_factor_);
}

bool CloudXRClient::HandleLaunchOptions(std::string &cmdline) {
    // first, try to read "command line in a text file"
    launch_options_.ParseFile("/sdcard/CloudXRLaunchOptions.txt");
    // next, process actual 'commandline' args -- overrides any prior values
    LOGI("Parsing commandline string: %s", cmdline.c_str());
    launch_options_.ParseString(cmdline);

    return true;
}

void CloudXRClient::Stats() {
    // Log connection stats every 3 seconds
    const int STATS_INTERVAL_SEC = 3;
    frames_until_stats_--;
    if (frames_until_stats_ <= 0 &&
        cxrGetConnectionStats(cloudxr_receiver_, &stats_) == cxrError_Success)
    {
        // Capture the key connection statistics
        char statsString[64] = { 0 };
        snprintf(statsString, 64, "FPS: %6.1f    Bitrate (kbps): %5d    Latency (ms): %3d",
                 stats_.framesPerSecond, stats_.bandwidthUtilizationKbps, stats_.roundTripDelayMs);

        // Turn the connection quality into a visual representation along the lines of a signal strength bar
        char qualityString[64] = { 0 };
        snprintf(qualityString, 64, "Connection quality: [%c%c%c%c%c]",
                 stats_.quality >= cxrConnectionQuality_Bad ? '#' : '_',
                 stats_.quality >= cxrConnectionQuality_Poor ? '#' : '_',
                 stats_.quality >= cxrConnectionQuality_Fair ? '#' : '_',
                 stats_.quality >= cxrConnectionQuality_Good ? '#' : '_',
                 stats_.quality == cxrConnectionQuality_Excellent ? '#' : '_');

        // There could be multiple reasons for low quality however we show only the most impactful to the end user here
        char reasonString[64] = { 0 };
        if (stats_.quality <= cxrConnectionQuality_Fair)
        {
            if (stats_.qualityReasons == cxrConnectionQualityReason_EstimatingQuality)
            {
                snprintf(reasonString, 64, "Reason: Estimating quality");
            }
            else if (stats_.qualityReasons & cxrConnectionQualityReason_HighLatency)
            {
                snprintf(reasonString, 64, "Reason: High Latency (ms): %3d", stats_.roundTripDelayMs);
            }
            else if (stats_.qualityReasons & cxrConnectionQualityReason_LowBandwidth)
            {
                snprintf(reasonString, 64, "Reason: Low Bandwidth (kbps): %5d", stats_.bandwidthAvailableKbps);
            }
            else if (stats_.qualityReasons & cxrConnectionQualityReason_HighPacketLoss)
            {
                if (stats_.totalPacketsLost == 0)
                {
                    snprintf(reasonString, 64, "Reason: High Packet Loss (Recoverable)");
                }
                else
                {
                    snprintf(reasonString, 64, "Reason: High Packet Loss (%%): %3.1f", 100.0f * stats_.totalPacketsLost / stats_.totalPacketsReceived);
                }
            }
        }

        LOGI("%s    %s    %s", statsString, qualityString, reasonString);
        frames_until_stats_ = (int)stats_.framesPerSecond * STATS_INTERVAL_SEC;
    }
}

bool CloudXRClient::Render(FrameMask eye) {
    if (!IsRunning() || !latched_) {
        return false;
    }
    bool res = false;

    res = cxrBlitFrame(cloudxr_receiver_, &frames_latched_, eye);

//    if (eye == FrameMask_Left || eye == FrameMask_Right) {
//        int eyeNum = eye == FrameMask_Left ? 0 : 1;
//        res = cxrBlitFrame(cloudxr_receiver_, &frames_latched_, 1 << eyeNum);
//        LOGV("CloudXR Render eye[%d] %d res %d", eye, eyeNum, res);
//    } else {
//        res = cxrBlitFrame(cloudxr_receiver_, &frames_latched_, eye);
//    }

    return res;
}

cxrError CloudXRClient::Release() {
    if (!latched_) {
        return cxrError_Frame_Not_Ready;
    }

    cxrError error = cxrReleaseFrame(cloudxr_receiver_, &frames_latched_);
    latched_ = false;
    return error;
}

cxrError CloudXRClient::Latch(cxrFramesLatched& framesLatched) {
    if (latched_) {
        return cxrError_Success;
    }

    if (!IsRunning()) {
        return cxrError_Receiver_Not_Running;
    }

    // Fetch the frame
    const uint32_t timeout_ms = 150;
    cxrError status = cxrLatchFrame(cloudxr_receiver_, &frames_latched_, cxrFrameMask_All, timeout_ms);

    if (status != cxrError_Success) {
        LOGI("CloudXR frame is not available!");
        return status;
    }

    latched_ = true;
    framesLatched = frames_latched_;

    return cxrError_Success;
}

int CloudXRClient::DetermineOffset() const {
    for (int offset = 0; offset < kQueueLen; offset++) {
        const int idx = current_idx_ < offset ?
                        kQueueLen + (current_idx_ - offset)%kQueueLen :
                        (current_idx_ - offset)%kQueueLen;

        const auto& pose_matrix = pose_matrix_[idx];

        int notMatch = 0;
        for (int i=0; i<3; i++) {
            for (int j=0; j<4; j++) {
                if (fabsf(pose_matrix.m[i][j] - frames_latched_.poseMatrix.m[i][j]) >= 0.0001f)
                    notMatch++;
            }
        }
        if (0==notMatch) // then matrices are close enough to qualify as equal
            return offset;
    }

    return 0;
}

void CloudXRClient::SetFps(int fps) {
    fps_ = fps;
}

void CloudXRClient::SetProjectionMatrix(int eye, const glm::mat4 &projection) {
    if (fabsf(projection[2][0]) > 0.0001f) {
        // Non-symmetric projection
        const float oneOver00 = 1.f/projection[0][0];
        const float l = -(1.f - projection[2][0])*oneOver00;
        const float r = 2.f*oneOver00 + l;

        const float oneOver11 = 1.f/projection[1][1];
        const float b = -(1.f - projection[2][1])*oneOver11;
        const float t = 2.f*oneOver11 + b;

        // TODO why ? fov top and bottom ?
        // quest default
        // l=52.000000 r=42.000000 t=53.000000 b=47.000004
        // 1.279942    0.9004      1.32704     1.072
        // Proj: -1.279942 0.900404 -1.072369 1.327045
        // l=42.000000 r=52.000000 t=53.000000 b=47.000004
        // 0.9004      1.279942    1.32704     1.072
        // Proj: -0.900404 1.279942 -1.072369 1.327045 ? old not right
        // 0-3 for left, right, top, bottom edges.

        device_desc_.proj[eye][0] = l;
        device_desc_.proj[eye][1] = r;
        device_desc_.proj[eye][2] = b;
        device_desc_.proj[eye][3] = t;
    } else {
        // Symmetric projection
        device_desc_.proj[eye][0] = -1.f/projection[0][0];
        device_desc_.proj[eye][1] = -device_desc_.proj[eye][0];
        device_desc_.proj[eye][2] = -1.f/projection[1][1];
        device_desc_.proj[eye][3] = -device_desc_.proj[eye][2];
    }

    LOGI("Proj[%d]: %f %f %f %f", eye, device_desc_.proj[eye][0], device_desc_.proj[eye][1],
         device_desc_.proj[eye][2], device_desc_.proj[eye][3]);
}

void CloudXRClient::SetProjectionMatrix(int eye, const larkxrRenderFov& larkxrFov) {
    double rr = M_PI / 180;
    device_desc_.proj[eye][0] = -tan(larkxrFov.left * rr);
    device_desc_.proj[eye][1] = tan(larkxrFov.right * rr);
    device_desc_.proj[eye][2] = -tan(larkxrFov.top * rr);
    device_desc_.proj[eye][3] = tan(larkxrFov.bottom * rr);
}

void CloudXRClient::SetPoseMatrix(const glm::mat4 &pose_mat) {
    auto& pose_matrix = pose_matrix_[current_idx_];

    pose_matrix.m[0][0] = pose_mat[0][0];
    pose_matrix.m[0][1] = pose_mat[1][0];
    pose_matrix.m[0][2] = pose_mat[2][0];
    pose_matrix.m[0][3] = pose_mat[3][0];
    pose_matrix.m[1][0] = pose_mat[0][1];
    pose_matrix.m[1][1] = pose_mat[1][1];
    pose_matrix.m[1][2] = pose_mat[2][1];
    pose_matrix.m[1][3] = pose_mat[3][1];
    pose_matrix.m[2][0] = pose_mat[0][2];
    pose_matrix.m[2][1] = pose_mat[1][2];
    pose_matrix.m[2][2] = pose_mat[2][2];
    pose_matrix.m[2][3] = pose_mat[3][2];

    current_idx_ = (current_idx_ + 1)%kQueueLen;
}

void CloudXRClient::Teardown() {
    if (playback_stream_) {
        playback_stream_->close();
        playback_stream_ = nullptr;
    }

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_ = nullptr;
    }

    if (cloudxr_receiver_) {
        LOGI("Tearing down CloudXR...");
        cxrDestroyReceiver(cloudxr_receiver_);
        cloudxr_receiver_ = nullptr;
    }

    client_state_ = cxrClientState_ReadyToConnect;
    inited_ = false;
}

oboe::DataCallbackResult
CloudXRClient::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    if (cloudxr_receiver_ && IsConnect()) {
        cxrAudioFrame recordedFrame{};
        recordedFrame.streamBuffer = (int16_t *) audioData;
        recordedFrame.streamSizeBytes =
                numFrames * CXR_AUDIO_CHANNEL_COUNT * CXR_AUDIO_SAMPLE_SIZE;
        cxrSendAudio(cloudxr_receiver_, &recordedFrame);

        return oboe::DataCallbackResult::Continue;
    } else {
        return oboe::DataCallbackResult::Continue;
    }
}

void
CloudXRClient::Draw(lark::Object::Eye eye, const glm::mat4 &projection, const glm::mat4 &view) {
    Object::Draw(eye, projection, view);

    // WARNING  GL_SCISSOR_TEST NOT SUPPORT IN CLOUDXR
    // enabel GL_SCISSOR_TEST CLOUDXR draw black
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);

    Render(eye == lark::Object::EYE_LEFT ? FrameMask_Left : FrameMask_Right);

    glEnable(GL_SCISSOR_TEST);
    // 开启透明同道混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void CloudXRClient::DrawMultiview(const glm::mat4 &projection, const glm::mat4 &view) {
    Object::DrawMultiview(projection, view);

    // WARNING  GL_SCISSOR_TEST NOT SUPPORT IN CLOUDXR
    // enabel GL_SCISSOR_TEST CLOUDXR draw black
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);

    Render(FrameMask_All);

    glEnable(GL_SCISSOR_TEST);
    // 开启透明同道混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void CloudXRClient::InitRenderParamsWithLarkXRConfig() {
    for (int eye = 0; eye < 2; eye ++) {
        SetProjectionMatrix(eye, lark::XRConfig::fov[eye]);
    }
    set_ipd(lark::XRConfig::ipd);
    SetFps(lark::XRConfig::fps);
    SetStreamRes(lark::XRConfig::render_width /  2, lark::XRConfig::render_height);
}

cxrVRTrackingState CloudXRClient::VRTrackingStateFrom(const larkxrTrackingDevicePairFrame &pxyframe) {
    cxrVRTrackingState res = {};

    res.hmd.flags |= cxrHmdTrackingFlags_HasPoseID;

    // res.hmd.flags = 0; // TODO: consider tracking local IPD value and only flag when it actually changes...

    res.hmd.poseID = pxyframe.frameIndex;

    res.hmd.pose.position.v[0] = pxyframe.devicePair.hmdPose.position.x;
    res.hmd.pose.position.v[1] = pxyframe.devicePair.hmdPose.position.y;
    res.hmd.pose.position.v[2] = pxyframe.devicePair.hmdPose.position.z;

    res.hmd.pose.rotation.w = pxyframe.devicePair.hmdPose.rotation.w;
    res.hmd.pose.rotation.x = pxyframe.devicePair.hmdPose.rotation.x;
    res.hmd.pose.rotation.y = pxyframe.devicePair.hmdPose.rotation.y;
    res.hmd.pose.rotation.z = pxyframe.devicePair.hmdPose.rotation.z;

    res.hmd.pose.angularVelocity.v[0] = pxyframe.devicePair.hmdPose.angularVelocity.x;
    res.hmd.pose.angularVelocity.v[1] = pxyframe.devicePair.hmdPose.angularVelocity.y;
    res.hmd.pose.angularVelocity.v[2] = pxyframe.devicePair.hmdPose.angularVelocity.z;

    res.hmd.pose.velocity.v[0] = pxyframe.devicePair.hmdPose.velocity.x;
    res.hmd.pose.velocity.v[1] = pxyframe.devicePair.hmdPose.velocity.y;
    res.hmd.pose.velocity.v[2] = pxyframe.devicePair.hmdPose.velocity.z;

    res.hmd.pose.poseIsValid = cxrTrue;
    res.hmd.pose.deviceIsConnected = cxrTrue;

    res.hmd.pose.trackingResult = cxrTrackingResult_Running_OK;

    for (int hand = 0; hand < 2; hand ++) {
        res.controller[hand].pose.poseIsValid = cxrTrue;
        res.controller[hand].pose.deviceIsConnected = cxrTrue;
        res.controller[hand].pose.trackingResult = cxrTrackingResult_Running_OK;
        res.controller[hand].pose.position.v[0] = pxyframe.devicePair.controllerState[hand].pose.position.x;
        res.controller[hand].pose.position.v[1] = pxyframe.devicePair.controllerState[hand].pose.position.y;
        res.controller[hand].pose.position.v[2] = pxyframe.devicePair.controllerState[hand].pose.position.z;

        res.controller[hand].pose.rotation.w = pxyframe.devicePair.controllerState[hand].pose.rotation.w;
        res.controller[hand].pose.rotation.x = pxyframe.devicePair.controllerState[hand].pose.rotation.x;
        res.controller[hand].pose.rotation.y = pxyframe.devicePair.controllerState[hand].pose.rotation.y;
        res.controller[hand].pose.rotation.z = pxyframe.devicePair.controllerState[hand].pose.rotation.z;

        uint64_t buttons = pxyframe.devicePair.controllerState[hand].inputState.buttons;

        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_A_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_A);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_B_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_B);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_X_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_X);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_Y_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Y);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_Joystick_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Joystick_Click);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_Joystick_Touch)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Joystick_Touch);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_Trigger_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Trigger_Click);
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Trigger_Touch);
        }
        if ((buttons & LARKXR_BUTTON_FLAG(larkxr_Input_Grip_Click)) != 0) {
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Grip_Click);
            res.controller[hand].booleanComps |= CLOUDXR_BUTTON_FLAG(cxrButtonId::cxrButton_Grip_Touch);
        }

        // TODO save pre_controller_state
        static uint64_t pre_controller_state[2] = {};

        res.controller[hand].booleanCompsChanged = pre_controller_state[hand] ^ res.controller[hand].booleanComps;
        pre_controller_state[hand] = res.controller[hand].booleanComps;

        res.controller[hand].scalarComps[cxrAnalog_Trigger] = pxyframe.devicePair.controllerState[hand].inputState.triggerValue;
        res.controller[hand].scalarComps[cxrAnalog_Grip] = pxyframe.devicePair.controllerState[hand].inputState.gripValue;
        res.controller[hand].scalarComps[cxrAnalog_Grip_Force] = pxyframe.devicePair.controllerState[hand].inputState.gripValue;
        res.controller[hand].scalarComps[cxrAnalog_JoystickX] = pxyframe.devicePair.controllerState[hand].inputState.touchPadAxis.x;
        res.controller[hand].scalarComps[cxrAnalog_JoystickY] = pxyframe.devicePair.controllerState[hand].inputState.touchPadAxis.y;
    }

    return res;
}

