//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//
#include "graphics_device_android.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "log.h"
#include "openxr/openxr_platform.h"

namespace {
    static const char* EglErrorString(const EGLint error) {
        switch (error) {
            case EGL_SUCCESS:
                return "EGL_SUCCESS";
            case EGL_NOT_INITIALIZED:
                return "EGL_NOT_INITIALIZED";
            case EGL_BAD_ACCESS:
                return "EGL_BAD_ACCESS";
            case EGL_BAD_ALLOC:
                return "EGL_BAD_ALLOC";
            case EGL_BAD_ATTRIBUTE:
                return "EGL_BAD_ATTRIBUTE";
            case EGL_BAD_CONTEXT:
                return "EGL_BAD_CONTEXT";
            case EGL_BAD_CONFIG:
                return "EGL_BAD_CONFIG";
            case EGL_BAD_CURRENT_SURFACE:
                return "EGL_BAD_CURRENT_SURFACE";
            case EGL_BAD_DISPLAY:
                return "EGL_BAD_DISPLAY";
            case EGL_BAD_SURFACE:
                return "EGL_BAD_SURFACE";
            case EGL_BAD_MATCH:
                return "EGL_BAD_MATCH";
            case EGL_BAD_PARAMETER:
                return "EGL_BAD_PARAMETER";
            case EGL_BAD_NATIVE_PIXMAP:
                return "EGL_BAD_NATIVE_PIXMAP";
            case EGL_BAD_NATIVE_WINDOW:
                return "EGL_BAD_NATIVE_WINDOW";
            case EGL_CONTEXT_LOST:
                return "EGL_CONTEXT_LOST";
            default:
                return "unknown";
        }
    }
}

namespace hxr {

GraphicsDeviceAndroid::GraphicsDeviceAndroid() = default;

GraphicsDeviceAndroid::~GraphicsDeviceAndroid() = default;

void GraphicsDeviceAndroid::InitializeDevice(XrInstance instance, XrSystemId systemId) {
    // Get the graphics requirements.
    LOGI("XrGetOpenGLESGraphicsRequirementsKHR");
    XrGraphicsRequirementsOpenGLESKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
    xrGetOpenGLESGraphicsRequirementsKHR(instance, systemId, &graphicsRequirements);

    CreateContext(nullptr);

    // Check the graphics requirements.
    int eglMajor = 0;
    int eglMinor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &eglMajor);
    glGetIntegerv(GL_MINOR_VERSION, &eglMinor);
    const XrVersion eglVersion = XR_MAKE_VERSION(eglMajor, eglMinor, 0);
    if (eglVersion < graphicsRequirements.minApiVersionSupported ||
        eglVersion > graphicsRequirements.maxApiVersionSupported) {
        LOGE("GLES version %d.%d not supported", eglMajor, eglMinor);
        exit(0);
    }

    graphics_binding_.next = nullptr;
    graphics_binding_.config = config_;
    graphics_binding_.context = context_;
    graphics_binding_.display = display_;
}

void GraphicsDeviceAndroid::ClearEgl() {
    major_version_ = 0;
    minor_version_ = 0;
    display_ = nullptr;
    config_ = nullptr;
    tiny_surface_ = EGL_NO_SURFACE;
    main_surface_ = EGL_NO_SURFACE;
    context_ = EGL_NO_CONTEXT;
}

void GraphicsDeviceAndroid::CreateContext(EGLContext shareContext) {
    if (display_ != nullptr) {
        return;
    }

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    LOGV("        eglInitialize( Display, &MajorVersion, &MinorVersion )");
    eglInitialize(display_, &major_version_, &minor_version_);
    // Do NOT use eglChooseConfig, because the Android EGL code pushes in multisample
    // flags in eglChooseConfig if the user has selected the "force 4x MSAA" option in
    // settings, and that is completely wasted for our warp target.
    const int MAX_CONFIGS = 1024;
    EGLConfig configs[MAX_CONFIGS];
    EGLint numConfigs = 0;
    if (eglGetConfigs(display_, configs, MAX_CONFIGS, &numConfigs) == EGL_FALSE) {
        LOGE("        eglGetConfigs() failed: %s", EglErrorString(eglGetError()));
        return;
    }
    const EGLint configAttribs[] = {
            EGL_RED_SIZE,
            8,
            EGL_GREEN_SIZE,
            8,
            EGL_BLUE_SIZE,
            8,
            EGL_ALPHA_SIZE,
            8, // need alpha for the multi-pass timewarp compositor
            EGL_DEPTH_SIZE,
            0,
            EGL_STENCIL_SIZE,
            0,
            EGL_SAMPLES,
            0,
            EGL_NONE};
    config_ = nullptr;
    for (int i = 0; i < numConfigs; i++) {
        EGLint value = 0;

        eglGetConfigAttrib(display_, configs[i], EGL_RENDERABLE_TYPE, &value);
        if ((value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR) {
            continue;
        }

        // The pbuffer config also needs to be compatible with normal window rendering
        // so it can share textures with the window context.
        eglGetConfigAttrib(display_, configs[i], EGL_SURFACE_TYPE, &value);
        if ((value & (EGL_WINDOW_BIT | EGL_PBUFFER_BIT)) != (EGL_WINDOW_BIT | EGL_PBUFFER_BIT)) {
            continue;
        }

        int j = 0;
        for (; configAttribs[j] != EGL_NONE; j += 2) {
            eglGetConfigAttrib(display_, configs[i], configAttribs[j], &value);
            if (value != configAttribs[j + 1]) {
                break;
            }
        }
        if (configAttribs[j] == EGL_NONE) {
            config_ = configs[i];
            break;
        }
    }
    if (config_ == nullptr) {
        LOGE("        eglChooseConfig() failed: %s", EglErrorString(eglGetError()));
        return;
    }
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    LOGV("        Context = eglCreateContext( Display, Config, EGL_NO_CONTEXT, contextAttribs )");
    context_ = eglCreateContext(
            display_,
            config_,
            (shareContext != nullptr) ? shareContext : EGL_NO_CONTEXT,
            contextAttribs);
    if (context_ == EGL_NO_CONTEXT) {
        LOGE("        eglCreateContext() failed: %s", EglErrorString(eglGetError()));
        return;
    }

#if 0
    const EGLint surfaceAttribs[] = {EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE};
    LOGV("        TinySurface = eglCreatePbufferSurface( Display, Config, surfaceAttribs )");
    tiny_surface_ = eglCreatePbufferSurface(display_, config_, surfaceAttribs);
    if (tiny_surface_ == EGL_NO_SURFACE) {
        LOGE("        eglCreatePbufferSurface() failed: %s", EglErrorString(eglGetError()));
        eglDestroyContext(display_, context_);
        context_ = EGL_NO_CONTEXT;
        return;
    }
    LOGV("        eglMakeCurrent( Display, TinySurface, TinySurface, Context )");
    if (eglMakeCurrent(display_, tiny_surface_, tiny_surface_, context_) ==
        EGL_FALSE) {
        LOGE("        eglMakeCurrent() failed: %s", EglErrorString(eglGetError()));
        eglDestroySurface(display_, tiny_surface_);
        eglDestroyContext(display_, context_);
        context_ = EGL_NO_CONTEXT;
        return;
    }
#endif

//    egl = 1.5
//    es3 = TRUE
//    GL_OES_vertex_array_object
//    GL_QCOM_tiled_rendering
//    GL_EXT_sRGB_write_control
//    GL_EXT_disjoint_timer_query
//    GL_EXT_memory_object
//    GL_EXT_memory_object_fd
//    GL_QCOM_texture_foveated
//    GL_KHR_debug
//    GL_OES_EGL_image
//    EGL_EXT_image_dma_buf_import
//    GL_EXT_texture_border_clamp
//    GL_EXT_texture_filter_anisotropic
//    GL_OES_texture_view
//    GL_EXT_texture_format_sRGB_override
//    GL_IMG_texture_filter_cubic
//    GL_EXT_clip_cull_distance
}

void GraphicsDeviceAndroid::DestroyContext() {
    if (display_ != nullptr) {
        LOGE("        eglMakeCurrent( Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT )");
        if (eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ==
            EGL_FALSE) {
            LOGE("        eglMakeCurrent() failed: %s", EglErrorString(eglGetError()));
        }
    }
    if (context_ != EGL_NO_CONTEXT) {
        LOGE("        eglDestroyContext( Display, Context )");
        if (eglDestroyContext(display_, context_) == EGL_FALSE) {
            LOGE("        eglDestroyContext() failed: %s", EglErrorString(eglGetError()));
        }
        context_ = EGL_NO_CONTEXT;
    }
    if (tiny_surface_ != EGL_NO_SURFACE) {
        LOGE("        eglDestroySurface( Display, TinySurface )");
        if (eglDestroySurface(display_, tiny_surface_) == EGL_FALSE) {
            LOGE("        eglDestroySurface() failed: %s", EglErrorString(eglGetError()));
        }
        tiny_surface_ = EGL_NO_SURFACE;
    }
    if (display_ != nullptr) {
        LOGE("        eglTerminate( Display )");
        if (eglTerminate(display_) == EGL_FALSE) {
            LOGE("        eglTerminate() failed: %s", EglErrorString(eglGetError()));
        }
        display_ = nullptr;
    }
}

void GraphicsDeviceAndroid::InitEGLSurface(ANativeWindow* nativeWindow) {
    native_window_ = nativeWindow;
    const EGLint attribs[] = { EGL_NONE };
    main_surface_ = eglCreateWindowSurface(display_, config_, native_window_, attribs);

    if (eglMakeCurrent(display_, main_surface_,  main_surface_, context_) == EGL_FALSE)
    {
        LOGE("        eglMakeCurrent() failed: %s", EglErrorString(eglGetError()));
    }
}


}