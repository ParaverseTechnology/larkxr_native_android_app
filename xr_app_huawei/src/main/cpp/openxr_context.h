//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_OPENXR_CONTEXT_H
#define LARKXR_OPENXR_CONTEXT_H

#include <memory>
#include "graphics_device_android.h"
#include "input_state.h"

namespace hxr {
class OpenxrContext {
public:
    enum { ovrMaxNumEyes = 2 };

    OpenxrContext();
    ~OpenxrContext();

    // Create an Instance and other basic instance-level initialization.
    void CreateInstance(jobject jactivity, JavaVM* jvm);

    // Select a System for the view configuration specified in the Options and initialize the graphics device for the selected
    // system.
    void InitializeSystem();

    // Create a Session and other basic session-level initialization.
    void InitializeSession(ANativeWindow* nativeWindow);

    // Create a Swapchain which requires coordinating with the graphics plugin to select the format, getting the system graphics
    // properties, getting the view configuration and grabbing the resulting swapchain images.
    void CreateSwapchains();

    // Process any events in the event queue.
    void PollEvents(bool* exitRenderLoop, bool* sessionRunning, bool* sessionStateStopping, bool* sessionStateExiting);

    void PollActions();

    void Destory();

    void BeginSession();

    inline void set_resumed(bool resumed) { resumed_ = resumed; }
    inline bool resumed() { return resumed_; }
    inline void set_native_window(ANativeWindow* nativeWindow) { native_window_ = nativeWindow; }
    inline bool session_active() { return session_active_; }
    inline InputState& input_state() { return input_state_; }

    inline const XrInstance& instance() const { return instance_; }
    inline const XrSession& session() const  { return session_; }
    inline const XrSpace& app_space() const { return app_space_; }
    inline const XrSpace& local_space() const { return local_space_; }
    inline const XrSpace& head_space() const { return head_space_; }
    inline XrSystemId system_id() { return system_id_; }

    inline XrViewConfigurationProperties viewport_config() { return viewport_config_; }

    inline std::vector<XrView>& views() { return views_; }

    inline const XrSystemProperties& system_properties() { return system_properties_; }

    inline std::vector<float>& support_display_refresh_rates() { return support_display_refresh_rates_; }

    inline std::vector<XrSwapchain>& swapchains() { return swapchains_;}
    inline std::vector<std::vector<XrSwapchainImageOpenGLESKHR>>& swapchains_image_array() { return swapchains_image_array_;}

    inline GraphicsDeviceAndroid&  graphics_plugin() const { return *graphics_plugin_; }

    inline double GetSeconds()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        const int64_t result = (int64_t)tp.tv_sec * (int64_t)(1000 * 1000 * 1000) + int64_t(tp.tv_nsec);

        return result*0.000000001;
    }
    inline double GetNanos()
    {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC, &tp);
        const int64_t result = (int64_t)tp.tv_sec * (int64_t)(1000 * 1000 * 1000) + int64_t(tp.tv_nsec);

        return result;
    }
private:
    void InitSpace();
    void LogInfos();

    // session
    XrInstance instance_{XR_NULL_HANDLE};
    XrSession session_{XR_NULL_HANDLE};
    XrSpace app_space_{XR_NULL_HANDLE};
    XrSpace head_space_{XR_NULL_HANDLE};
    XrSpace local_space_{XR_NULL_HANDLE};
    XrSystemId system_id_{XR_NULL_SYSTEM_ID};

    XrSystemProperties system_properties_{};

    // views
    std::vector<XrView> views_ = {};


    // views
    XrViewConfigurationProperties viewport_config_ = {};
    XrViewConfigurationView view_configuration_view_[ovrMaxNumEyes] = {};

    // fps
    std::vector<float> support_display_refresh_rates_ {};
    uint32_t RequestedDisplayRefreshRateIndex{};


    std::unique_ptr<GraphicsDeviceAndroid> graphics_plugin_;
    InputState input_state_;

    // state
    bool resumed_ = false;
    bool focused_ = false;
    bool session_active_ = false;
    ANativeWindow* native_window_ = nullptr;

    //Jvm & Activity
    jobject activity_;
    JavaVM* jvm_;

    std::vector<XrSwapchain> swapchains_{};
    std::vector<std::vector<XrSwapchainImageOpenGLESKHR>> swapchains_image_array_{};

    //time
    double start_;
    double finish_;

    //Instance State
    bool instance_loss_state_;
    double instance_loss_state_time_;
};
}

#endif //LARKXR_OPENXR_CONTEXT_H
