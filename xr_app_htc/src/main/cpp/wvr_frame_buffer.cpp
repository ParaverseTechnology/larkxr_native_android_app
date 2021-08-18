//
// Created by fcx on 2020/5/7.
//

#include <log.h>
#include <cstdlib>
#include <EGL/egl.h>
#include <GLES3/gl31.h>
#include "wvr_frame_buffer.h"
#include "object.h"

#define NUM_MULTIVIEW_SLICES    2

// Functions that have to be queried from EGL
typedef void(*PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei);
PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR glFramebufferTextureMultiviewOVR = NULL;

#ifndef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY        0x9102
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE       0x8D56
#endif

// Functions that have to be queried from EGL
typedef void(*PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR)(GLenum, GLenum, GLuint, GLint, GLsizei, GLint, GLsizei);

PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR glFramebufferTextureMultisampleMultiviewOVR = NULL;

typedef void(*PFNGLTEXSTORAGE3DMULTISAMPLEOES)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei,GLboolean);

PFNGLTEXSTORAGE3DMULTISAMPLEOES glTexStorage3DMultisampleOES = NULL;

WvrFrameBuffer::WvrFrameBuffer(int textureId, int width, int height, bool msaa, bool multiview):
    texture_id_(textureId),
    width_(width),
    height_(height),
    msaa_(msaa)
{
    if (msaa) {
        int samples;
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
        if (samples < 4 || !lark::Object::HasGlExtension("GL_EXT_multisampled_render_to_texture"))
            msaa = false;
    }

    if (multiview) {
        InitializeMultiView();
        if (msaa) {
            msaa = false;
            InitMultiviewMSAA();
            return;
        } else {
            msaa = false;
            InitMultiview();
        }
    }
    else {
        if (msaa) {
            InitMSAA();
        } else {
            msaa = false;
            Init();
        }
    }

    // check FBO status
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        has_error_ = true;
        Clear();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

WvrFrameBuffer::~WvrFrameBuffer() {
    Clear();
}

void WvrFrameBuffer::InitMSAA() {
    // Reference to https://www.khronos.org/registry/gles/extensions/EXT/EXT_multisampled_render_to_texture.txt
    PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT =
            (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress( "glRenderbufferStorageMultisampleEXT" );

    PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT =
            (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress( "glFramebufferTexture2DMultisampleEXT" );

    if (glRenderbufferStorageMultisampleEXT == NULL ||
        glFramebufferTexture2DMultisampleEXT == NULL) {
        has_error_ = true;
        return;
    }

    glGenRenderbuffers(1, &depth_buffer_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id_);
    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, width_, height_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &frame_buffer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id_);
    glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0, 4);
}

void WvrFrameBuffer::Init() {
    glGenFramebuffers(1, &frame_buffer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);

    glGenRenderbuffers(1, &depth_buffer_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id_);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);
}

void WvrFrameBuffer::InitMultiviewMSAA() {
    const GLsizei NumSamples = 2;

    glGenTextures(1, &depth_buffer_id_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_buffer_id_);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, width_, height_, 2);

    glGenFramebuffers(1, &frame_buffer_id_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_id_);

    glFramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_buffer_id_, 0, NumSamples, 0, 2);

    glFramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id_, 0, NumSamples, 0, 2);
}

void WvrFrameBuffer::InitMultiview() {
    glGenTextures(1, &depth_buffer_id_);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_buffer_id_);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, width_, height_, 2);

    glGenFramebuffers(1, &frame_buffer_id_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_id_);

    glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id_, 0, 0, 2);

    glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_buffer_id_, 0, 0, 2);
}

void WvrFrameBuffer::BindFrameBuffer(bool multiview) {
    if(multiview){
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_id_);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id_);
    }
}

void WvrFrameBuffer::UnbindFrameBuffer(bool multiview) {
    if(multiview)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WvrFrameBuffer::Clear() {
    if (depth_buffer_id_ != 0)
        glDeleteRenderbuffers(1, &depth_buffer_id_);
    if (frame_buffer_id_ != 0)
        glDeleteFramebuffers(1, &frame_buffer_id_);
    frame_buffer_id_ = depth_buffer_id_ = texture_id_ = 0;
}

void WvrFrameBuffer::InitializeMultiView() {
    if(glFramebufferTextureMultiviewOVR == NULL)
        glFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
    if (!glFramebufferTextureMultiviewOVR)
    {
        LOGE("Failed to  get proc address for glFramebufferTextureMultiviewOVR!!");
        exit(EXIT_FAILURE);
    }

    if(glFramebufferTextureMultisampleMultiviewOVR == NULL)
        glFramebufferTextureMultisampleMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVR)eglGetProcAddress("glFramebufferTextureMultisampleMultiviewOVR");
    if (!glFramebufferTextureMultisampleMultiviewOVR)
    {
        LOGE("Failed to  get proc address for glFramebufferTextureMultisampleMultiviewOVR!!");
        exit(EXIT_FAILURE);
    }

    if(glTexStorage3DMultisampleOES == NULL)
        glTexStorage3DMultisampleOES = (PFNGLTEXSTORAGE3DMULTISAMPLEOES)eglGetProcAddress("glTexStorage3DMultisampleOES");
    if (!glTexStorage3DMultisampleOES)
    {
        LOGE("Failed to  get proc address for glTexStorage3DMultisampleOES!!");
        exit(EXIT_FAILURE);
    }
}
