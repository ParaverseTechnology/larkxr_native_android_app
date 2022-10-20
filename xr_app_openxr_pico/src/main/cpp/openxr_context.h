//
// Created by Administrator on 2021/7/20.
//

#ifndef CLOUDLARKXR_OPENXR_CONTEXT_H
#define CLOUDLARKXR_OPENXR_CONTEXT_H

#include "pch.h"
#include "options.h"
#include "platformplugin.h"
#include "graphics_device_android.h"
#include "openxr/openxr.h"
#include "xr_input_state.h"

struct Swapchain {
    XrSwapchain handle;
    int32_t width;
    int32_t height;
};

class OpenxrContext {
public:
    OpenxrContext(const std::shared_ptr<Options>& options, const std::shared_ptr<IPlatformPlugin>& platformPlugin);
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
    void PollEvents(bool* exitRenderLoop, bool* requestRestart);

    void PollActions();

    // Manage session lifecycle to track if RenderFrame should be called.
    inline bool IsSessionRunning() const { return session_running_; };

    // Manage session state to track if input should be processed.
    inline bool IsSessionFocused() const { return session_state_ == XR_SESSION_STATE_FOCUSED; };

    inline const XrInstance& instance() const { return instance_; }
    inline const XrSession& session() const  { return session_; }
    inline const XrSpace& app_space() const { return app_space_; }
    inline const InputState& input() { return input_;}
    inline const XrEnvironmentBlendMode& environment_blend_mode() { return environment_blend_mode_; }
    inline const std::vector<XrView>& views() { return views_; }

    void ResetSensor(const XrResetSensorOption& option);
    void GetConfig(enum ConfigsEXT configIndex, float* configData);
    inline GraphicsDeviceAndroid* graphics_plugin() { return graphics_plugin_.get(); }
    inline const XrViewConfigurationType& view_config_type() { return view_config_type_; }
    inline const std::vector<XrViewConfigurationView>& config_views() { return config_views_; }
    inline const std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>>& swapchain_images() {
        return swapchain_images_;
    }
    inline const std::vector<Swapchain>& swapchains() { return swapchains_; }
    inline int color_swapchain_format() { return color_swapchain_format_; }
    inline XrSystemId system_id() { return system_id_; }
private:
    void LogInstanceInfo();
    void LogViewConfigurations();
    void LogEnvironmentBlendMode(XrViewConfigurationType type);
    void LogReferenceSpaces();
    void LogActionSourceName(XrAction action, const std::string& actionName) const;

    // Return event if one is available, otherwise return null.
    const XrEventDataBaseHeader* TryReadNextEvent();
    void HandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop,
                                        bool* requestRestart);

    const std::shared_ptr<Options> options_;
    std::shared_ptr<IPlatformPlugin> platform_plugin_;
    std::shared_ptr<GraphicsDeviceAndroid> graphics_plugin_;
    XrInstance instance_{XR_NULL_HANDLE};
    XrSession session_{XR_NULL_HANDLE};
    XrSpace app_space_{XR_NULL_HANDLE};
    XrFormFactor form_factor_{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};
    XrViewConfigurationType view_config_type_{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};
    XrEnvironmentBlendMode environment_blend_mode_{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};
    XrSystemId system_id_{XR_NULL_SYSTEM_ID};

    std::vector<XrViewConfigurationView> config_views_;
    std::vector<Swapchain> swapchains_;
    std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> swapchain_images_;
    std::vector<XrView> views_;
    int64_t color_swapchain_format_{-1};

    // Application's current lifecycle state according to the runtime
    XrSessionState session_state_{XR_SESSION_STATE_UNKNOWN};
    bool session_running_{false};

    XrEventDataBuffer event_data_buffer_;
    InputState input_;

    PFN_xrResetSensorPICO  pfn_xr_reset_sensor_pico_ = nullptr;
    PFN_xrGetConfigPICO    pfn_xr_get_config_pico_ = nullptr;
    PFN_xrSetConfigPICO    pfn_xr_set_config_pico_ = nullptr;
};


#endif //CLOUDLARKXR_OPENXR_CONTEXT_H
