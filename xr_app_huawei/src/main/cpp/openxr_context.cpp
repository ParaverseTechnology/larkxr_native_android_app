//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <utils.h>
#include "openxr_context.h"
#include "log.h"

#define GL_FRAMEBUFFER_SRGB               0x8DB9

namespace hxr {

OpenxrContext::OpenxrContext() = default;

OpenxrContext::~OpenxrContext() = default;

void OpenxrContext::CreateInstance(jobject jactivity, JavaVM* jvm) {
    activity_ = jactivity;
    jvm_ = jvm;

    LOGI("XrEnumerate");
    uint32_t apiLayerPropCount;
    int ret = xrEnumerateApiLayerProperties(0, &apiLayerPropCount, nullptr);
    LOGI("EnumerateApiLayerProperties %d",ret);
    std::vector<XrApiLayerProperties> apiProperties;
    apiProperties.resize(apiLayerPropCount, {XR_TYPE_API_LAYER_PROPERTIES});
    ret = xrEnumerateApiLayerProperties(apiLayerPropCount, &apiLayerPropCount, apiProperties.data());
    LOGI("EnumerateApiLayerProperties 2 %d",ret);
    //return;

    uint32_t insExtensionCount;
    ret = xrEnumerateInstanceExtensionProperties(nullptr, 0, &insExtensionCount, nullptr);
    //return;
    LOGI("EnumerateInstanceExtensionProperties %d",ret);
    std::vector<XrExtensionProperties> extProperties;
    extProperties.resize(insExtensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
    xrEnumerateInstanceExtensionProperties(nullptr, insExtensionCount, &insExtensionCount, extProperties.data());
    LOGI("extProperties.size():%d",extProperties.size());
    for(int i = 0; i < extProperties.size(); i++)
    {
        LOGI("ExtProps: %s %d", extProperties[i].extensionName, extProperties[i].extensionVersion);
    }

    LOGI("XrCreateInstance");
    XrInstanceCreateInfoAndroidKHR createInfoAndroid{XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    createInfoAndroid.applicationActivity = (void*)activity_;
    createInfoAndroid.applicationVM = (void*)jvm_;
    std::vector<const char*> extensions;
    extensions.push_back(XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME);
    extensions.push_back(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);

    XrInstanceCreateInfo instanceCreateInfo{XR_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.next = (const void*)&createInfoAndroid;
    instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instanceCreateInfo.enabledExtensionNames = extensions.data();
    strcpy(instanceCreateInfo.applicationInfo.applicationName, "HelloXR");
    instanceCreateInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    ret = xrCreateInstance(&instanceCreateInfo, &instance_);
    LOGI("xrCreateInstance %d",ret);
    //return;
    //Test instanceProperties
    LOGI("before xrGetInstanceProperties");
    XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
    xrGetInstanceProperties(instance_, &instanceProperties);
    LOGI("after demo xrGetInstanceProperties");
}

void OpenxrContext::InitializeSystem() {
    if (instance_ == nullptr) {
        LOGE("must create insance before init system.");
        return;
    }

    LOGI("XrGetSystemId");
    XrFormFactor formFactor{XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};

    XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemInfo.formFactor = formFactor;
    xrGetSystem(instance_, &systemInfo, &system_id_);


    XrSystemProperties systemProperties = {};
    systemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;
    systemProperties.next = nullptr;
    xrGetSystemProperties(instance_, system_id_, &systemProperties);

    LOGV(
            "System Properties: Name=%s VendorId=%x",
            systemProperties.systemName,
            systemProperties.vendorId);
    LOGV(
            "System Graphics Properties: MaxWidth=%d MaxHeight=%d MaxLayers=%d",
            systemProperties.graphicsProperties.maxSwapchainImageWidth,
            systemProperties.graphicsProperties.maxSwapchainImageHeight,
            systemProperties.graphicsProperties.maxLayerCount);
    LOGV(
            "System Tracking Properties: OrientationTracking=%s PositionTracking=%s",
            systemProperties.trackingProperties.orientationTracking ? "True" : "False",
            systemProperties.trackingProperties.positionTracking ? "True" : "False");

    system_properties_ = systemProperties;

}

void OpenxrContext::InitializeSession(ANativeWindow* nativeWindow) {
    if (instance_ == nullptr || system_id_ == 0) {
        LOGE("must create insance and system before init session.");
        return;
    }

    graphics_plugin_ = std::make_unique<GraphicsDeviceAndroid>();
    // The graphics API can initialize the graphics device now that the systemId and instance
    // handle are available.
    graphics_plugin_->InitializeDevice(instance_, system_id_);
    graphics_plugin_->InitEGLSurface(nativeWindow);

    XrSessionCreateInfo sessionCreateInfo = {};
    memset(&sessionCreateInfo, 0, sizeof(sessionCreateInfo));
    sessionCreateInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionCreateInfo.next = graphics_plugin_->graphics_binding();
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = system_id_;

    XrResult initResult = xrCreateSession(instance_, &sessionCreateInfo, &session_);
    if (initResult != XR_SUCCESS) {
        LOGE("Failed to create XR session: %d.", initResult);
        exit(1);
    }



    // log info...
    LogInfos();

    // setup space
    InitSpace();

    // setup input state
    input_state_.InitializeActions(instance_, session_);
}

void OpenxrContext::CreateSwapchains() {
    LOGI("XrGetSystemProps");
    XrSystemProperties properties{XR_TYPE_SYSTEM_PROPERTIES};
    xrGetSystemProperties(instance_, system_id_, &properties);

    uint32_t viewConfigTypeCount;
    xrEnumerateViewConfigurations(instance_, system_id_, 0, &viewConfigTypeCount, nullptr);

    std::vector<XrViewConfigurationType> viewConfigTypes(viewConfigTypeCount);
    xrEnumerateViewConfigurations(instance_, system_id_, viewConfigTypeCount, &viewConfigTypeCount,viewConfigTypes.data());


    LOGI("XrGetViewConfigProps");
    XrViewConfigurationType viewConfigType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    XrViewConfigurationProperties viewConfigProperties{XR_TYPE_VIEW_CONFIGURATION_PROPERTIES};
    int ret = xrGetViewConfigurationProperties(instance_, system_id_, viewConfigType, &viewConfigProperties);
    LOGI("xrGetViewConfigurationProperties ret:%d",ret);

    LOGI("XrEnumViewConfigViews");
    uint32_t viewCount;
    std::vector<XrViewConfigurationView> viewConfigViews;
    ret = xrEnumerateViewConfigurationViews(instance_, system_id_, viewConfigType, 0, &viewCount, nullptr);

    viewConfigViews.resize(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    ret = xrEnumerateViewConfigurationViews(instance_, system_id_, viewConfigType, viewCount, &viewCount, viewConfigViews.data());
    LOGI("xrEnumerateViewConfigurationViews ret:%d",ret);
    LOGI("viewCount:%d",viewCount);

    LOGI("XrEnumBlendModes");
    uint32_t blendModeCount;
    xrEnumerateEnvironmentBlendModes(instance_, system_id_, viewConfigType, 0, &blendModeCount, nullptr);

    std::vector<XrEnvironmentBlendMode> blendModes(blendModeCount);
    xrEnumerateEnvironmentBlendModes(instance_, system_id_, viewConfigType, blendModeCount, &blendModeCount, blendModes.data());

    LOGV("Available Viewport Configuration Types: %d", viewConfigTypeCount);

    LOGI("XrEnumSwapchainFormats");
    uint32_t formatCount;
    xrEnumerateSwapchainFormats(session_, 0, &formatCount, nullptr);

    std::vector<int64_t> swapchainFormats(formatCount);
    xrEnumerateSwapchainFormats(session_, formatCount, &formatCount, swapchainFormats.data());


    LOGI("XrCreateSwapchain xrEnumerateSwapchainImages");
    swapchains_.resize(viewCount);
    swapchains_image_array_.resize(viewCount);
    for(int i = 0; i < viewCount; i++)
    {
        XrSwapchainCreateInfo swapchainCreateInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
        swapchainCreateInfo.createFlags = 0;
        swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.format = GL_RGBA8;
        swapchainCreateInfo.sampleCount = 1;
        swapchainCreateInfo.width = viewConfigViews[i].recommendedImageRectWidth;
        swapchainCreateInfo.height = viewConfigViews[i].recommendedImageRectHeight;
        swapchainCreateInfo.faceCount = 1;
        swapchainCreateInfo.arraySize = 1;
        swapchainCreateInfo.mipCount = 1;

        xrCreateSwapchain(session_, &swapchainCreateInfo, &(swapchains_[i]));

        uint32_t imageCount;
        xrEnumerateSwapchainImages(swapchains_[i], 0, &imageCount, nullptr);
        std::vector<XrSwapchainImageOpenGLESKHR> swapchainImages(imageCount);
        for (XrSwapchainImageOpenGLESKHR& image : swapchainImages)
        {
            image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        }
        XrSwapchainImageBaseHeader* swapchainImageBase;
        swapchainImageBase = (reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImages.data()));
        xrEnumerateSwapchainImages(swapchains_[i], imageCount, &imageCount, swapchainImageBase);
        swapchains_image_array_[i] = swapchainImages;
    }
}

void OpenxrContext::InitSpace() {
    bool stageSupported = false;

    uint32_t numOutputSpaces = 0;
    xrEnumerateReferenceSpaces(session_, 0, &numOutputSpaces, NULL);

    XrReferenceSpaceType* referenceSpaces =
            (XrReferenceSpaceType*)malloc(numOutputSpaces * sizeof(XrReferenceSpaceType));

    xrEnumerateReferenceSpaces(
            session_, numOutputSpaces, &numOutputSpaces, referenceSpaces);

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
    xrCreateReferenceSpace(session_, &spaceCreateInfo, &head_space_);

    // local space
     spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
     xrCreateReferenceSpace(session_, &spaceCreateInfo, &local_space_);

    // Create a default stage space to use if SPACE_TYPE_STAGE is not
    // supported, or calls to xrGetReferenceSpaceBoundsRect fail.

    if (stageSupported) {
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        spaceCreateInfo.poseInReferenceSpace.position.y = 0.0f;
        xrCreateReferenceSpace(session_, &spaceCreateInfo, &app_space_);
        LOGV("Created stage space");
    } else {
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        spaceCreateInfo.poseInReferenceSpace.position.y = -1.6750f;
        xrCreateReferenceSpace(session_, &spaceCreateInfo, &app_space_);
        LOGV("Created fake stage space from local space with offset");
    }
}

void OpenxrContext::LogInfos() {
    // Log available layers.
    {
        XrResult result;

        PFN_xrEnumerateApiLayerProperties xrEnumerateApiLayerProperties;
        result = xrGetInstanceProcAddr(
                XR_NULL_HANDLE,
                "xrEnumerateApiLayerProperties",
                (PFN_xrVoidFunction*)&xrEnumerateApiLayerProperties);
        if (result != XR_SUCCESS) {
            LOGE("Failed to get xrEnumerateApiLayerProperties function pointer.");
            exit(1);
        }

        uint32_t numInputLayers = 0;
        uint32_t numOutputLayers = 0;
        xrEnumerateApiLayerProperties(numInputLayers, &numOutputLayers, NULL);

        numInputLayers = numOutputLayers;

        XrApiLayerProperties* layerProperties =
                (XrApiLayerProperties*)malloc(numOutputLayers * sizeof(XrApiLayerProperties));

        for (uint32_t i = 0; i < numOutputLayers; i++) {
            layerProperties[i].type = XR_TYPE_API_LAYER_PROPERTIES;
            layerProperties[i].next = NULL;
        }

        xrEnumerateApiLayerProperties(numInputLayers, &numOutputLayers, layerProperties);

        for (uint32_t i = 0; i < numOutputLayers; i++) {
            LOGV("Found layer %s", layerProperties[i].layerName);
        }

        free(layerProperties);
    }
}

void OpenxrContext::PollEvents(bool* exitRenderLoop, bool* sessionRunning, bool* sessionStateStopping, bool* sessionStateExiting) {
    LOGI("In ProcessEvents");
    *exitRenderLoop = false;

    XrEventDataBuffer buffer{XR_TYPE_EVENT_DATA_BUFFER};
    XrEventDataBaseHeader* header = reinterpret_cast<XrEventDataBaseHeader*>(&buffer);

    if (instance_loss_state_ == true && (GetSeconds() - start_) >= instance_loss_state_time_) {
        LOGI("*exitRenderLoop = true");
        *exitRenderLoop = true;
    }

    // Process all pending messages.
    while (xrPollEvent(instance_, &buffer) != XR_EVENT_UNAVAILABLE)
    {
        LOGI("xrPollEvent(mInstance, &buffer) != XR_EVENT_UNAVAILABLE");
        switch (header->type)
        {
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
            {
                LOGI("XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING exitRenderLoop = true");
                const auto stateInstanceEvent = *reinterpret_cast<const XrEventDataInstanceLossPending*>(header);
                if (stateInstanceEvent.lossTime > 0) {
                    start_ = GetSeconds();
                    LOGI("start_: %f", start_);
                    instance_loss_state_ = true;
                    instance_loss_state_time_ = (double)(stateInstanceEvent.lossTime / 1000000000);
                    LOGI("instance_loss_state_time_: %f", instance_loss_state_time_);
                    return;
                }

                *exitRenderLoop = true;
                return;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
            {
                LOGI("XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED");
                XrSessionState sessionState{XR_SESSION_STATE_UNKNOWN};
                const auto stateEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(header);
                sessionState = stateEvent.state;
                switch (sessionState)
                {
                    case XR_SESSION_STATE_READY:
                    {
                        LOGI("XR_SESSION_STATE_READY");
                        XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
                        sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO

                        XrResult retxrBeginSessSw = xrBeginSession(session_, &sessionBeginInfo);
                        LOGI("retxrBeginSessSw: %d",retxrBeginSessSw);
                        *sessionRunning = true;
                        break;
                    }
                    case XR_SESSION_STATE_STOPPING:
                    {
                        LOGI("XR_SESSION_STATE_STOPPING");
                        *sessionRunning = false;
                        XrResult retxrEndSession = xrEndSession(session_);
                        LOGI("retxrEndSession: %d", retxrEndSession);
                        *sessionStateStopping = true;
                        break;
                    }
                    case XR_SESSION_STATE_EXITING:
                    {
                        LOGI("XR_SESSION_STATE_EXITING");
                        *sessionStateExiting = true;
                        break;
                    }
                    case XR_SESSION_STATE_LOSS_PENDING:
                    {
                        // Poll for a new systemId
                        LOGI("XR_SESSION_STATE_LOSS_PENDING exitRenderLoop = true");
                        *exitRenderLoop = true;
                        break;
                    }
                    default:
                    {
                        LOGI("default");
                        break;
                    }

                }
                break;
            }
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
            {
                LOGI("XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING");
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
            {
                LOGI("XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED");
                break;
            }
            default:
            {
                LOGI("Ignoring event type %d", header->type);
                break;
            }
        }
    }
}

void OpenxrContext::PollActions() {

}

void OpenxrContext::Destory() {
    input_state_ = {};

    graphics_plugin_->DestroyContext();

    xrDestroySpace(app_space_);
    app_space_ = XR_NULL_HANDLE;

    xrDestroySession(session_);
    session_ = XR_NULL_HANDLE;

    xrDestroyInstance(instance_);
    instance_ = XR_NULL_HANDLE;
}

void OpenxrContext::BeginSession() {
    XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
    sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; //XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    XrResult retxrBeginSession = xrBeginSession(session_, &sessionBeginInfo);
    LOGI("retxrBeginSession :%d",retxrBeginSession);
}
}
