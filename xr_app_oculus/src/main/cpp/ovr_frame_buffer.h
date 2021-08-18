//
// Created by fcx@pingxingyun.com on 2019/11/6.
//

#ifndef MY_APPLICATION_FRAME_BUFFER_H
#define MY_APPLICATION_FRAME_BUFFER_H


#include <VrApi_Ext.h>
#include <GLES3/gl3.h>

class OvrFrameBuffer {
public:
    OvrFrameBuffer();
    ~OvrFrameBuffer();

    void clear();
    bool create(const bool useMultiview, const GLenum colorFormat, const int width, const int height, const int multisamples);
    void destory();
    void setCurrent();
    void setNone();
    void resolve();
    void advance();

    int getWidth() { return mWidth; }
    int getHeight() { return mHeight; }
    int getMultisamples() { return mMultisamples; }
    int getTextureSwapChainLength() { return mTextureSwapChainLength; }
    int getTextureSwapChainIndex() { return mTextureSwapChainIndex; }
    bool getUseMultiview() { return use_multiview_; }
    ovrTextureSwapChain * getOvrTextureSwapChain() { return mColorTextureSwapChain; }
    GLuint * getDepthBuffers() { return mDepthBuffers; }
    GLuint * getFrameBuffers() { return mFrameBuffers; }
    bool getCreated() { return mCreated; }
private:
    int						mWidth;
    int						mHeight;
    int						mMultisamples;
    int						mTextureSwapChainLength;
    int						mTextureSwapChainIndex;
    bool					use_multiview_;
    ovrTextureSwapChain *	mColorTextureSwapChain;
    GLuint *				mDepthBuffers;
    GLuint *				mFrameBuffers;

    bool                    mCreated;
};


#endif //MY_APPLICATION_FRAME_BUFFER_H
