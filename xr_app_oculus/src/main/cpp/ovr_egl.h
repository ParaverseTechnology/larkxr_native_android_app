//
// Created by fcx@pingxingyun.com on 2019/10/24.
//

#ifndef MY_APPLICATION_OVR_EGL_H
#define MY_APPLICATION_OVR_EGL_H

#include <EGL/egl.h>

class OvrEgl {
public:
    OvrEgl();
    ~OvrEgl();

    void clear();
    bool createContext();
    bool destoryContext();

    inline EGLint  getMajorVersion() { return mMajorVersion; };
    inline EGLint  getMinorVersion() { return mMinorVersion; };
    inline EGLDisplay  getDisplay() { return mDisplay; };
    inline EGLContext getContext() { return mContext; };

    inline bool getMultiView() { return mMultiView; };
    inline bool getEXTTextureBorderClamp() { return mEXTTextureBorderClamp; };
private:
    void eglInitExtensions();

    EGLint		mMajorVersion;
    EGLint		mMinorVersion;
    EGLDisplay	mDisplay;
    EGLConfig	mConfig;
    EGLSurface	mTinySurface;
    EGLSurface	mMainSurface;
    EGLContext	mContext;

    // extends
    bool mMultiView;
    bool mEXTTextureBorderClamp;
};


#endif //MY_APPLICATION_OVR_EGL_H
