//=============================================================================
// FILE: svrRenderTarget.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#include <GLES3/gl3.h>
#include <log.h>

#include "svrRenderTarget.h"
#include "svrUtil.h"


#if !defined( GL_TEXTURE_PROTECTED_EXT )
#define GL_TEXTURE_PROTECTED_EXT    0x8BFA
#endif

namespace Svr
{

void GetFormatTypeFromSizedFormat(int sizedFormat, int& outFormat, int& outType)
{
    switch(sizedFormat)
    {
    case GL_R8:
        outFormat = GL_RED;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_R8_SNORM:
        outFormat = GL_RED;
        outType = GL_BYTE;
        return;
    case GL_R16F:
        outFormat = GL_RED;
        outType = GL_HALF_FLOAT;
        return;
    case GL_R32F:
        outFormat = GL_RED;
        outType = GL_FLOAT;
        return;
    case GL_R8UI:
        outFormat = GL_RED_INTEGER;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_R8I:
        outFormat = GL_RED_INTEGER;
        outType = GL_BYTE;
        return;
    case GL_R16UI:
        outFormat = GL_RED_INTEGER;
        outType = GL_UNSIGNED_SHORT;
        return;
    case GL_R16I:
        outFormat = GL_RED_INTEGER;
        outType = GL_SHORT;
        return;
    case GL_R32UI:
        outFormat = GL_RED_INTEGER;
        outType = GL_UNSIGNED_INT;
        return;
    case GL_R32I:
        outFormat = GL_RED_INTEGER;
        outType = GL_INT;
        return;
    case GL_RG8:
        outFormat = GL_RG;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RG8_SNORM:
        outFormat = GL_RG;
        outType = GL_BYTE;
        return;
    case GL_RG16F:
        outFormat = GL_RG;
        outType = GL_HALF_FLOAT;
        return;
    case GL_RG32F:
        outFormat = GL_RG;
        outType = GL_FLOAT;
        return;
    case GL_RG8UI:
        outFormat = GL_RG_INTEGER;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RG8I:
        outFormat = GL_RG_INTEGER;
        outType = GL_BYTE;
        return;
    case GL_RG16UI:
        outFormat = GL_RG_INTEGER;
        outType = GL_UNSIGNED_SHORT;
        return;
    case GL_RG16I:
        outFormat = GL_RG_INTEGER;
        outType = GL_SHORT;
        return;
    case GL_RG32UI:
        outFormat = GL_RG_INTEGER;
        outType = GL_UNSIGNED_INT;
        return;
    case GL_RG32I:
        outFormat = GL_RG_INTEGER;
        outType = GL_INT;
        return;
    case GL_RGB8:
        outFormat = GL_RGB;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_SRGB8:
        outFormat = GL_RGB;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGB565:
        outFormat = GL_RGB;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGB8_SNORM:
        outFormat = GL_RGB;
        outType = GL_BYTE;
        return;
    case GL_R11F_G11F_B10F:
        outFormat = GL_RGB;
        outType = GL_HALF_FLOAT;
        return;
    case GL_RGB9_E5:
        outFormat = GL_RGB;
        outType = GL_HALF_FLOAT;
        return;
    case GL_RGB16F:
        outFormat = GL_RGB;
        outType = GL_HALF_FLOAT;
        return;
    case GL_RGB32F:
        outFormat = GL_RGB;
        outType = GL_FLOAT;
        return;
    case GL_RGB8UI:
        outFormat = GL_RGB_INTEGER;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGB8I:
        outFormat = GL_RGB_INTEGER;
        outType = GL_BYTE;
        return;
    case GL_RGB16UI:
        outFormat = GL_RGB_INTEGER;
        outType = GL_UNSIGNED_SHORT;
        return;
    case GL_RGB16I:
        outFormat = GL_RGB_INTEGER;
        outType = GL_SHORT;
        return;
    case GL_RGB32UI:
        outFormat = GL_RGB_INTEGER;
        outType = GL_UNSIGNED_INT;
        return;
    case GL_RGB32I:
        outFormat = GL_RGB_INTEGER;
        outType = GL_INT;
        return;
    case GL_RGBA8:
        outFormat = GL_RGBA;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_SRGB8_ALPHA8:
        outFormat = GL_RGBA;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGBA8_SNORM:
        outFormat = GL_RGBA;
        outType = GL_BYTE;
        return;
    case GL_RGB5_A1:
        outFormat = GL_RGBA;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGBA4:
        outFormat = GL_RGBA;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGB10_A2:
        outFormat = GL_RGBA;
        outType = GL_UNSIGNED_INT_2_10_10_10_REV;
        return;
    case GL_RGBA16F:
        outFormat = GL_RGBA;
        outType = GL_HALF_FLOAT;
        return;
    case GL_RGBA32F:
        outFormat = GL_RGBA;
        outType = GL_FLOAT;
        return;
    case GL_RGBA8UI:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_UNSIGNED_BYTE;
        return;
    case GL_RGBA8I:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_BYTE;
        return;
    case GL_RGB10_A2UI:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_UNSIGNED_INT_2_10_10_10_REV;
        return;
    case GL_RGBA16UI:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_UNSIGNED_SHORT;
        return;
    case GL_RGBA16I:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_SHORT;
        return;
    case GL_RGBA32I:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_INT;
        return;
    case GL_RGBA32UI:
        outFormat = GL_RGBA_INTEGER;
        outType = GL_UNSIGNED_INT;
        return;
    }
}

bool CheckFrameBuffer(const char* pPrefix)
{
    // Check results
    GLenum eResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(eResult != GL_FRAMEBUFFER_COMPLETE)
    {
        switch(eResult)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGE("%s => Error (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) setting up FBO", pPrefix);
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGE("%s => Error (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) setting up FBO", pPrefix);
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGE("%s => Error (GL_FRAMEBUFFER_UNSUPPORTED) setting up FBO", pPrefix);
            break;
        default:
            LOGE("%s => Error (0x%X) setting up FBO", pPrefix, eResult);
            break;
        }
        // We failed setting up the FBO
        return false;
    }
    return true;
}

SvrRenderTarget::SvrRenderTarget()
    : mWidth(0)
    , mHeight(0)
    , mSamples(0)
    , mColorAttachmentId(0)
    , mDepthAttachmentId(0)
    , mFramebufferId(0)
{
    mRefCount = 0;
}

SvrRenderTarget::~SvrRenderTarget()
{
	Destroy();
}

void SvrRenderTarget::InitializeSingleSample(int width, int height, int samples, int colorSizedFormat, int format, int type, bool requiresDepth)
{
	GL(glGenTextures(1, &mColorAttachmentId));
	GL(glBindTexture(GL_TEXTURE_2D, mColorAttachmentId));

	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    if (mIsProtectedContent)
    {
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PROTECTED_EXT, GL_TRUE));
        GL(glTexStorage2D(GL_TEXTURE_2D, 1, colorSizedFormat, width, height));
    } 
    else 
    {
        GL(glTexImage2D(GL_TEXTURE_2D, 0, colorSizedFormat, width, height, 0, format, type, NULL));
    }
	GL(glBindTexture(GL_TEXTURE_2D, 0));

	if (requiresDepth)
	{
		//Create the depth attachment (forced to 32bit currently)
		GL(glGenTextures(1, &mDepthAttachmentId));
		GL(glBindTexture(GL_TEXTURE_2D, mDepthAttachmentId));
        if(mIsProtectedContent)
        {
            GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_PROTECTED_EXT, GL_TRUE));
            GL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height));

        } 
        else 
        {
            GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL));
        }
		GL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	//Create the framebuffer
	GL(glGenFramebuffers(1, &mFramebufferId));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId));

	GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachmentId, 0));
	if (requiresDepth)
	{
		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthAttachmentId, 0));
    }
}

void SvrRenderTarget::InitializeMultiSample(int width, int height, int samples, int colorSizedFormat, int format, bool requiresDepth)
{
	const GLenum renderBuffer = GL_RENDERBUFFER;
	GL(glGenRenderbuffers(1, &mColorAttachmentId));
	GL(glBindRenderbuffer(renderBuffer, mColorAttachmentId));
	GL(glRenderbufferStorageMultisample(renderBuffer, samples, colorSizedFormat, width, height));

	if (requiresDepth)
	{
		//Create the depth attachment (forced to 32bit currently)
		GL(glGenRenderbuffers(1, &mDepthAttachmentId));
		GL(glBindRenderbuffer(renderBuffer, mDepthAttachmentId));
		GL(glRenderbufferStorageMultisample(renderBuffer, samples, GL_DEPTH24_STENCIL8, width, height));
		GL(glBindRenderbuffer(renderBuffer, 0));
	}

	//Create the framebuffer
	const GLenum frameBuffer = GL_FRAMEBUFFER;
	GL(glGenFramebuffers(1, &mFramebufferId));
	GL(glBindFramebuffer(frameBuffer, mFramebufferId));

	GL(glFramebufferRenderbuffer(frameBuffer, GL_COLOR_ATTACHMENT0, renderBuffer, mColorAttachmentId));
	if (requiresDepth)
	{
		GL(glFramebufferRenderbuffer(frameBuffer, GL_DEPTH_ATTACHMENT, renderBuffer, mDepthAttachmentId));
	}
}

void SvrRenderTarget::Initialize(int width, int height, int samples, int colorSizedFormat, bool requiresDepth, bool isProtectedContent)
{
    mWidth = width;
    mHeight = height;
    mSamples = samples;
    mIsProtectedContent = isProtectedContent;

    int format, type;
    GetFormatTypeFromSizedFormat( colorSizedFormat, format, type);

    //Create the color attachment
	if (samples > 1)
	{
		InitializeMultiSample(width, height, samples, colorSizedFormat, format, requiresDepth);
	}
	else
	{
		InitializeSingleSample(width, height, samples, colorSizedFormat, format, type, requiresDepth);
	}

	CheckFrameBuffer("2D FBO");

	GL( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );
}

void SvrRenderTarget::Destroy()
{
    GL( glDeleteFramebuffers( 1, &mFramebufferId));

	if (mSamples > 1)
	{
		GL(glDeleteRenderbuffers(1, &mColorAttachmentId));
		if (mDepthAttachmentId != 0)
		{
			GL(glDeleteRenderbuffers(1, &mDepthAttachmentId));
		}
	}
	else
	{
		GL(glDeleteTextures(1, &mColorAttachmentId));
		if (mDepthAttachmentId != 0)
		{
			GL(glDeleteTextures(1, &mDepthAttachmentId));
		}
	}

    mFramebufferId = 0;
    mColorAttachmentId = 0;
    mDepthAttachmentId = 0;
}

void SvrRenderTarget::Bind()
{
    if (mRefCount != 0)
    {
        LOGE("Warning! RenderTarget (Handle = %d) is being bound without being unbound. Bind count = %d", mFramebufferId, mRefCount);
    }
    mRefCount = mRefCount + 1;

    GL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId));
}

void SvrRenderTarget::Unbind()
{
    if (mRefCount == 0)
    {
        LOGE("Warning! RenderTarget (Handle = %d) is being unbound without being bound.", mFramebufferId);
    }
    else
    {
        mRefCount = mRefCount - 1;
    }

    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

unsigned int SvrRenderTarget::GetColorAttachment() const
{
    return mColorAttachmentId;
}

unsigned int SvrRenderTarget::GetDepthAttachment() const
{
    return mDepthAttachmentId;
}

unsigned int SvrRenderTarget::GetFrameBufferId() const
{
    return mFramebufferId;
}

int SvrRenderTarget::GetWidth() const
{
	return mWidth;
}

int SvrRenderTarget::GetHeight() const
{
	return mHeight;
}

int SvrRenderTarget::GetSamples() const
{
	return mSamples;
}

}//namespace Svr
