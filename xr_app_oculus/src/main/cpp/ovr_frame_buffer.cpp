//
// Created by fcx@pingxingyun.com on 2019/11/6.
//

#include <EGL/egl.h>
#include <VrApi.h>
#include <malloc.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl31.h>
#include "ovr_frame_buffer.h"
#include "log.h"
#include "egl_utils.h"

OvrFrameBuffer::OvrFrameBuffer():
    mWidth(0),
    mHeight(0),
    mMultisamples(0),
    mTextureSwapChainLength(0),
    mTextureSwapChainIndex(0),
    use_multiview_(false),
    mColorTextureSwapChain(nullptr),
    mDepthBuffers(nullptr),
    mFrameBuffers(nullptr),
    mCreated(false)
{
}

OvrFrameBuffer::~OvrFrameBuffer() {
    if (mCreated) {
        destory();
    }
};

void OvrFrameBuffer::clear() {
    mWidth = 0;
    mHeight = 0;
    mMultisamples = 0;
    mTextureSwapChainLength = 0;
    mTextureSwapChainIndex = 0;
    use_multiview_ = false;
    mColorTextureSwapChain = nullptr;
    mDepthBuffers = nullptr;
    mFrameBuffers = nullptr;
    mCreated = false;
}

bool OvrFrameBuffer::create(const bool useMultiview, const GLenum colorFormat, const int width, const int height, const int multisamples) {
    auto glRenderbufferStorageMultisampleEXT =
            (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress( "glRenderbufferStorageMultisampleEXT" );
    auto glFramebufferTexture2DMultisampleEXT =
            (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress( "glFramebufferTexture2DMultisampleEXT" );

    auto glFramebufferTextureMultiviewOVR =
            (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) eglGetProcAddress( "glFramebufferTextureMultiviewOVR" );
    auto glFramebufferTextureMultisampleMultiviewOVR =
            (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC) eglGetProcAddress( "glFramebufferTextureMultisampleMultiviewOVR" );

    mWidth = width;
    mHeight = height;
    mMultisamples = multisamples;
    use_multiview_ = useMultiview && (glFramebufferTextureMultiviewOVR != nullptr);
    mColorTextureSwapChain = vrapi_CreateTextureSwapChain3(use_multiview_ ? VRAPI_TEXTURE_TYPE_2D_ARRAY : VRAPI_TEXTURE_TYPE_2D,
            colorFormat, width, height, 1, 3);
    mTextureSwapChainLength = vrapi_GetTextureSwapChainLength(mColorTextureSwapChain);
    mDepthBuffers = (GLuint *)malloc(mTextureSwapChainLength * sizeof(GLuint));
    mFrameBuffers = (GLuint *)malloc(mTextureSwapChainLength * sizeof(GLuint));

    LOGV( "        frameBuffer->UseMultiview = %d", use_multiview_);

    for (int i = 0; i < mTextureSwapChainLength; i++)
    {
        // Create the color buffer texture.
        const GLuint colorTexture = vrapi_GetTextureSwapChainHandle(mColorTextureSwapChain, i);
        GLenum colorTextureTarget = use_multiview_ ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
        GL( glBindTexture( colorTextureTarget, colorTexture ) );
        if (egl_utils::glExtensions.EXT_texture_border_clamp)
        {
            GL( glTexParameteri( colorTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER ) );
            GL( glTexParameteri( colorTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER ) );
            GLfloat borderColor[] = { 0.0F, 0.0F, 0.0F, 0.0F };
            GL( glTexParameterfv( colorTextureTarget, GL_TEXTURE_BORDER_COLOR, borderColor ) );
        }
        else
        {
            // Just clamp to edge. However, this requires manually clearing the border
            // around the layer to clear the edge texels.
            GL( glTexParameteri( colorTextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
            GL( glTexParameteri( colorTextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );
        }
        GL( glTexParameteri( colorTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
        GL( glTexParameteri( colorTextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
        GL( glBindTexture( colorTextureTarget, 0 ) );

        if (use_multiview_)
        {
            // Create the depth buffer texture.
            GL( glGenTextures( 1, &mDepthBuffers[i] ) );
            GL( glBindTexture( GL_TEXTURE_2D_ARRAY, mDepthBuffers[i] ) );
            GL( glTexStorage3D( GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, width, height, 2 ) );
            GL( glBindTexture( GL_TEXTURE_2D_ARRAY, 0 ) );

            // Create the frame buffer.
            GL( glGenFramebuffers( 1, &mFrameBuffers[i] ) );
            GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mFrameBuffers[i] ) );
            if ( multisamples > 1 && ( glFramebufferTextureMultisampleMultiviewOVR != nullptr ) )
            {
                GL( glFramebufferTextureMultisampleMultiviewOVR( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthBuffers[i], 0 /* level */, multisamples /* samples */, 0 /* baseViewIndex */, 2 /* numViews */ ) );
                GL( glFramebufferTextureMultisampleMultiviewOVR( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0 /* level */, multisamples /* samples */, 0 /* baseViewIndex */, 2 /* numViews */ ) );
            }
            else
            {
                GL( glFramebufferTextureMultiviewOVR( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mDepthBuffers[i], 0 /* level */, 0 /* baseViewIndex */, 2 /* numViews */ ) );
                GL( glFramebufferTextureMultiviewOVR( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0 /* level */, 0 /* baseViewIndex */, 2 /* numViews */ ) );
            }

            GL( GLenum renderFramebufferStatus = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) );
            GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
            if ( renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE )
            {
                LOGE( "Incomplete frame buffer object: %s", egl_utils::GlFrameBufferStatusString( renderFramebufferStatus ) );
                return false;
            }
        }
        else
        {
            if ( multisamples > 1 && glRenderbufferStorageMultisampleEXT != nullptr && glFramebufferTexture2DMultisampleEXT != nullptr )
            {
                // Create multisampled depth buffer.
                GL( glGenRenderbuffers( 1, &mDepthBuffers[i] ) );
                GL( glBindRenderbuffer( GL_RENDERBUFFER, mDepthBuffers[i] ) );
                GL( glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER, multisamples, GL_DEPTH_COMPONENT24, width, height ) );
                GL( glBindRenderbuffer( GL_RENDERBUFFER, 0 ) );

                // Create the frame buffer.
                // NOTE: glFramebufferTexture2DMultisampleEXT only works with GL_FRAMEBUFFER.
                GL( glGenFramebuffers( 1, &mFrameBuffers[i] ) );
                GL( glBindFramebuffer( GL_FRAMEBUFFER, mFrameBuffers[i] ) );
                GL( glFramebufferTexture2DMultisampleEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0, multisamples ) );
                GL( glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffers[i] ) );
                GL( GLenum renderFramebufferStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER ) );
                GL( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );
                if ( renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE )
                {
                    LOGE( "Incomplete frame buffer object: %s", egl_utils::GlFrameBufferStatusString( renderFramebufferStatus ) );
                    return false;
                }
            }
            else
            {
                // Create depth buffer.
                GL( glGenRenderbuffers( 1, &mDepthBuffers[i] ) );
                GL( glBindRenderbuffer( GL_RENDERBUFFER, mDepthBuffers[i] ) );
                GL( glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height ) );
                GL( glBindRenderbuffer( GL_RENDERBUFFER, 0 ) );

                // Create the frame buffer.
                GL( glGenFramebuffers( 1, &mFrameBuffers[i] ) );
                GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mFrameBuffers[i] ) );
                GL( glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffers[i] ) );
                GL( glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0 ) );
                GL( GLenum renderFramebufferStatus = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER ) );
                GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
                if ( renderFramebufferStatus != GL_FRAMEBUFFER_COMPLETE )
                {
                    LOGE( "Incomplete frame buffer object: %s", egl_utils::GlFrameBufferStatusString( renderFramebufferStatus ) );
                    return false;
                }
            }
        }
    }
    return true;
}

void OvrFrameBuffer::destory() {
    if (!mCreated) {
        return;
    }
    GL( glDeleteFramebuffers( mTextureSwapChainLength, mFrameBuffers ) );
    if ( use_multiview_ )
    {
        GL( glDeleteTextures( mTextureSwapChainLength, mDepthBuffers ) );
    }
    else
    {
        GL( glDeleteRenderbuffers( mTextureSwapChainLength, mDepthBuffers ) );
    }
    vrapi_DestroyTextureSwapChain( mColorTextureSwapChain );

    free( mDepthBuffers );
    free( mFrameBuffers );
    clear();
}

void OvrFrameBuffer::setCurrent() {
    GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, mFrameBuffers[mTextureSwapChainIndex] ) );
}

void OvrFrameBuffer::setNone() {
    GL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 ) );
}

void OvrFrameBuffer::resolve() {
    // Discard the depth buffer, so the tiler won't need to write it back out to memory.
    const GLenum depthAttachment[1] = { GL_DEPTH_ATTACHMENT };
    glInvalidateFramebuffer( GL_DRAW_FRAMEBUFFER, 1, depthAttachment );

    // Flush this frame worth of commands.
    glFlush();
}

void OvrFrameBuffer::advance() {
    // Advance to the next texture from the set.
    mTextureSwapChainIndex = ( mTextureSwapChainIndex + 1 ) % mTextureSwapChainLength;
}

