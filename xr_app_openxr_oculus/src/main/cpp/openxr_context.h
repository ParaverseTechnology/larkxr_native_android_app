//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_OPENXR_CONTEXT_H
#define LARKXR_OPENXR_CONTEXT_H

#include <memory>
#include "graphics_device_android.h"
#include "frame_buffer.h"
#include "input_state.h"

namespace oxr {
class OpenxrContext {
public:
    static const int CPU_LEVEL = 2;
    static const int GPU_LEVEL = 3;
    static const int NUM_MULTI_SAMPLES = 4;
    static const std::string SYSTEM_NAME_QUEST;
    static const std::string SYSTEM_NAME_QUEST2;

    enum { ovrMaxLayerCount = 16 };
    enum { ovrMaxNumEyes = 2 };

    OpenxrContext();
    ~OpenxrContext();

    // Create an Instance and other basic instance-level initialization.
    void CreateInstance();

    // Select a System for the view configuration specified in the Options and initialize the graphics device for the selected
    // system.
    void InitializeSystem();

    // Create a Session and other basic session-level initialization.
    void InitializeSession();

    // Create a Swapchain which requires coordinating with the graphics plugin to select the format, getting the system graphics
    // properties, getting the view configuration and grabbing the resulting swapchain images.
    void CreateSwapchains();

    // Process any events in the event queue.
    void PollEvents();

    void PollActions();

    void Destory();

    float GetCurrentDisplayRefreshRate();
    void SetDisplayRefreshRate(float fps);
    void SetColorSpace(XrColorSpaceFB colorSpaceFB);
    void SetFoveation(XrFoveationLevelFB level,
                      float verticalOffset,
                      XrFoveationDynamicFB dynamic);

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
    inline FrameBuffer* frame_buffer() { return frame_buffer_; }
    inline FrameBuffer& frame_buffer(int eye) { return frame_buffer_[eye]; }

    inline XrViewConfigurationProperties viewport_config() { return viewport_config_; }

    inline std::vector<XrView>& views() { return views_; }

    inline const XrSystemProperties& system_properties() { return system_properties_; }

    inline std::vector<float>& support_display_refresh_rates() { return support_display_refresh_rates_; }
private:
    void InitSpace();
    void LogInfos();
    void HandleSessionStateChanges(XrSessionState state);

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

    // basic setup
    int cpu_level_ = 2;
    int gpu_level_ = 2;
    // These threads will be marked as performance threads.
    int main_thread_tid_ = 0;
    int render_thread_tid_ = 0;

    // views
    XrViewConfigurationProperties viewport_config_ = {};
    XrViewConfigurationView view_configuration_view_[ovrMaxNumEyes] = {};

    // fps
    std::vector<float> support_display_refresh_rates_ {};
    uint32_t RequestedDisplayRefreshRateIndex{};

    // ptr...
    PFN_xrGetDisplayRefreshRateFB pfnGetDisplayRefreshRate = nullptr;
    PFN_xrRequestDisplayRefreshRateFB pfnRequestDisplayRefreshRate = nullptr;
    PFN_xrEnumerateDisplayRefreshRatesFB pfnxrEnumerateDisplayRefreshRatesFB = nullptr;
    // color space ptr
    PFN_xrEnumerateColorSpacesFB pfnxrEnumerateColorSpacesFB = nullptr;
    PFN_xrSetColorSpaceFB pfnxrSetColorSpaceFB = nullptr;
    PFN_xrCreateFoveationProfileFB pfnCreateFoveationProfileFB = nullptr;
    PFN_xrDestroyFoveationProfileFB pfnDestroyFoveationProfileFB = nullptr;
    PFN_xrUpdateSwapchainFB pfnUpdateSwapchainFB;

    std::unique_ptr<GraphicsDeviceAndroid> graphics_plugin_;
    InputState input_state_;
    FrameBuffer frame_buffer_[ovrMaxNumEyes];

    // state
    bool resumed_ = false;
    bool focused_ = false;
    bool session_active_ = false;
    ANativeWindow* native_window_ = nullptr;
};
}

#endif //LARKXR_OPENXR_CONTEXT_H
