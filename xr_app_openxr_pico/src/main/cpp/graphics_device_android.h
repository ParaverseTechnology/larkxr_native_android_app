//
// Created by Administrator on 2021/7/7.
//

#ifndef CLOUDLARKXR_GRAPHICS_DEVICE_ANDROID_H
#define CLOUDLARKXR_GRAPHICS_DEVICE_ANDROID_H


#include <common/gfxwrapper_opengl.h>
#include <vector>
#include <string>
#include <list>
#include <map>
#include "openxr/openxr_platform.h"

class GraphicsDeviceAndroid {
public:
    GraphicsDeviceAndroid();
    ~GraphicsDeviceAndroid();

    void InitializeDevice(XrInstance instance, XrSystemId systemId);
    int64_t SelectColorSwapchainFormat(const std::vector<int64_t>& runtimeFormats);
    std::vector<XrSwapchainImageBaseHeader*> AllocateSwapchainImageStructs(
            uint32_t capacity, const XrSwapchainCreateInfo& /*swapchainCreateInfo*/);

    void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message);
    uint32_t GetDepthTexture(uint32_t colorTexture);
    inline std::vector<std::string> GetInstanceExtensions() const { return {XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME}; }
    inline void Swap() {
        if ((every_other_++ & 1) != 0) {
            ksGpuWindow_SwapBuffers(&window_);
        }
    }
    inline ksGpuWindow window() { return window_; }
    inline XrBaseInStructure* GetGraphicsBinding() {
        return reinterpret_cast<XrBaseInStructure *>(&graphics_binding_);
    }
    inline GLuint swapchain_framebuffer() { return swapchain_framebuffer_; }
    inline uint32_t GetSupportedSwapchainSampleCount(const XrViewConfigurationView& view) {
        return view.recommendedSwapchainSampleCount;
    }
private:
    ksGpuWindow window_;
    XrGraphicsBindingOpenGLESAndroidKHR graphics_binding_{XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
    std::list<std::vector<XrSwapchainImageOpenGLESKHR>> swapchain_image_buffers_;
    GLuint swapchain_framebuffer_{0};
    // Map color buffer to associated depth buffer. This map is populated on demand.
    std::map<uint32_t, uint32_t> color_to_depth_map_;
    uint64_t every_other_{0};
};


#endif //CLOUDLARKXR_GRAPHICS_DEVICE_ANDROID_H
