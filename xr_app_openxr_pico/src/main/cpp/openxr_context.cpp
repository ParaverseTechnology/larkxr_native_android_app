//
// Created by Administrator on 2021/7/20.
//

#include "openxr_context.h"
#include "common.h"
#include "pvr_xr_utils.h"
#include "pController.h"

using namespace pxrutils;

namespace {
    void LogLayersAndExtensions() {
        // Write out extension properties for a given layer.
        const auto logExtensions = [](const char* layerName, int indent = 0) {
            uint32_t instanceExtensionCount;
            CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(layerName, 0, &instanceExtensionCount, nullptr));

            std::vector<XrExtensionProperties> extensions(instanceExtensionCount);
            for (XrExtensionProperties& extension : extensions) {
                extension.type = XR_TYPE_EXTENSION_PROPERTIES;
            }

            CHECK_XRCMD(xrEnumerateInstanceExtensionProperties(layerName, (uint32_t)extensions.size(), &instanceExtensionCount,
                                                               extensions.data()));

            const std::string indentStr(indent, ' ');
            Log::Write(Log::Level::Verbose, Fmt("%sAvailable Extensions: (%d)", indentStr.c_str(), instanceExtensionCount));
            for (const XrExtensionProperties& extension : extensions) {
                Log::Write(Log::Level::Verbose, Fmt("%s  Name=%s SpecVersion=%d", indentStr.c_str(), extension.extensionName,
                                                    extension.extensionVersion));
            }
        };

        // Log non-layer extensions (layerName==nullptr).
        logExtensions(nullptr);

        // Log layers and any of their extensions.
        {
            uint32_t layerCount;
            CHECK_XRCMD(xrEnumerateApiLayerProperties(0, &layerCount, nullptr));

            std::vector<XrApiLayerProperties> layers(layerCount);
            for (XrApiLayerProperties& layer : layers) {
                layer.type = XR_TYPE_API_LAYER_PROPERTIES;
            }

            CHECK_XRCMD(xrEnumerateApiLayerProperties((uint32_t)layers.size(), &layerCount, layers.data()));

            Log::Write(Log::Level::Info, Fmt("Available Layers: (%d)", layerCount));
            for (const XrApiLayerProperties& layer : layers) {
                Log::Write(Log::Level::Verbose,
                           Fmt("  Name=%s SpecVersion=%s LayerVersion=%d Description=%s", layer.layerName,
                               GetXrVersionString(layer.specVersion).c_str(), layer.layerVersion, layer.description));
                logExtensions(layer.layerName, 4);
            }
        }
    }
}

OpenxrContext::OpenxrContext(const std::shared_ptr<Options>& options, const std::shared_ptr<IPlatformPlugin>& platformPlugin)
    :options_(options), platform_plugin_(platformPlugin)
{
    graphics_plugin_ = std::make_shared<GraphicsDeviceAndroid>();
}

OpenxrContext::~OpenxrContext() {
    for (auto & i : frame_buffer_) {
        i.Destroy();
    }

    if (input_.actionSet != XR_NULL_HANDLE) {
        for (auto hand : {Side::LEFT, Side::RIGHT}) {
            xrDestroySpace(input_.handSpace[hand]);
        }
        xrDestroyActionSet(input_.actionSet);
    }

    if (app_space_ != XR_NULL_HANDLE) {
        xrDestroySpace(app_space_);
    }

    if (session_ != XR_NULL_HANDLE) {
        xrDestroySession(session_);
    }

    if (instance_ != XR_NULL_HANDLE) {
        xrDestroyInstance(instance_);
    }
}

void OpenxrContext::CreateInstance() {
    LogLayersAndExtensions();

    CHECK(instance_ == XR_NULL_HANDLE);

    // Create union of extensions required by platform and graphics plugins.
    std::vector<const char*> extensions;

    // Transform platform and graphics extension std::strings to C strings.
    const std::vector<std::string> platformExtensions = platform_plugin_->GetInstanceExtensions();
    std::transform(platformExtensions.begin(), platformExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });
    const std::vector<std::string> graphicsExtensions = graphics_plugin_->GetInstanceExtensions();
    std::transform(graphicsExtensions.begin(), graphicsExtensions.end(), std::back_inserter(extensions),
                   [](const std::string& ext) { return ext.c_str(); });

    extensions.push_back(XR_PICO_VIEW_STATE_EXT_ENABLE_EXTENSION_NAME);
    extensions.push_back(XR_PICO_FRAME_END_INFO_EXT_EXTENSION_NAME);
    //Enable Pico controller extension
    extensions.push_back(XR_PICO_ANDROID_CONTROLLER_FUNCTION_EXT_ENABLE_EXTENSION_NAME);
    // Enable reset haed sensor extension
    extensions.push_back(XR_PICO_CONFIGS_EXT_EXTENSION_NAME);
    extensions.push_back(XR_PICO_RESET_SENSOR_EXTENSION_NAME);
    XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    createInfo.next = platform_plugin_->GetInstanceCreateExtension();
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();

    strcpy(createInfo.applicationInfo.applicationName, "LakrXR");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    CHECK_XRCMD(xrCreateInstance(&createInfo, &instance_));
    pxr::InitializeGraphicDeivce(instance_);
    xrGetInstanceProcAddr(instance_,"xrGetConfigPICO",
                          reinterpret_cast<PFN_xrVoidFunction *>(&pfn_xr_get_config_pico_));
    xrGetInstanceProcAddr(instance_,"xrSetConfigPICO",
                          reinterpret_cast<PFN_xrVoidFunction *>(&pfn_xr_set_config_pico_));
    xrGetInstanceProcAddr(instance_,"xrResetSensorPICO",
                          reinterpret_cast<PFN_xrVoidFunction *>(&pfn_xr_reset_sensor_pico_));
    LogInstanceInfo();
}

void OpenxrContext::LogInstanceInfo() {
    CHECK(instance_ != XR_NULL_HANDLE);

    XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
    CHECK_XRCMD(xrGetInstanceProperties(instance_, &instanceProperties));

    Log::Write(Log::Level::Info, Fmt("Instance RuntimeName=%s RuntimeVersion=%s", instanceProperties.runtimeName,
                                     GetXrVersionString(instanceProperties.runtimeVersion).c_str()));
}

void OpenxrContext::LogViewConfigurations() {
    CHECK(instance_ != XR_NULL_HANDLE);
    CHECK(system_id_ != XR_NULL_SYSTEM_ID);

    uint32_t viewConfigTypeCount;
    CHECK_XRCMD(xrEnumerateViewConfigurations(instance_, system_id_, 0, &viewConfigTypeCount, nullptr));
    std::vector<XrViewConfigurationType> viewConfigTypes(viewConfigTypeCount);
    CHECK_XRCMD(xrEnumerateViewConfigurations(instance_, system_id_, viewConfigTypeCount, &viewConfigTypeCount,
                                              viewConfigTypes.data()));
    CHECK((uint32_t)viewConfigTypes.size() == viewConfigTypeCount);

    Log::Write(Log::Level::Info, Fmt("Available View Configuration Types: (%d)", viewConfigTypeCount));
    for (XrViewConfigurationType viewConfigType : viewConfigTypes) {
        Log::Write(Log::Level::Info, Fmt("  View Configuration Type: %s %s", to_string(viewConfigType),
                                            viewConfigType == view_config_type_ ? "(Selected)" : ""));

        XrViewConfigurationProperties viewConfigProperties{XR_TYPE_VIEW_CONFIGURATION_PROPERTIES};
        CHECK_XRCMD(xrGetViewConfigurationProperties(instance_, system_id_, viewConfigType, &viewConfigProperties));

        Log::Write(Log::Level::Info,
                   Fmt("  View configuration FovMutable=%s", viewConfigProperties.fovMutable == XR_TRUE ? "True" : "False"));

        uint32_t viewCount;
        CHECK_XRCMD(xrEnumerateViewConfigurationViews(instance_, system_id_, viewConfigType, 0, &viewCount, nullptr));
        if (viewCount > 0) {
            std::vector<XrViewConfigurationView> views(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
            CHECK_XRCMD(
                    xrEnumerateViewConfigurationViews(instance_, system_id_, viewConfigType, viewCount, &viewCount, views.data()));

            for (uint32_t i = 0; i < views.size(); i++) {
                const XrViewConfigurationView& view = views[i];

                Log::Write(Log::Level::Info, Fmt("    View [%d]: Recommended Width=%d Height=%d SampleCount=%d", i,
                                                    view.recommendedImageRectWidth, view.recommendedImageRectHeight,
                                                    view.recommendedSwapchainSampleCount));
                Log::Write(Log::Level::Info,
                           Fmt("    View [%d]:     Maximum Width=%d Height=%d SampleCount=%d", i, view.maxImageRectWidth,
                               view.maxImageRectHeight, view.maxSwapchainSampleCount));
            }
        } else {
            Log::Write(Log::Level::Error, Fmt("Empty view configuration type"));
        }

        LogEnvironmentBlendMode(viewConfigType);
    }
}

void OpenxrContext::LogEnvironmentBlendMode(XrViewConfigurationType type) {
    CHECK(instance_ != XR_NULL_HANDLE);
    CHECK(system_id_ != 0);

    uint32_t count;
    CHECK_XRCMD(xrEnumerateEnvironmentBlendModes(instance_, system_id_, type, 0, &count, nullptr));
    CHECK(count > 0);

    Log::Write(Log::Level::Info, Fmt("Available Environment Blend Mode count : (%d)", count));

    std::vector<XrEnvironmentBlendMode> blendModes(count);
    CHECK_XRCMD(xrEnumerateEnvironmentBlendModes(instance_, system_id_, type, count, &count, blendModes.data()));

    bool blendModeFound = false;
    for (XrEnvironmentBlendMode mode : blendModes) {
        const bool blendModeMatch = (mode == environment_blend_mode_);
        Log::Write(Log::Level::Info,
                   Fmt("Environment Blend Mode (%s) : %s", to_string(mode), blendModeMatch ? "(Selected)" : ""));
        blendModeFound |= blendModeMatch;
    }
    CHECK(blendModeFound);
}

void OpenxrContext::LogReferenceSpaces() {
    CHECK(session_ != XR_NULL_HANDLE);

    uint32_t spaceCount;
    CHECK_XRCMD(xrEnumerateReferenceSpaces(session_, 0, &spaceCount, nullptr));
    std::vector<XrReferenceSpaceType> spaces(spaceCount);
    CHECK_XRCMD(xrEnumerateReferenceSpaces(session_, spaceCount, &spaceCount, spaces.data()));

    Log::Write(Log::Level::Info, Fmt("Available reference spaces: %d", spaceCount));
    for (XrReferenceSpaceType space : spaces) {
        Log::Write(Log::Level::Verbose, Fmt("  Name: %s", to_string(space)));
    }
}

void OpenxrContext::InitializeSystem() {
    CHECK(instance_ != XR_NULL_HANDLE);
    CHECK(system_id_ == XR_NULL_SYSTEM_ID);

    form_factor_ = GetXrFormFactor(options_->FormFactor);
    view_config_type_ = GetXrViewConfigurationType(options_->ViewConfiguration);
    environment_blend_mode_ = GetXrEnvironmentBlendMode(options_->EnvironmentBlendMode);

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = form_factor_;
    CHECK_XRCMD(xrGetSystem(instance_, &systemInfo, &system_id_));

    Log::Write(Log::Level::Verbose, Fmt("Using system %d for form factor %s", system_id_, to_string(form_factor_)));
    CHECK(instance_ != XR_NULL_HANDLE);
    CHECK(system_id_ != XR_NULL_SYSTEM_ID);

    LogViewConfigurations();

    // The graphics API can initialize the graphics device now that the systemId and instance
    // handle are available.
    graphics_plugin_->InitializeDevice(instance_, system_id_);
}

void OpenxrContext::InitializeSession() {
    CHECK(instance_ != XR_NULL_HANDLE);
    CHECK(session_ == XR_NULL_HANDLE);

    {
        Log::Write(Log::Level::Verbose, Fmt("Creating session..."));

        XrSessionCreateInfo createInfo{XR_TYPE_SESSION_CREATE_INFO};
        createInfo.next = graphics_plugin_->GetGraphicsBinding();
        createInfo.systemId = system_id_;
        CHECK_XRCMD(xrCreateSession(instance_, &createInfo, &session_));
    }

    //set eye level
    pfn_xr_set_config_pico_(session_, TRACKING_ORIGIN, "0");

    LogReferenceSpaces();
    input_.InitializeActions(instance_, session_);

    {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(options_->AppSpace);
        CHECK_XRCMD(xrCreateReferenceSpace(session_, &referenceSpaceCreateInfo, &app_space_));
    }
}

void OpenxrContext::CreateSwapchains() {
    CHECK(session_ != XR_NULL_HANDLE);
    CHECK(config_views_.empty());

    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{XR_TYPE_SYSTEM_PROPERTIES};
    CHECK_XRCMD(xrGetSystemProperties(instance_, system_id_, &systemProperties));

    // Log system properties.
    Log::Write(Log::Level::Info,
               Fmt("System Properties: Name=%s VendorId=%d", systemProperties.systemName, systemProperties.vendorId));
    Log::Write(Log::Level::Info, Fmt("System Graphics Properties: MaxWidth=%d MaxHeight=%d MaxLayers=%d",
                                     systemProperties.graphicsProperties.maxSwapchainImageWidth,
                                     systemProperties.graphicsProperties.maxSwapchainImageHeight,
                                     systemProperties.graphicsProperties.maxLayerCount));
    Log::Write(Log::Level::Info, Fmt("System Tracking Properties: OrientationTracking=%s PositionTracking=%s",
                                     systemProperties.trackingProperties.orientationTracking == XR_TRUE ? "True" : "False",
                                     systemProperties.trackingProperties.positionTracking == XR_TRUE ? "True" : "False"));

    // Note: No other view configurations exist at the time this code was written. If this
    // condition is not met, the project will need to be audited to see how support should be
    // added.
    CHECK_MSG(view_config_type_ == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, "Unsupported view configuration type");

    // Query and cache view configuration views.
    uint32_t viewCount;
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(instance_, system_id_, view_config_type_, 0, &viewCount, nullptr));
    config_views_.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(instance_, system_id_, view_config_type_, viewCount, &viewCount,
                                                  config_views_.data()));

    // Create and cache view buffer for xrLocateViews later.
    views_.resize(viewCount, {XR_TYPE_VIEW});

    // Create the swapchain and get the images.
    if (viewCount > 0) {
        // Select a swapchain format.
        uint32_t swapchainFormatCount;
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session_, 0, &swapchainFormatCount, nullptr));
        std::vector<int64_t> swapchainFormats(swapchainFormatCount);
        CHECK_XRCMD(xrEnumerateSwapchainFormats(session_, (uint32_t)swapchainFormats.size(), &swapchainFormatCount,
                                                swapchainFormats.data()));
        CHECK(swapchainFormatCount == swapchainFormats.size());
        color_swapchain_format_ = graphics_plugin_->SelectColorSwapchainFormat(swapchainFormats);

        // Print swapchain formats and the selected one.
        {
            std::string swapchainFormatsString;
            for (int64_t format : swapchainFormats) {
                const bool selected = format == color_swapchain_format_;
                swapchainFormatsString += " ";
                if (selected) {
                    swapchainFormatsString += "[";
                }
                swapchainFormatsString += std::to_string(format);
                if (selected) {
                    swapchainFormatsString += "]";
                }
            }
            Log::Write(Log::Level::Verbose, Fmt("Swapchain Formats: %s", swapchainFormatsString.c_str()));
        }

        assert(viewCount == ovrMaxNumEyes);

        for (int eye = 0; eye < ovrMaxNumEyes; eye++) {

            // TODO setup res
            config_views_[eye].recommendedImageRectWidth = 3644 / 2;
            config_views_[eye].recommendedImageRectHeight = 1920;

//                                  GL_SRGB8_ALPHA8,
//                                  GL_RGBA,
//                                  GL_RGBA8
            frame_buffer_[eye].Create(session_,
                                      color_swapchain_format_,
                                      config_views_[eye].recommendedImageRectWidth,
                                      config_views_[eye].recommendedImageRectHeight,
                                      NUM_MULTI_SAMPLES);
        }
    }
}

const XrEventDataBaseHeader *OpenxrContext::TryReadNextEvent() {
    // It is sufficient to clear the just the XrEventDataBuffer header to
    // XR_TYPE_EVENT_DATA_BUFFER
    XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&event_data_buffer_);
    *baseHeader = {XR_TYPE_EVENT_DATA_BUFFER};
    const XrResult xr = xrPollEvent(instance_, &event_data_buffer_);
    if (xr == XR_SUCCESS) {
        if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST) {
            const XrEventDataEventsLost* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
            Log::Write(Log::Level::Warning, Fmt("%d events lost", eventsLost));
        }

        return baseHeader;
    }
    if (xr == XR_EVENT_UNAVAILABLE) {
        return nullptr;
    }
    THROW_XR(xr, "xrPollEvent");
}

void OpenxrContext::PollEvents(bool *exitRenderLoop, bool *requestRestart) {
    *exitRenderLoop = *requestRestart = false;

    // Process all pending messages.
    while (const XrEventDataBaseHeader* event = TryReadNextEvent()) {
        switch (event->type) {
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                const auto& instanceLossPending = *reinterpret_cast<const XrEventDataInstanceLossPending*>(event);
                Log::Write(Log::Level::Warning, Fmt("XrEventDataInstanceLossPending by %lld", instanceLossPending.lossTime));
                *exitRenderLoop = true;
                *requestRestart = true;
                return;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                auto sessionStateChangedEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(event);
                HandleSessionStateChangedEvent(sessionStateChangedEvent, exitRenderLoop, requestRestart);
                break;
            }
            case XR_TYPE_EVENT_CONTROLLER_STATE_CHANGED: {
                auto eventDataPerfSettingsEXT = *reinterpret_cast<const XrControllerEventChanged *>(event);
                Log::Write(Log::Level::Info, Fmt("controller event callback controller %d, status %d  eventtype %d",
                                                 eventDataPerfSettingsEXT.controller,eventDataPerfSettingsEXT.status,eventDataPerfSettingsEXT.eventtype));
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                LogActionSourceName(input_.grabAction, "Grab");
                LogActionSourceName(input_.quitAction, "Quit");
                LogActionSourceName(input_.poseAction, "Pose");
                LogActionSourceName(input_.vibrateAction, "Vibrate");
                break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
            default: {
                Log::Write(Log::Level::Verbose, Fmt("Ignoring event type %d", event->type));
                break;
            }
        }
    }
}

void OpenxrContext::HandleSessionStateChangedEvent(
        const XrEventDataSessionStateChanged &stateChangedEvent, bool *exitRenderLoop,
        bool *requestRestart) {
    const XrSessionState oldState = session_state_;
    session_state_ = stateChangedEvent.state;

    Log::Write(Log::Level::Info, Fmt("XrEventDataSessionStateChanged: state %s->%s session=%lld time=%lld", to_string(oldState),
                                     to_string(session_state_), stateChangedEvent.session, stateChangedEvent.time));

    if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != session_)) {
        Log::Write(Log::Level::Error, "XrEventDataSessionStateChanged for unknown session");
        return;
    }

    switch (session_state_) {
        case XR_SESSION_STATE_READY: {
            CHECK(session_ != XR_NULL_HANDLE);
            XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = view_config_type_;
            CHECK_XRCMD(xrBeginSession(session_, &sessionBeginInfo));
            session_running_ = true;
            break;
        }
        case XR_SESSION_STATE_STOPPING: {
            CHECK(session_ != XR_NULL_HANDLE);
            session_running_ = false;
            CHECK_XRCMD(xrEndSession(session_))
            break;
        }
        case XR_SESSION_STATE_EXITING: {
            *exitRenderLoop = true;
            // Do not attempt to restart because user closed this session.
            *requestRestart = false;
            break;
        }
        case XR_SESSION_STATE_LOSS_PENDING: {
            *exitRenderLoop = true;
            // Poll for a new instance.
            *requestRestart = true;
            break;
        }
        default:
            break;
    }
}

void OpenxrContext::LogActionSourceName(XrAction action, const std::string& actionName) const {
    XrBoundSourcesForActionEnumerateInfo getInfo = {XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO};
    getInfo.action = action;
    uint32_t pathCount = 0;
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(session_, &getInfo, 0, &pathCount, nullptr));
    std::vector<XrPath> paths(pathCount);
    CHECK_XRCMD(xrEnumerateBoundSourcesForAction(session_, &getInfo, uint32_t(paths.size()), &pathCount, paths.data()));

    std::string sourceName;
    for (uint32_t i = 0; i < pathCount; ++i) {
        constexpr XrInputSourceLocalizedNameFlags all = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                                                        XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

        XrInputSourceLocalizedNameGetInfo nameInfo = {XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO};
        nameInfo.sourcePath = paths[i];
        nameInfo.whichComponents = all;

        uint32_t size = 0;
        CHECK_XRCMD(xrGetInputSourceLocalizedName(session_, &nameInfo, 0, &size, nullptr));
        if (size < 1) {
            continue;
        }
        std::vector<char> grabSource(size);
        CHECK_XRCMD(xrGetInputSourceLocalizedName(session_, &nameInfo, uint32_t(grabSource.size()), &size, grabSource.data()));
        if (!sourceName.empty()) {
            sourceName += " and ";
        }
        sourceName += "'";
        sourceName += std::string(grabSource.data(), size - 1);
        sourceName += "'";
    }

    Log::Write(Log::Level::Info,
               Fmt("%s action is bound to %s", actionName.c_str(), ((!sourceName.empty()) ? sourceName.c_str() : "nothing")));
}

void OpenxrContext::ResetSensor(const XrResetSensorOption& option) {
    if (pfn_xr_reset_sensor_pico_ != nullptr && session_ != XR_NULL_HANDLE) {
        pfn_xr_reset_sensor_pico_(session_, option);
    }
}

void OpenxrContext::GetConfig(enum ConfigsEXT configIndex, float *configData) {
    if (pfn_xr_get_config_pico_ != nullptr && session_ != XR_NULL_HANDLE) {
        pfn_xr_get_config_pico_(session_, configIndex, configData);
    }
}

void OpenxrContext::PollActions() {
    bool ret = false;
    input_.handActive = {XR_FALSE, XR_FALSE};

    // Sync actions
    const XrActiveActionSet activeActionSet{input_.actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(session_, &syncInfo));

    // Get pose and grab action state and start haptic vibrate when hand is 90% squeezed.
    for (auto hand : {Side::LEFT, Side::RIGHT}) {
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = input_.poseAction;
        XrActionStatePose poseState{XR_TYPE_ACTION_STATE_POSE};
        CHECK_XRCMD(xrGetActionStatePose(session_, &getInfo, &poseState));
        input_.handActive[hand] = poseState.isActive;
    }
}

float OpenxrContext::GetFPS() {
    float fps = 0;
    ConfigsEXT configsExt = ConfigsEXT::DISPLAY_REFRESH_RATE;
    GetConfig(configsExt, &fps);
    return fps;
}

void OpenxrContext::SetFPS(float fps) {
    char buffer[50] = {};
    sprintf(buffer, "%f", fps);
    if (pfn_xr_set_config_pico_ && session_) {
        XrResult result = pfn_xr_set_config_pico_(session_, ConfigsSetEXT::SET_DISPLAY_RATE, buffer);
        LOGV("set fps %f result %d", fps, result);
    }
}
