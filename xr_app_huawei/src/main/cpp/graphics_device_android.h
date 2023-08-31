//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_GRAPHICS_DEVICE_ANDROID_H
#define LARKXR_GRAPHICS_DEVICE_ANDROID_H

#include <vector>
#include <string>
#include <list>
#include <map>
#include "jni.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "openxr/openxr_platform.h"

namespace hxr {
class GraphicsDeviceAndroid {
public:
    GraphicsDeviceAndroid();
    ~GraphicsDeviceAndroid();

    void InitializeDevice(XrInstance instance, XrSystemId systemId);

    void CreateContext(EGLContext shareContext);
    void InitEGLSurface(ANativeWindow* nativeWindow);
    void ClearEgl();
    void DestroyContext();

    inline XrBaseInStructure* graphics_binding() {
        return reinterpret_cast<XrBaseInStructure *>(&graphics_binding_);
    }
private:
    XrGraphicsBindingOpenGLESAndroidKHR graphics_binding_{XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};

    EGLint major_version_{};
    EGLint minor_version_{};
    EGLDisplay display_{};
    EGLConfig config_{};
    EGLSurface tiny_surface_{};
    EGLSurface main_surface_{};
    EGLContext context_{};

    //Native window surface
    ANativeWindow* native_window_{};
};
}

#endif //LARKXR_GRAPHICS_DEVICE_ANDROID_H
