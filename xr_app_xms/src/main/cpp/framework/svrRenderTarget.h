//=============================================================================
// FILE: svrRenderTarget.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

namespace Svr
{
    class SvrRenderTarget
    {
    public:
        SvrRenderTarget();
		~SvrRenderTarget();

        void Initialize(int width, int height, int samples, int colorSizedFormat, bool requiresDepth, bool isProtectedContent = false);
        void Destroy();
        void Bind();
        void Unbind();

        unsigned int GetColorAttachment() const;
        unsigned int GetDepthAttachment() const;
        unsigned int GetFrameBufferId() const;

		int GetWidth() const;
		int GetHeight() const;
		int GetSamples() const;

    private:
		void InitializeSingleSample(int width, int height, int samples, int colorSizedFormat, int format, int type, bool requiresDepth);
		void InitializeMultiSample(int width, int height, int samples, int colorSizedFormat, int format, bool requiresDepth);

        int             mWidth;
        int             mHeight;
        int             mSamples;

        unsigned int    mRefCount;

        unsigned int mColorAttachmentId;
        unsigned int mDepthAttachmentId;
        unsigned int mFramebufferId;
        bool 		 mIsProtectedContent;
    };
}
