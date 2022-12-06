//
// Created by fcx@pingxingyung.com on 2022/11/9.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_FRAME_BUFFER_H
#define LARKXR_FRAME_BUFFER_H

#include <stdio.h>
#include "jni.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace picoxr {
class FrameBuffer {
public:
    struct ovrSwapChain{
        XrSwapchain Handle;
        uint32_t Width;
        uint32_t Height;
    };
    FrameBuffer();
    ~FrameBuffer();

    bool Create(XrSession session, const GLenum colorFormat, const int width, const int height, const int multisamples);

    void SetCurrent();
    void SetNone();
    void Resolve();
    void Acquire();

    void Release();

    void Clear();
    void Destroy();

    inline int width() const { return width_; }
    inline int height() const { return height_; }
    inline const ovrSwapChain color_swapchain() const { return color_swapchain_;}
private:
    int width_;
    int height_;
    int multi_samples_;
    uint32_t texture_swapchain_length_;
    uint32_t texture_swapchain_index_;
    ovrSwapChain color_swapchain_;
    XrSwapchainImageOpenGLESKHR* color_swapchain_image_;
    GLuint* depth_buffers_;
    GLuint* frame_buffers_;
};
}

#endif //LARKXR_FRAME_BUFFER_H
