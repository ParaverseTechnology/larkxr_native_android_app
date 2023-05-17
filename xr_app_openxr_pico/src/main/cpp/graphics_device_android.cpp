//
// Created by Administrator on 2021/7/7.
//

#include <log.h>
#include "graphics_device_android.h"
#include "common/gfxwrapper_opengl.h"
#include "logger.h"
#include <common/xr_linear.h>

#define LOG_TAG "graphics_device"

#define GL_MULTISAMPLE                    0x809D


GraphicsDeviceAndroid::GraphicsDeviceAndroid() = default;

GraphicsDeviceAndroid::~GraphicsDeviceAndroid() = default;

void GraphicsDeviceAndroid::InitializeDevice(XrInstance instance, XrSystemId systemId) {
    glEnable(GL_MULTISAMPLE);

    // Extension function must be loaded by name
    PFN_xrGetOpenGLESGraphicsRequirementsKHR pfnGetOpenGLESGraphicsRequirementsKHR = nullptr;
    xrGetInstanceProcAddr(instance, "xrGetOpenGLESGraphicsRequirementsKHR",
                                      reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetOpenGLESGraphicsRequirementsKHR));

    XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
    pfnGetOpenGLESGraphicsRequirementsKHR(instance, systemId, &graphicsRequirements);

    // Initialize the gl extensions. Note we have to open a window.
    ksDriverInstance driverInstance{};
    ksGpuQueueInfo queueInfo{};
    ksGpuSurfaceColorFormat colorFormat{KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8};
    ksGpuSurfaceDepthFormat depthFormat{KS_GPU_SURFACE_DEPTH_FORMAT_D24};
    ksGpuSampleCount sampleCount{KS_GPU_SAMPLE_COUNT_1};
    if (!ksGpuWindow_Create(&window_, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, false)) {
        LOGE("Unable to create GL context");
    }

    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    const XrVersion desiredApiVersion = XR_MAKE_VERSION(major, minor, 0);
    if (graphicsRequirements.minApiVersionSupported > desiredApiVersion) {
        LOGE("Runtime does not support desired Graphics API and/or version");
    }

#if defined(XR_USE_PLATFORM_ANDROID)
    graphics_binding_.display = window_.display;
    graphics_binding_.config = window_.context.config;
    graphics_binding_.context = window_.context.context;
#endif

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
               const void* userParam) {
                ((GraphicsDeviceAndroid*)userParam)->DebugMessageCallback(source, type, id, severity, length, message);
            },
            this);
}

void
GraphicsDeviceAndroid::DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) {
    (void)source;
    (void)type;
    (void)id;
    (void)severity;
//     Log::Write(Log::Level::Info, "GLES Debug: " + std::string(message, 0, length));
}

int64_t
GraphicsDeviceAndroid::SelectColorSwapchainFormat(const std::vector<int64_t> &runtimeFormats) {
    // List of supported color swapchain formats.
    constexpr int64_t SupportedColorSwapchainFormats[] = {
            GL_RGBA8,
            GL_RGBA8_SNORM,
    };

    auto swapchainFormatIt =
            std::find_first_of(runtimeFormats.begin(), runtimeFormats.end(), std::begin(SupportedColorSwapchainFormats),
                               std::end(SupportedColorSwapchainFormats));
    if (swapchainFormatIt == runtimeFormats.end()) {
        LOGE("No runtime swapchain format supported for color swapchain");
    }

    return *swapchainFormatIt;
}
