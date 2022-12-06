//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <utils.h>
#include "oxr_utils.h"
#include "openxr_context.h"
#include "openxr/openxr_oculus_helpers.h"
#include "log.h"

#define GL_FRAMEBUFFER_SRGB               0x8DB9

namespace oxr {
// quest
// Runtime Oculus: Version : 46.261.0
// System Properties: Name=Oculus Quest VendorId=2833
// System Graphics Properties: MaxWidth=8192 MaxHeight=8192 MaxLayers=32
// quest2
// System Properties: Name=Oculus Quest2 VendorId=2833
// Runtime Oculus: Version : 46.261.0
// System Graphics Properties: MaxWidth=8192 MaxHeight=8192 MaxLayers=32
const std::string OpenxrContext::SYSTEM_NAME_QUEST = "Oculus Quest";
const std::string OpenxrContext::SYSTEM_NAME_QUEST2 = "Oculus Quest2";

OpenxrContext::OpenxrContext() = default;

OpenxrContext::~OpenxrContext() = default;

void OpenxrContext::CreateInstance() {
    // Check that the extensions required are present.
    const char* const requiredExtensionNames[] = {
            XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME,
            XR_EXT_PERFORMANCE_SETTINGS_EXTENSION_NAME,
            XR_KHR_ANDROID_THREAD_SETTINGS_EXTENSION_NAME,
            XR_KHR_COMPOSITION_LAYER_CUBE_EXTENSION_NAME,
            XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME,
            XR_KHR_COMPOSITION_LAYER_EQUIRECT2_EXTENSION_NAME,
            XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME,
            XR_FB_COLOR_SPACE_EXTENSION_NAME,
            XR_FB_SWAPCHAIN_UPDATE_STATE_EXTENSION_NAME,
            XR_FB_SWAPCHAIN_UPDATE_STATE_OPENGL_ES_EXTENSION_NAME,
            XR_FB_FOVEATION_EXTENSION_NAME,
            XR_FB_FOVEATION_CONFIGURATION_EXTENSION_NAME};
    const uint32_t numRequiredExtensions =
            sizeof(requiredExtensionNames) / sizeof(requiredExtensionNames[0]);

    // Check the list of required extensions against what is supported by the runtime.
    {
        XrResult result;
        PFN_xrEnumerateInstanceExtensionProperties xrEnumerateInstanceExtensionProperties;
        result = xrGetInstanceProcAddr(
                XR_NULL_HANDLE,
                "xrEnumerateInstanceExtensionProperties",
                (PFN_xrVoidFunction*)&xrEnumerateInstanceExtensionProperties);
        if (result != XR_SUCCESS) {
            ALOGE("Failed to get xrEnumerateInstanceExtensionProperties function pointer.");
            exit(1);
        }

        uint32_t numInputExtensions = 0;
        uint32_t numOutputExtensions = 0;
        xrEnumerateInstanceExtensionProperties(
                NULL, numInputExtensions, &numOutputExtensions, NULL);
        ALOGV("xrEnumerateInstanceExtensionProperties found %u extension(s).", numOutputExtensions);

        numInputExtensions = numOutputExtensions;

        XrExtensionProperties* extensionProperties =
                (XrExtensionProperties*)malloc(numOutputExtensions * sizeof(XrExtensionProperties));

        for (uint32_t i = 0; i < numOutputExtensions; i++) {
            extensionProperties[i].type = XR_TYPE_EXTENSION_PROPERTIES;
            extensionProperties[i].next = NULL;
        }

        xrEnumerateInstanceExtensionProperties(
                NULL, numInputExtensions, &numOutputExtensions, extensionProperties);
        for (uint32_t i = 0; i < numOutputExtensions; i++) {
            ALOGV("Extension #%d = '%s'.", i, extensionProperties[i].extensionName);
        }

        for (uint32_t i = 0; i < numRequiredExtensions; i++) {
            bool found = false;
            for (uint32_t j = 0; j < numOutputExtensions; j++) {
                if (!strcmp(requiredExtensionNames[i], extensionProperties[j].extensionName)) {
                    ALOGV("Found required extension %s", requiredExtensionNames[i]);
                    found = true;
                    break;
                }
            }
            if (!found) {
                ALOGE("Failed to find required extension %s", requiredExtensionNames[i]);
                exit(1);
            }
        }

        free(extensionProperties);
    }

    // Create the OpenXR instance.
    XrApplicationInfo appInfo;
    memset(&appInfo, 0, sizeof(appInfo));
    strcpy(appInfo.applicationName, "LarkXR-Oculus");
    appInfo.applicationVersion = 0;
    strcpy(appInfo.engineName, "PxyGL");
    appInfo.engineVersion = 0;
    appInfo.apiVersion = XR_CURRENT_API_VERSION;

    XrInstanceCreateInfo instanceCreateInfo;
    memset(&instanceCreateInfo, 0, sizeof(instanceCreateInfo));
    instanceCreateInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.next = NULL;
    instanceCreateInfo.createFlags = 0;
    instanceCreateInfo.applicationInfo = appInfo;
    instanceCreateInfo.enabledApiLayerCount = 0;
    instanceCreateInfo.enabledApiLayerNames = NULL;
    instanceCreateInfo.enabledExtensionCount = numRequiredExtensions;
    instanceCreateInfo.enabledExtensionNames = requiredExtensionNames;

    XrResult initResult;
    initResult = xrCreateInstance(&instanceCreateInfo, &instance_);
    if (initResult != XR_SUCCESS) {
        ALOGE("Failed to create XR instance: %d.", initResult);
        exit(1);
    }

    XrInstanceProperties instanceInfo;
    instanceInfo.type = XR_TYPE_INSTANCE_PROPERTIES;
    instanceInfo.next = NULL;
    xrGetInstanceProperties(instance_, &instanceInfo);
    ALOGV(
            "Runtime %s: Version : %u.%u.%u",
            instanceInfo.runtimeName,
            XR_VERSION_MAJOR(instanceInfo.runtimeVersion),
            XR_VERSION_MINOR(instanceInfo.runtimeVersion),
            XR_VERSION_PATCH(instanceInfo.runtimeVersion));
}

void OpenxrContext::InitializeSystem() {
    if (instance_ == nullptr) {
        ALOGE("must create insance before init system.");
        return;
    }

    XrSystemGetInfo systemGetInfo;
    memset(&systemGetInfo, 0, sizeof(systemGetInfo));
    systemGetInfo.type = XR_TYPE_SYSTEM_GET_INFO;
    systemGetInfo.next = NULL;
    systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrSystemId systemId;
    XrResult initResult = xrGetSystem(instance_, &systemGetInfo, &systemId);
    if (initResult != XR_SUCCESS) {
        ALOGE("Failed to get system.");
        exit(1);
    }

    XrSystemColorSpacePropertiesFB colorSpacePropertiesFB = {};
    colorSpacePropertiesFB.type = XR_TYPE_SYSTEM_COLOR_SPACE_PROPERTIES_FB;

    XrSystemProperties systemProperties = {};
    systemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;
    systemProperties.next = &colorSpacePropertiesFB;
    OXR(xrGetSystemProperties(instance_, systemId, &systemProperties));

    ALOGV(
            "System Properties: Name=%s VendorId=%x",
            systemProperties.systemName,
            systemProperties.vendorId);
    ALOGV(
            "System Graphics Properties: MaxWidth=%d MaxHeight=%d MaxLayers=%d",
            systemProperties.graphicsProperties.maxSwapchainImageWidth,
            systemProperties.graphicsProperties.maxSwapchainImageHeight,
            systemProperties.graphicsProperties.maxLayerCount);
    ALOGV(
            "System Tracking Properties: OrientationTracking=%s PositionTracking=%s",
            systemProperties.trackingProperties.orientationTracking ? "True" : "False",
            systemProperties.trackingProperties.positionTracking ? "True" : "False");

    ALOGV("System Color Space Properties: colorspace=%d", colorSpacePropertiesFB.colorSpace);

    // quest
    // Runtime Oculus: Version : 46.261.0
    // System Properties: Name=Oculus Quest VendorId=2833
    // System Graphics Properties: MaxWidth=8192 MaxHeight=8192 MaxLayers=32

    // System Properties: Name=Oculus Quest2 VendorId=2833
    // Runtime Oculus: Version : 46.261.0
    // System Graphics Properties: MaxWidth=8192 MaxHeight=8192 MaxLayers=32

    system_properties_ = systemProperties;

    assert(ovrMaxLayerCount <= systemProperties.graphicsProperties.maxLayerCount);

    cpu_level_ = CPU_LEVEL;
    gpu_level_ = GPU_LEVEL;
    main_thread_tid_ = gettid();
    system_id_ = systemId;
}

void OpenxrContext::InitializeSession() {
    if (instance_ == nullptr || system_id_ == 0) {
        ALOGE("must create insance and system before init session.");
        return;
    }

    graphics_plugin_ = std::make_unique<GraphicsDeviceAndroid>();
    // The graphics API can initialize the graphics device now that the systemId and instance
    // handle are available.
    graphics_plugin_->InitializeDevice(instance_, system_id_);

    XrSessionCreateInfo sessionCreateInfo = {};
    memset(&sessionCreateInfo, 0, sizeof(sessionCreateInfo));
    sessionCreateInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionCreateInfo.next = graphics_plugin_->graphics_binding();
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = system_id_;

    XrResult initResult = xrCreateSession(instance_, &sessionCreateInfo, &session_);
    if (initResult != XR_SUCCESS) {
        ALOGE("Failed to create XR session: %d.", initResult);
        exit(1);
    }

    // setup framerates
    // support framerates
    XrResult result = xrGetInstanceProcAddr(
            instance_,
            "xrEnumerateDisplayRefreshRatesFB",
            (PFN_xrVoidFunction*)(&pfnxrEnumerateDisplayRefreshRatesFB));
    // get fps
    result = xrGetInstanceProcAddr(
            instance_,
            "xrGetDisplayRefreshRateFB",
            (PFN_xrVoidFunction*)(&pfnGetDisplayRefreshRate));
    // set fps
    result = xrGetInstanceProcAddr(
            instance_,
            "xrRequestDisplayRefreshRateFB",
            (PFN_xrVoidFunction*)(&pfnRequestDisplayRefreshRate));
    // support color spaces
    result = xrGetInstanceProcAddr(
            instance_,
            "xrEnumerateColorSpacesFB",
            (PFN_xrVoidFunction*)(&pfnxrEnumerateColorSpacesFB));
    // set color space
    result = xrGetInstanceProcAddr(
            instance_, "xrSetColorSpaceFB", (PFN_xrVoidFunction*)(&pfnxrSetColorSpaceFB));

    OXR(xrGetInstanceProcAddr(
            instance_,
            "xrCreateFoveationProfileFB",
            (PFN_xrVoidFunction*)(&pfnCreateFoveationProfileFB)));

    OXR(xrGetInstanceProcAddr(
            instance_,
            "xrDestroyFoveationProfileFB",
            (PFN_xrVoidFunction*)(&pfnDestroyFoveationProfileFB)));

    OXR(xrGetInstanceProcAddr(
            instance_, "xrUpdateSwapchainFB", (PFN_xrVoidFunction*)(&pfnUpdateSwapchainFB)));

    // log info...
    LogInfos();

    // set color space
    SetColorSpace(XR_COLOR_SPACE_REC2020_FB);
//    SetColorSpace(XR_COLOR_SPACE_ADOBE_RGB_FB);
//    SetColorSpace(XR_COLOR_SPACE_QUEST_FB);
//    SetColorSpace(XR_COLOR_SPACE_UNMANAGED_FB);
    // set fps to default
    SetDisplayRefreshRate(0);
//    SetDisplayRefreshRate(90);

    // setup space
    InitSpace();

    // setup input state
    input_state_.InitializeActions(instance_, session_);
}

void OpenxrContext::CreateSwapchains() {
    // App only supports the primary stereo view config.
    const XrViewConfigurationType supportedViewConfigType =
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    // Enumerate the viewport configurations.
    uint32_t viewportConfigTypeCount = 0;
    OXR(xrEnumerateViewConfigurations(
            instance_, system_id_, 0, &viewportConfigTypeCount, NULL));

    XrViewConfigurationType* viewportConfigurationTypes =
            (XrViewConfigurationType*)malloc(viewportConfigTypeCount * sizeof(XrViewConfigurationType));

    OXR(xrEnumerateViewConfigurations(
            instance_,
            system_id_,
            viewportConfigTypeCount,
            &viewportConfigTypeCount,
            viewportConfigurationTypes));

    ALOGV("Available Viewport Configuration Types: %d", viewportConfigTypeCount);

    for (uint32_t i = 0; i < viewportConfigTypeCount; i++) {
        const XrViewConfigurationType viewportConfigType = viewportConfigurationTypes[i];

        ALOGV(
                "Viewport configuration type %d : %s",
                viewportConfigType,
                viewportConfigType == supportedViewConfigType ? "Selected" : "");

        XrViewConfigurationProperties viewportConfig;
        viewportConfig.type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES;
        OXR(xrGetViewConfigurationProperties(
                instance_, system_id_, viewportConfigType, &viewportConfig));
        ALOGV(
                "FovMutable=%s ConfigurationType %d",
                viewportConfig.fovMutable ? "true" : "false",
                viewportConfig.viewConfigurationType);

        uint32_t viewCount;
        OXR(xrEnumerateViewConfigurationViews(
                instance_, system_id_, viewportConfigType, 0, &viewCount, NULL));

        if (viewCount > 0) {
            XrViewConfigurationView* elements =
                    (XrViewConfigurationView*)malloc(viewCount * sizeof(XrViewConfigurationView));

            for (uint32_t e = 0; e < viewCount; e++) {
                elements[e].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
                elements[e].next = NULL;
            }

            OXR(xrEnumerateViewConfigurationViews(
                    instance_,
                    system_id_,
                    viewportConfigType,
                    viewCount,
                    &viewCount,
                    elements));

            // Log the view config info for each view type for debugging purposes.
            for (uint32_t e = 0; e < viewCount; e++) {
                const XrViewConfigurationView* element = &elements[e];

                ALOGV(
                        "Viewport [%d]: Recommended Width=%d Height=%d SampleCount=%d",
                        e,
                        element->recommendedImageRectWidth,
                        element->recommendedImageRectHeight,
                        element->recommendedSwapchainSampleCount);

                ALOGV(
                        "Viewport [%d]: Max Width=%d Height=%d SampleCount=%d",
                        e,
                        element->maxImageRectWidth,
                        element->maxImageRectHeight,
                        element->maxSwapchainSampleCount);
            }

            // Cache the view config properties for the selected config type.
            if (viewportConfigType == supportedViewConfigType) {
                assert(viewCount == ovrMaxNumEyes);
                for (uint32_t e = 0; e < viewCount; e++) {
                    view_configuration_view_[e] = elements[e];
                }
            }

            free(elements);
        } else {
            ALOGE("Empty viewport configuration type: %d", viewCount);
        }
    }

    free(viewportConfigurationTypes);

    // Get the viewport configuration info for the chosen viewport configuration type.
    viewport_config_.type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES;

    OXR(xrGetViewConfigurationProperties(
            instance_, system_id_, supportedViewConfigType, &viewport_config_));

    // hack color space
    // disable GL_FRAMEBUFFER_SRGB and use GL_SRGB8_ALPHA8
    // https://forums.oculusvr.com/t5/OpenXR-Development/sRGB-RGB-giving-washed-out-bright-image/m-p/957475
    glDisable(GL_FRAMEBUFFER_SRGB);

    for (int eye = 0; eye < ovrMaxNumEyes; eye++) {
        // TODO config render width
        // Screen Refresh Rate
        // Oculus Quest 2 72 Hz (90Hz coming soon)
        // Oculus Quest   72 Hz

        // Per Eye Display Resolution
        // Oculus Quest 2 1832x1920
        // Oculus Quest   1440x1600
        if (OpenxrContext::SYSTEM_NAME_QUEST.compare(system_properties().systemName) != -1) {
            view_configuration_view_[eye].recommendedImageRectWidth = 1440;
            view_configuration_view_[eye].recommendedImageRectHeight = 1660;
        } else {
            view_configuration_view_[eye].recommendedImageRectWidth = 1832;
            view_configuration_view_[eye].recommendedImageRectHeight = 1920;
        }


//                                  GL_SRGB8_ALPHA8,
//                                  GL_RGBA,
//                                  GL_RGBA8
        frame_buffer_[eye].Create(session_,
                                  GL_SRGB8_ALPHA8,
                                  view_configuration_view_[eye].recommendedImageRectWidth,
                                  view_configuration_view_[eye].recommendedImageRectHeight,
                                  NUM_MULTI_SAMPLES);
    }

    // init views
    // Create and cache view buffer for xrLocateViews later.
    // TODO get view count
    views_.resize(ovrMaxNumEyes, {XR_TYPE_VIEW});

    // Test set fov.
//    SetFoveation(XR_FOVEATION_LEVEL_NONE_FB,
//                 0,
//                 XR_FOVEATION_DYNAMIC_DISABLED_FB);
}

void OpenxrContext::InitSpace() {
    bool stageSupported = false;

    uint32_t numOutputSpaces = 0;
    OXR(xrEnumerateReferenceSpaces(session_, 0, &numOutputSpaces, NULL));

    XrReferenceSpaceType* referenceSpaces =
            (XrReferenceSpaceType*)malloc(numOutputSpaces * sizeof(XrReferenceSpaceType));

    OXR(xrEnumerateReferenceSpaces(
            session_, numOutputSpaces, &numOutputSpaces, referenceSpaces));

    for (uint32_t i = 0; i < numOutputSpaces; i++) {
        if (referenceSpaces[i] == XR_REFERENCE_SPACE_TYPE_STAGE) {
            stageSupported = true;
            break;
        }
    }

    free(referenceSpaces);

    // Create a space to the first path
    XrReferenceSpaceCreateInfo spaceCreateInfo = {};
    spaceCreateInfo.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
    spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    spaceCreateInfo.poseInReferenceSpace.orientation.w = 1.0f;

    // head space
    OXR(xrCreateReferenceSpace(session_, &spaceCreateInfo, &head_space_));

    // local space
     spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
     OXR(xrCreateReferenceSpace(session_, &spaceCreateInfo, &local_space_));

    // Create a default stage space to use if SPACE_TYPE_STAGE is not
    // supported, or calls to xrGetReferenceSpaceBoundsRect fail.

    if (stageSupported) {
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        spaceCreateInfo.poseInReferenceSpace.position.y = 0.0f;
        OXR(xrCreateReferenceSpace(session_, &spaceCreateInfo, &app_space_));
        ALOGV("Created stage space");
    } else {
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        spaceCreateInfo.poseInReferenceSpace.position.y = -1.6750f;
        OXR(xrCreateReferenceSpace(session_, &spaceCreateInfo, &app_space_));
        ALOGV("Created fake stage space from local space with offset");
    }
}

void OpenxrContext::LogInfos() {
    // Log available layers.
    {
        XrResult result;

        PFN_xrEnumerateApiLayerProperties xrEnumerateApiLayerProperties;
        OXR(result = xrGetInstanceProcAddr(
                XR_NULL_HANDLE,
                "xrEnumerateApiLayerProperties",
                (PFN_xrVoidFunction*)&xrEnumerateApiLayerProperties));
        if (result != XR_SUCCESS) {
            ALOGE("Failed to get xrEnumerateApiLayerProperties function pointer.");
            exit(1);
        }

        uint32_t numInputLayers = 0;
        uint32_t numOutputLayers = 0;
        OXR(xrEnumerateApiLayerProperties(numInputLayers, &numOutputLayers, NULL));

        numInputLayers = numOutputLayers;

        XrApiLayerProperties* layerProperties =
                (XrApiLayerProperties*)malloc(numOutputLayers * sizeof(XrApiLayerProperties));

        for (uint32_t i = 0; i < numOutputLayers; i++) {
            layerProperties[i].type = XR_TYPE_API_LAYER_PROPERTIES;
            layerProperties[i].next = NULL;
        }

        OXR(xrEnumerateApiLayerProperties(numInputLayers, &numOutputLayers, layerProperties));

        for (uint32_t i = 0; i < numOutputLayers; i++) {
            ALOGV("Found layer %s", layerProperties[i].layerName);
        }

        free(layerProperties);
    }

    uint32_t colorSpaceCountOutput = 0;
    pfnxrEnumerateColorSpacesFB(session_, 0, &colorSpaceCountOutput, NULL);

    XrColorSpaceFB* colorSpaces =
            (XrColorSpaceFB*)malloc(colorSpaceCountOutput * sizeof(XrColorSpaceFB));

    pfnxrEnumerateColorSpacesFB(
            session_, colorSpaceCountOutput, &colorSpaceCountOutput, colorSpaces);
    ALOGV("Supported ColorSpaces:");

    for (uint32_t i = 0; i < colorSpaceCountOutput; i++) {
        ALOGV("%d:%d", i, colorSpaces[i]);
    }
    free(colorSpaces);

    uint32_t support_num;
    // Get the supported display refresh rates for the system.
    pfnxrEnumerateDisplayRefreshRatesFB(
            session_, 0, &support_num, nullptr);

    support_display_refresh_rates_.resize(support_num);

    pfnxrEnumerateDisplayRefreshRatesFB(
            session_,
            support_num,
            &support_num,
            support_display_refresh_rates_.data());
    ALOGV("Supported Refresh Rates:");
    for (uint32_t i = 0; i < support_display_refresh_rates_.size(); i++) {
        ALOGV("%d:%f", i, support_display_refresh_rates_[i]);
    }

    ALOGV("Current System Display Refresh Rate: %f", GetCurrentDisplayRefreshRate());
}

float OpenxrContext::GetCurrentDisplayRefreshRate() {
    if (!session_ || !pfnGetDisplayRefreshRate) {
        LOGE("SetDisplayRefreshRate failed session not ready");
        return 0;
    }
    float currentDisplayRefreshRate = 0.0f;
    pfnGetDisplayRefreshRate(session_, &currentDisplayRefreshRate);
    return currentDisplayRefreshRate;
}

void OpenxrContext::SetDisplayRefreshRate(float fps) {
    if (!session_ || !pfnRequestDisplayRefreshRate) {
        LOGE("SetDisplayRefreshRate failed session not ready");
        return;
    }
    pfnRequestDisplayRefreshRate(session_, fps);
}
void OpenxrContext::SetColorSpace(XrColorSpaceFB colorSpaceFB) {
    if (!session_ || !pfnxrSetColorSpaceFB) {
        LOGE("SetColorSpace failed session not ready");
        return;
    }
    pfnxrSetColorSpaceFB(session_, colorSpaceFB);
}

void OpenxrContext::PollEvents() {
    XrEventDataBuffer eventDataBuffer = {};

    // Poll for events
    for (;;) {
        XrEventDataBaseHeader* baseEventHeader = (XrEventDataBaseHeader*)(&eventDataBuffer);
        baseEventHeader->type = XR_TYPE_EVENT_DATA_BUFFER;
        baseEventHeader->next = NULL;
        XrResult r;
        OXR(r = xrPollEvent(instance_, &eventDataBuffer));
        if (r != XR_SUCCESS) {
            break;
        }

        switch (baseEventHeader->type) {
            case XR_TYPE_EVENT_DATA_EVENTS_LOST:
                ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_EVENTS_LOST event");
                break;
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                const XrEventDataInstanceLossPending* instance_loss_pending_event =
                        (XrEventDataInstanceLossPending*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING event: time %f",
                        FromXrTime(instance_loss_pending_event->lossTime));
            } break;
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED event");
                break;
            case XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT: {
                const XrEventDataPerfSettingsEXT* perf_settings_event =
                        (XrEventDataPerfSettingsEXT*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT event: type %d subdomain %d : level %d -> level %d",
                        perf_settings_event->type,
                        perf_settings_event->subDomain,
                        perf_settings_event->fromLevel,
                        perf_settings_event->toLevel);
            } break;
            case XR_TYPE_EVENT_DATA_DISPLAY_REFRESH_RATE_CHANGED_FB: {
                const XrEventDataDisplayRefreshRateChangedFB* refresh_rate_changed_event =
                        (XrEventDataDisplayRefreshRateChangedFB*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_DISPLAY_REFRESH_RATE_CHANGED_FB event: fromRate %f -> toRate %f",
                        refresh_rate_changed_event->fromDisplayRefreshRate,
                        refresh_rate_changed_event->toDisplayRefreshRate);
            } break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                XrEventDataReferenceSpaceChangePending* ref_space_change_event =
                        (XrEventDataReferenceSpaceChangePending*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING event: changed space: %d for session %p at time %f",
                        ref_space_change_event->referenceSpaceType,
                        (void*)ref_space_change_event->session,
                        FromXrTime(ref_space_change_event->changeTime));
            } break;
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                const XrEventDataSessionStateChanged* session_state_changed_event =
                        (XrEventDataSessionStateChanged*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: %d for session %p at time %f",
                        session_state_changed_event->state,
                        (void*)session_state_changed_event->session,
                        FromXrTime(session_state_changed_event->time));

                switch (session_state_changed_event->state) {
                    case XR_SESSION_STATE_FOCUSED:
                        focused_ = true;
                        break;
                    case XR_SESSION_STATE_VISIBLE:
                        focused_ = false;
                        break;
                    case XR_SESSION_STATE_READY:
                    case XR_SESSION_STATE_STOPPING:
                        HandleSessionStateChanges(session_state_changed_event->state);
                        break;
                    default:
                        break;
                }
            } break;
            default:
                ALOGV("xrPollEvent: Unknown event");
                break;
        }
    }
}

void OpenxrContext::PollActions() {

}

void OpenxrContext::HandleSessionStateChanges(XrSessionState state) {
    if (state == XR_SESSION_STATE_READY) {
        // assert(resumed_);
        // assert(native_window_ != NULL);
        // assert(session_active_ == false);

        XrSessionBeginInfo sessionBeginInfo;
        memset(&sessionBeginInfo, 0, sizeof(sessionBeginInfo));
        sessionBeginInfo.type = XR_TYPE_SESSION_BEGIN_INFO;
        sessionBeginInfo.next = NULL;
        sessionBeginInfo.primaryViewConfigurationType = viewport_config_.viewConfigurationType;

        XrResult result;
        OXR(result = xrBeginSession(session_, &sessionBeginInfo));

        session_active_ = (result == XR_SUCCESS);

        // Set session state once we have entered VR mode and have a valid session object.
        if (session_active_) {
            XrPerfSettingsLevelEXT cpuPerfLevel = XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT;
            switch (cpu_level_) {
                case 0:
                    cpuPerfLevel = XR_PERF_SETTINGS_LEVEL_POWER_SAVINGS_EXT;
                    break;
                case 1:
                    cpuPerfLevel = XR_PERF_SETTINGS_LEVEL_SUSTAINED_LOW_EXT;
                    break;
                case 2:
                    cpuPerfLevel = XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT;
                    break;
                case 3:
                    cpuPerfLevel = XR_PERF_SETTINGS_LEVEL_BOOST_EXT;
                    break;
                default:
                    ALOGE("Invalid CPU level %d", cpu_level_);
                    break;
            }

            XrPerfSettingsLevelEXT gpuPerfLevel = XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT;
            switch (gpu_level_) {
                case 0:
                    gpuPerfLevel = XR_PERF_SETTINGS_LEVEL_POWER_SAVINGS_EXT;
                    break;
                case 1:
                    gpuPerfLevel = XR_PERF_SETTINGS_LEVEL_SUSTAINED_LOW_EXT;
                    break;
                case 2:
                    gpuPerfLevel = XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT;
                    break;
                case 3:
                    gpuPerfLevel = XR_PERF_SETTINGS_LEVEL_BOOST_EXT;
                    break;
                default:
                    ALOGE("Invalid GPU level %d", gpu_level_);
                    break;
            }

            PFN_xrPerfSettingsSetPerformanceLevelEXT pfnPerfSettingsSetPerformanceLevelEXT = NULL;
            OXR(xrGetInstanceProcAddr(
                    instance_,
                    "xrPerfSettingsSetPerformanceLevelEXT",
                    (PFN_xrVoidFunction*)(&pfnPerfSettingsSetPerformanceLevelEXT)));

            OXR(pfnPerfSettingsSetPerformanceLevelEXT(
                    session_, XR_PERF_SETTINGS_DOMAIN_CPU_EXT, cpuPerfLevel));
            OXR(pfnPerfSettingsSetPerformanceLevelEXT(
                    session_, XR_PERF_SETTINGS_DOMAIN_GPU_EXT, gpuPerfLevel));

            PFN_xrSetAndroidApplicationThreadKHR pfnSetAndroidApplicationThreadKHR = NULL;
            OXR(xrGetInstanceProcAddr(
                    instance_,
                    "xrSetAndroidApplicationThreadKHR",
                    (PFN_xrVoidFunction*)(&pfnSetAndroidApplicationThreadKHR)));

            OXR(pfnSetAndroidApplicationThreadKHR(
                    session_, XR_ANDROID_THREAD_TYPE_APPLICATION_MAIN_KHR, main_thread_tid_));
            OXR(pfnSetAndroidApplicationThreadKHR(
                    session_, XR_ANDROID_THREAD_TYPE_RENDERER_MAIN_KHR, render_thread_tid_));
        }
    } else if (state == XR_SESSION_STATE_STOPPING) {
        // assert(resumed_ == false);
        // assert(session_active_);

        OXR(xrEndSession(session_));
        session_active_ = false;
    }
}

void OpenxrContext::Destory() {
    input_state_ = {};

    for (auto & i : frame_buffer_) {
        i.Destroy();
    }

    graphics_plugin_->DestroyContext();

    OXR(xrDestroySpace(app_space_));
    app_space_ = XR_NULL_HANDLE;

    OXR(xrDestroySession(session_));
    session_ = XR_NULL_HANDLE;

    OXR(xrDestroyInstance(instance_));
    instance_ = XR_NULL_HANDLE;
}

void OpenxrContext::SetFoveation(XrFoveationLevelFB level, float verticalOffset,
                                 XrFoveationDynamicFB dynamic) {
    for (int eye = 0; eye < ovrMaxNumEyes; eye++) {
        XrFoveationLevelProfileCreateInfoFB levelProfileCreateInfo;
        memset(&levelProfileCreateInfo, 0, sizeof(levelProfileCreateInfo));
        levelProfileCreateInfo.type = XR_TYPE_FOVEATION_LEVEL_PROFILE_CREATE_INFO_FB;
        levelProfileCreateInfo.level = level;
        levelProfileCreateInfo.verticalOffset = verticalOffset;
        levelProfileCreateInfo.dynamic = dynamic;

        XrFoveationProfileCreateInfoFB profileCreateInfo;
        memset(&profileCreateInfo, 0, sizeof(profileCreateInfo));
        profileCreateInfo.type = XR_TYPE_FOVEATION_PROFILE_CREATE_INFO_FB;
        profileCreateInfo.next = &levelProfileCreateInfo;

        XrFoveationProfileFB foveationProfile;

        pfnCreateFoveationProfileFB(session_, &profileCreateInfo, &foveationProfile);

        XrSwapchainStateFoveationFB foveationUpdateState;
        memset(&foveationUpdateState, 0, sizeof(foveationUpdateState));
        foveationUpdateState.type = XR_TYPE_SWAPCHAIN_STATE_FOVEATION_FB;
        foveationUpdateState.profile = foveationProfile;

        pfnUpdateSwapchainFB(
                frame_buffer_[eye].color_swapchain().Handle,
                (XrSwapchainStateBaseHeaderFB*)(&foveationUpdateState));

        pfnDestroyFoveationProfileFB(foveationProfile);
    }
}
}
