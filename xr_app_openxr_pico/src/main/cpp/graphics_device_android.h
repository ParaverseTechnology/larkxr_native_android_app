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

    void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message);

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
private:
#ifdef XR_USE_PLATFORM_ANDROID
    XrGraphicsBindingOpenGLESAndroidKHR graphics_binding_{XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
#endif
    ksGpuWindow window_{};
    uint64_t every_other_{0};
};


#endif //CLOUDLARKXR_GRAPHICS_DEVICE_ANDROID_H
