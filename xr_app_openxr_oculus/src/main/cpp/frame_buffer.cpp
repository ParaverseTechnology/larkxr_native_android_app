//
// Created by fcx@pingxingyung.com on 2022/11/9.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include "frame_buffer.h"
#include <memory>
#include "oxr_utils.h"

#define LOG_TAG "xr_framebuffer"

// EXT_texture_border_clamp
#ifndef GL_CLAMP_TO_BORDER
#define GL_CLAMP_TO_BORDER 0x812D
#endif

#ifndef GL_TEXTURE_BORDER_COLOR
#define GL_TEXTURE_BORDER_COLOR 0x1004
#endif

#ifndef GL_FRAMEBUFFER_SRGB_EXT
#define GL_FRAMEBUFFER_SRGB_EXT 0x8DB9
#endif

namespace oxr {
FrameBuffer::FrameBuffer() {

}

FrameBuffer::~FrameBuffer() {

}

bool FrameBuffer::Create(XrSession session, const GLenum colorFormat, const int width,
                         const int height, const int multisamples) {
    PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT =
            (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress(
                    "glRenderbufferStorageMultisampleEXT");
    PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT =
            (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress(
                    "glFramebufferTexture2DMultisampleEXT");

    width_ = width;
    height_ = height;
    multi_samples_ = multisamples;

    GLenum requestedGLFormat = colorFormat;

    // Get the number of supported formats.
    uint32_t numInputFormats = 0;
    uint32_t numOutputFormats = 0;
    OXR(xrEnumerateSwapchainFormats(session, numInputFormats, &numOutputFormats, NULL));

    // Allocate an array large enough to contain the supported formats.
    numInputFormats = numOutputFormats;
    int64_t* supportedFormats = (int64_t*)malloc(numOutputFormats * sizeof(int64_t));
    if (supportedFormats != NULL) {
        OXR(xrEnumerateSwapchainFormats(
                session, numInputFormats, &numOutputFormats, supportedFormats));
    }

    // Verify the requested format is supported.
    uint64_t selectedFormat = 0;
    for (uint32_t i = 0; i < numOutputFormats; i++) {
        if (supportedFormats[i] == requestedGLFormat) {
            selectedFormat = supportedFormats[i];
            break;
        }
    }

    free(supportedFormats);

    if (selectedFormat == 0) {
        ALOGE("Format not supported");
    } else {
        LOGI("Select color format %d %d", selectedFormat, requestedGLFormat);
    }

    XrSwapchainCreateInfo swapChainCreateInfo;
    memset(&swapChainCreateInfo, 0, sizeof(swapChainCreateInfo));
    swapChainCreateInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
    swapChainCreateInfo.usageFlags =
            XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.format = selectedFormat;
    swapChainCreateInfo.sampleCount = 1;
    swapChainCreateInfo.width = width;
    swapChainCreateInfo.height = height;
    swapChainCreateInfo.faceCount = 1;
    swapChainCreateInfo.arraySize = 1;
    swapChainCreateInfo.mipCount = 1;

    // Enable Foveation on this swapchain
//    XrSwapchainCreateInfoFoveationFB swapChainFoveationCreateInfo;
//    memset(&swapChainFoveationCreateInfo, 0, sizeof(swapChainFoveationCreateInfo));
//    swapChainFoveationCreateInfo.type = XR_TYPE_SWAPCHAIN_CREATE_INFO_FOVEATION_FB;
//    swapChainCreateInfo.next = &swapChainFoveationCreateInfo;
    swapChainCreateInfo.next = NULL;

    color_swapchain_.Width = swapChainCreateInfo.width;
    color_swapchain_.Height = swapChainCreateInfo.height;

    // Create the swapchain.
    OXR(xrCreateSwapchain(session, &swapChainCreateInfo, &color_swapchain_.Handle));
    // Get the number of swapchain images.
    OXR(xrEnumerateSwapchainImages(
            color_swapchain_.Handle, 0, &texture_swapchain_length_, NULL));
    // Allocate the swapchain images array.
    color_swapchain_image_ = (XrSwapchainImageOpenGLESKHR*)malloc(
            texture_swapchain_length_ * sizeof(XrSwapchainImageOpenGLESKHR));

    // Populate the swapchain image array.
    for (uint32_t i = 0; i < texture_swapchain_length_; i++) {
        color_swapchain_image_[i].type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR;
        color_swapchain_image_[i].next = NULL;
    }
    OXR(xrEnumerateSwapchainImages(
            color_swapchain_.Handle,
            texture_swapchain_length_,
            &texture_swapchain_length_,
            (XrSwapchainImageBaseHeader*)color_swapchain_image_));

    depth_buffers_ =
            (GLuint*)malloc(texture_swapchain_length_ * sizeof(GLuint));
    frame_buffers_ =
            (GLuint*)malloc(texture_swapchain_length_ * sizeof(GLuint));

    for (uint32_t i = 0; i < texture_swapchain_length_; i++) {
        // Create the color buffer texture.
        const GLuint colorTexture = color_swapchain_image_[i].image;

        GLenum colorTextureTarget = GL_TEXTURE_2D;
        GL(glBindTexture(colorTextureTarget, colorTexture));
        GL(glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL(glTexParameteri(colorTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL(glTexParameteri(colorTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(colorTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL(glBindTexture(colorTextureTarget, 0));

        if (multisamples > 1 && glRenderbufferStorageMultisampleEXT != NULL &&
            glFramebufferTexture2DMultisampleEXT != NULL) {
            // Create multisampled depth buffer.
            GL(glGenRenderbuffers(1, &depth_buffers_[i]));
            GL(glBindRenderbuffer(GL_RENDERBUFFER, depth_buffers_[i]));
            GL(glRenderbufferStorageMultisampleEXT(
                    GL_RENDERBUFFER, multisamples, GL_DEPTH_COMPONENT24, width, height));
            GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

            // Create the frame buffer.
            // NOTE: glFramebufferTexture2DMultisampleEXT only works with GL_FRAMEBUFFER.
            GL(glGenFramebuffers(1, &frame_buffers_[i]));
            GL(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffers_[i]));
            GL(glFramebufferTexture2DMultisampleEXT(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_2D,
                    colorTexture,
                    0,
                    multisamples));
            GL(glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    GL_RENDERBUFFER,
                    depth_buffers_[i]));
            GL(GLenum renderFramebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER));
            GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            if (renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
                ALOGE(
                        "Incomplete frame buffer object: %s",
                        GlFrameBufferStatusString(renderFramebufferStatus));
                return false;
            }
        } else {
            // Create depth buffer.
            GL(glGenRenderbuffers(1, &depth_buffers_[i]));
            GL(glBindRenderbuffer(GL_RENDERBUFFER, depth_buffers_[i]));
            GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height));
            GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

            // Create the frame buffer.
            GL(glGenFramebuffers(1, &frame_buffers_[i]));
            GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffers_[i]));
            GL(glFramebufferRenderbuffer(
                    GL_DRAW_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    GL_RENDERBUFFER,
                    depth_buffers_[i]));
            GL(glFramebufferTexture2D(
                    GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0));
            GL(GLenum renderFramebufferStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
            GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            if (renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
                ALOGE(
                        "Incomplete frame buffer object: %s",
                        GlFrameBufferStatusString(renderFramebufferStatus));
                return false;
            }
        }
    }

    // hack color space
    // https://forums.oculusvr.com/t5/OpenXR-Development/sRGB-RGB-giving-washed-out-bright-image/m-p/957475
    GL(glDisable(GL_FRAMEBUFFER_SRGB_EXT));

    return true;
}

void FrameBuffer::SetCurrent() {
    GL(glBindFramebuffer(
            GL_DRAW_FRAMEBUFFER, frame_buffers_[texture_swapchain_index_]));
}

void FrameBuffer::SetNone() {
    GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}

void FrameBuffer::Resolve() {
    // Discard the depth buffer, so the tiler won't need to write it back out to memory.
    const GLenum depthAttachment[1] = {GL_DEPTH_ATTACHMENT};
    glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 1, depthAttachment);

    // We now let the resolve happen implicitly.
}

void FrameBuffer::Acquire() {
    // Acquire the swapchain image
    XrSwapchainImageAcquireInfo acquireInfo = {XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO, NULL};
    OXR(xrAcquireSwapchainImage(
            color_swapchain_.Handle, &acquireInfo, &texture_swapchain_index_));

    XrSwapchainImageWaitInfo waitInfo;
    waitInfo.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
    waitInfo.next = NULL;
    waitInfo.timeout = 1000000000; /* timeout in nanoseconds */
    XrResult res = xrWaitSwapchainImage(color_swapchain_.Handle, &waitInfo);
    int i = 0;
    while (res == XR_TIMEOUT_EXPIRED) {
        res = xrWaitSwapchainImage(color_swapchain_.Handle, &waitInfo);
        i++;
        ALOGV(
                " Retry xrWaitSwapchainImage %d times due to XR_TIMEOUT_EXPIRED (duration %f seconds)",
                i,
                waitInfo.timeout * (1E-9));
    }
}

void FrameBuffer::Release() {
    XrSwapchainImageReleaseInfo releaseInfo = {XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO, NULL};
    OXR(xrReleaseSwapchainImage(color_swapchain_.Handle, &releaseInfo));
}

void FrameBuffer::Clear() {
    width_ = 0;
    height_ = 0;
    multi_samples_ = 0;
    texture_swapchain_length_ = 0;
    texture_swapchain_index_ = 0;
    color_swapchain_.Handle = XR_NULL_HANDLE;
    color_swapchain_.Width = 0;
    color_swapchain_.Height = 0;
    color_swapchain_image_ = NULL;
    depth_buffers_ = NULL;
    frame_buffers_ = NULL;
}

void FrameBuffer::Destroy() {
    GL(glDeleteFramebuffers(texture_swapchain_length_, frame_buffers_));
    GL(glDeleteRenderbuffers(texture_swapchain_length_, depth_buffers_));
    OXR(xrDestroySwapchain(color_swapchain_.Handle));
    free(color_swapchain_image_);

    free(depth_buffers_);
    free(frame_buffers_);

    Clear();
}
}
