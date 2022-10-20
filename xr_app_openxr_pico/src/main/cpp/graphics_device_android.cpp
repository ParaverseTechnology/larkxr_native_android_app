//
// Created by Administrator on 2021/7/7.
//

#include <log.h>
#include "graphics_device_android.h"
#include "common/gfxwrapper_opengl.h"
#include "logger.h"
#include <common/xr_linear.h>

namespace {
    constexpr float DarkSlateGray[] = {0.184313729f, 0.309803933f, 0.309803933f, 1.0f};

    static const char* VertexShaderGlsl = R"_(
        #version 320 es

        in vec3 VertexPos;
        in vec3 VertexColor;

        out vec3 PSVertexColor;

        uniform mat4 ModelViewProjection;

        void main() {
           gl_Position = ModelViewProjection * vec4(VertexPos, 1.0);
           PSVertexColor = VertexColor;
        }
    )_";

    static const char* FragmentShaderGlsl = R"_(
        #version 320 es

        in lowp vec3 PSVertexColor;
        out lowp vec4 FragColor;

        void main() {
           FragColor = vec4(PSVertexColor, 1);
        }
    )_";
}

GraphicsDeviceAndroid::GraphicsDeviceAndroid() {

}

GraphicsDeviceAndroid::~GraphicsDeviceAndroid() {
    if (swapchain_framebuffer_ != 0) {
        glDeleteFramebuffers(1, &swapchain_framebuffer_);
    }
    for (auto& colorToDepth : color_to_depth_map_) {
        if (colorToDepth.second != 0) {
            glDeleteTextures(1, &colorToDepth.second);
        }
    }
}

void GraphicsDeviceAndroid::InitializeDevice(XrInstance instance, XrSystemId systemId) {
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
    graphics_binding_.config = (EGLConfig)0;
    graphics_binding_.context = window_.context.context;
#endif

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
               const void* userParam) {
                ((GraphicsDeviceAndroid*)userParam)->DebugMessageCallback(source, type, id, severity, length, message);
            },
            this);

    glGenFramebuffers(1, &swapchain_framebuffer_);
}

void
GraphicsDeviceAndroid::DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) {
    (void)source;
    (void)type;
    (void)id;
    (void)severity;
    Log::Write(Log::Level::Info, "GLES Debug: " + std::string(message, 0, length));
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

std::vector<XrSwapchainImageBaseHeader *>
GraphicsDeviceAndroid::AllocateSwapchainImageStructs(uint32_t capacity,
                                                     const XrSwapchainCreateInfo &) {
    // Allocate and initialize the buffer of image structs (must be sequential in memory for xrEnumerateSwapchainImages).
    // Return back an array of pointers to each swapchain image struct so the consumer doesn't need to know the type/size.
    std::vector<XrSwapchainImageOpenGLESKHR> swapchainImageBuffer(capacity);
    std::vector<XrSwapchainImageBaseHeader*> swapchainImageBase;
    for (XrSwapchainImageOpenGLESKHR& image : swapchainImageBuffer) {
        image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        swapchainImageBase.push_back(reinterpret_cast<XrSwapchainImageBaseHeader*>(&image));
    }

    // Keep the buffer alive by moving it into the list of buffers.
    swapchain_image_buffers_.push_back(std::move(swapchainImageBuffer));

    return swapchainImageBase;
}

uint32_t GraphicsDeviceAndroid::GetDepthTexture(uint32_t colorTexture) {
    // If a depth-stencil view has already been created for this back-buffer, use it.
    auto depthBufferIt = color_to_depth_map_.find(colorTexture);
    if (depthBufferIt != color_to_depth_map_.end()) {
        return depthBufferIt->second;
    }

    // This back-buffer has no corresponding depth-stencil texture, so create one with matching dimensions.

    GLint width;
    GLint height;
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    uint32_t depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

    color_to_depth_map_.insert(std::make_pair(colorTexture, depthTexture));

    return depthTexture;
}
