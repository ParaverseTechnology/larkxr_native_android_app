//
// Created by fcx on 2020/5/6.
//

#include <ui/home/home.h>
#include <ui/navigation.h>
#include <oboe/AudioStreamBuilder.h>
#include "application.h"
#include "env_context.h"
#include "log.h"
#include "utils.h"

const uint32_t CXR_AUDIO_CHANNEL_COUNT = 2;             ///< Audio is currently always stereo
const uint32_t CXR_AUDIO_SAMPLE_SIZE = sizeof(int16_t); ///< Audio is currently signed 16-bit samples (little-endian)
const uint32_t CXR_AUDIO_SAMPLING_RATE = 48000;         ///< Audio is currently always 48khz
const uint32_t CXR_AUDIO_FRAME_LENGTH_MS = 5;           ///< Sent audio has a 5 ms default frame length.  Received audio has 5 or 10 ms frame length, depending on the configuration.
const uint32_t CXR_AUDIO_BYTES_PER_MS = CXR_AUDIO_CHANNEL_COUNT * CXR_AUDIO_SAMPLE_SIZE * CXR_AUDIO_SAMPLING_RATE / 1000; ///< Total bytes of audio per ms

void Application::RegiseredInstance(Application *instance) {
    s_instance_ = instance;
}

void Application::UnRegiseredInstance() {
    s_instance_ = nullptr;
}

Application* Application::instance() {
    return s_instance_;
}

Application* Application::s_instance_ = nullptr;

Application::Application() = default;

Application::~Application() {
    connected_ = false;

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }
}

void Application::EnterAppli(const std::string &appId) {
    LOGV("enterappli with appid %s", appId.c_str());
    if (xr_client_) {
        xr_client_->EnterAppli(appId);
    }
}

void Application::EnterAppliParams(const lark::EnterAppliParams &params) {
    LOGV("enterappli with params appid %s region %s", params.appliId.c_str(), params.regionId.c_str());
    if (xr_client_) {
        xr_client_->EnterAppli(params);
    }
}

void Application::CloseAppli() {
}

void Application::SetServerAddr(const std::string &ip, uint16_t port) {
    if (xr_client_) {
        xr_client_->OnPause();
        xr_client_->SetServerAddr(ip, port);
        xr_client_->OnResume();
    }
}

void Application::OnClientId(const std::string& clientId) {
    Home::SetClientId(clientId);
}

void Application::OnInfo(int infoCode, const std::string &msg) {
    Navigation::ShowToast(msg);
}

void Application::InitCertificate() {
    if (!Context::instance()) {
        LOGW("Init certificate failed context not created.");
        return;
    }
    std::string internalPath = Context::instance()->internal_data_path();
    std::string externalPath = Context::instance()->external_data_path();

    std::string path = !externalPath.empty() ? externalPath : internalPath;

    if (path.empty()) {
        LOGW("init certificate failed. data path empty.");
        return;
    }
    std::string appKeyPath = path + "/larkxr/certificate_appkey.txt";
    std::string appSecretPath = path + "/larkxr/certificate_appsecret.txt";

    char appkey[1024] = {};
    int appKeyLen = utils::ReadFile(appKeyPath.c_str(), appkey, 1024);

    if (appKeyLen <= 0) {
        LOGW("read appkey certificate failed. %s code %d", appKeyPath.c_str(), appKeyLen);
        return;
    }

    char appSecret[1024] = {};
    int appSecretLen = utils::ReadFile(appSecretPath.c_str(), appSecret, 1024);

    LOGV("appkey %s appSecret %s", appkey, appSecret);
    lark::XRClient::SetCertificate(std::string(appkey, appKeyLen), std::string(appSecret, appSecretLen));
}

void Application::RequestAudioInput() {
    XRClientObserverWrap::RequestAudioInput();

    LOGV("RequestAudioInput");

    // Initialize audio recording
    oboe::AudioStreamBuilder recording_stream_builder;
    recording_stream_builder.setDirection(oboe::Direction::Input);
    recording_stream_builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    recording_stream_builder.setSharingMode(oboe::SharingMode::Exclusive);
    recording_stream_builder.setFormat(oboe::AudioFormat::I16);
    recording_stream_builder.setChannelCount(oboe::ChannelCount::Stereo);
    recording_stream_builder.setSampleRate(CXR_AUDIO_SAMPLING_RATE);
    recording_stream_builder.setInputPreset(oboe::InputPreset::VoiceCommunication);
    recording_stream_builder.setDataCallback(this);

    oboe::Result r = recording_stream_builder.openStream(recording_stream_);
    if (r != oboe::Result::OK) {
        LOGE("Failed to open recording stream. Error: %s", oboe::convertToText(r));
        //return; // for now continue to run...
    }
    else
    {
        r = recording_stream_->start();
        if (r != oboe::Result::OK)
        {
            LOGE("Failed to start recording stream. Error: %s", oboe::convertToText(r));
            //return; // for now continue to run...
        } else {
            LOGV("Start recod stream success");
        }
    }

    // if there was an error setting up, turn off sending audio for this connection.
    if (r != oboe::Result::OK) {
        LOGV("clear recod stream when failed");
        if (recording_stream_) {
            recording_stream_->close();
            recording_stream_.reset();
        }
    } else {
        LOGV("start recod stream success");
    }
}

void Application::OnClose(int code) {
    XRClientObserverWrap::OnClose(code);

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }
}

oboe::DataCallbackResult
Application::onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
//    LOGV("onAudioReady %d", numFrames);
    if (!connected_ || !xr_client_) {
        LOGV("skip on audio ready %d", numFrames);
        return oboe::DataCallbackResult::Continue;
    }
    int streamSizeBytes = numFrames * CXR_AUDIO_CHANNEL_COUNT * CXR_AUDIO_SAMPLE_SIZE;

//    LOGV("onAudioReady %d", streamSizeBytes);

    xr_client_->SendAudioData(static_cast<const char *>(audioData), streamSizeBytes);

    return oboe::DataCallbackResult::Continue;
}

void Application::OnConnected() {
    XRClientObserverWrap::OnConnected();
    // DEBUG AUDIO INPUT
//     RequestAudioInput();
}

void Application::OnError(int errCode, const std::string &msg) {
    XRClientObserverWrap::OnError(errCode, msg);

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }
}

