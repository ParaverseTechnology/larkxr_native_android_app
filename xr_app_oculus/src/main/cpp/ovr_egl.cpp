//
// Created by fcx@pingxingyun.com on 2019/10/24.
//

#include "ovr_egl.h"
#include "log.h"
#include "egl_utils.h"

OvrEgl::OvrEgl():
    mMajorVersion(0)
    ,mMinorVersion(0)
    ,mDisplay(nullptr)
    ,mConfig(nullptr)
    ,mTinySurface(EGL_NO_SURFACE)
    ,mMainSurface(EGL_NO_SURFACE)
    ,mContext(EGL_NO_CONTEXT)
    ,mMultiView(false)
    ,mEXTTextureBorderClamp(false)
{
}

OvrEgl::~OvrEgl() {
    clear();
}

void OvrEgl::clear() {
    mMajorVersion = 0;
    mMinorVersion = 0;
    mDisplay = nullptr;
    mConfig = nullptr;
    mTinySurface = EGL_NO_SURFACE;
    mMainSurface = EGL_NO_SURFACE;
    mContext = EGL_NO_CONTEXT;
    mMultiView = false;
    mEXTTextureBorderClamp = false;
}

bool OvrEgl::createContext() {
    mDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    eglInitialize(mDisplay, &mMajorVersion, &mMinorVersion);
    // Do NOT use eglChooseConfig, because the Android EGL code pushes in multisample
    // flags in eglChooseConfig if the user has selected the "force 4x MSAA" option in
    // settings, and that is completely wasted for our warp target.
    const int MAX_CONFIGS = 1024;
    EGLConfig configs[MAX_CONFIGS];
    EGLint numConfigs = 0;
    if (eglGetConfigs( mDisplay, configs, MAX_CONFIGS, &numConfigs ) == EGL_FALSE)
    {
        LOGE( "eglGetConfigs() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        return false;
    }
    const EGLint configAttribs[] =
            {
                    EGL_RED_SIZE,		8,
                    EGL_GREEN_SIZE,		8,
                    EGL_BLUE_SIZE,		8,
                    EGL_ALPHA_SIZE,		8, // need alpha for the multi-pass timewarp compositor
                    EGL_DEPTH_SIZE,		0,
                    EGL_STENCIL_SIZE,	0,
                    EGL_SAMPLES,		0,
                    EGL_NONE
            };
    mConfig = nullptr;

    for ( int i = 0; i < numConfigs; i++ )
    {
        EGLint value = 0;

        eglGetConfigAttrib( mDisplay, configs[i], EGL_RENDERABLE_TYPE, &value );
        if ( ( value & EGL_OPENGL_ES3_BIT_KHR ) != EGL_OPENGL_ES3_BIT_KHR )
        {
            continue;
        }

        // The pbuffer config also needs to be compatible with normal window rendering
        // so it can share textures with the window context.
        eglGetConfigAttrib( mDisplay, configs[i], EGL_SURFACE_TYPE, &value );
        if ( ( value & ( EGL_WINDOW_BIT | EGL_PBUFFER_BIT ) ) != ( EGL_WINDOW_BIT | EGL_PBUFFER_BIT ) )
        {
            continue;
        }

        int	j = 0;
        for ( ; configAttribs[j] != EGL_NONE; j += 2 )
        {
            eglGetConfigAttrib( mDisplay, configs[i], configAttribs[j], &value );
            if ( value != configAttribs[j + 1] )
            {
                break;
            }
        }
        if ( configAttribs[j] == EGL_NONE )
        {
            mConfig = configs[i];
            break;
        }
    }

    if ( mConfig == nullptr )
    {
        LOGE( "        eglChooseConfig() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        return false;
    }

    EGLint contextAttribs[] =
            {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
    LOGV( "        Context = eglCreateContext( Display, Config, EGL_NO_CONTEXT, contextAttribs )" );
    mContext = eglCreateContext( mDisplay, mConfig, EGL_NO_CONTEXT, contextAttribs );

    if (  mContext == EGL_NO_CONTEXT )
    {
        LOGE( "        eglCreateContext() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        return false;
    }

    const EGLint surfaceAttribs[] =
            {
                    EGL_WIDTH, 16,
                    EGL_HEIGHT, 16,
                    EGL_NONE
            };
    LOGV( "        TinySurface = eglCreatePbufferSurface( Display, Config, surfaceAttribs )" );

    mTinySurface = eglCreatePbufferSurface( mDisplay, mConfig, surfaceAttribs );
    if ( mTinySurface == EGL_NO_SURFACE )
    {
        LOGE( "        eglCreatePbufferSurface() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        eglDestroyContext( mDisplay, mContext );
        mContext = EGL_NO_CONTEXT;
        return false;
    }
    LOGV( "        eglMakeCurrent( Display, TinySurface, TinySurface, Context )" );
    if ( eglMakeCurrent( mDisplay, mTinySurface, mTinySurface, mContext ) == EGL_FALSE )
    {
        LOGE( "        eglMakeCurrent() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        eglDestroySurface( mDisplay, mTinySurface );
        eglDestroyContext( mDisplay, mContext );
        mContext = EGL_NO_CONTEXT;
        return false;
    }

    eglInitExtensions();
    // TODO remove this
    egl_utils::EglInitExtensions();
    return true;
}

bool OvrEgl::destoryContext() {
    if ( mDisplay != nullptr )
    {
        LOGE( "        destoryContext eglMakeCurrent( Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT )" );
        if ( eglMakeCurrent( mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT ) == EGL_FALSE )
        {
            LOGE( "        eglMakeCurrent() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        }
    }
    if ( mContext != EGL_NO_CONTEXT )
    {
        LOGE( "        destoryContext eglDestroyContext( Display, Context )" );
        if ( eglDestroyContext( mDisplay, mContext ) == EGL_FALSE )
        {
            LOGE( "        eglDestroyContext() failed: %s",  egl_utils::EglErrorString( eglGetError() ) );
        }
        mContext = EGL_NO_CONTEXT;
    }
    if ( mTinySurface != EGL_NO_SURFACE )
    {
        LOGE( "        destoryContext eglDestroySurface( Display, TinySurface )" );
        if ( eglDestroySurface( mDisplay, mTinySurface ) == EGL_FALSE )
        {
            LOGE( "        eglDestroySurface() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        }
        mTinySurface = EGL_NO_SURFACE;
    }
    if ( mDisplay != nullptr )
    {
        LOGE( "        destoryContext eglTerminate( Display )" );
        if ( eglTerminate( mDisplay ) == EGL_FALSE )
        {
            LOGE( "        eglTerminate() failed: %s", egl_utils::EglErrorString( eglGetError() ) );
        }
        mDisplay = nullptr;
    }
    LOGE("         shutdown gl finished");
    return true;
}

void OvrEgl::eglInitExtensions() {
    const char * allExtensions = (const char *)glGetString( GL_EXTENSIONS );
    if ( allExtensions != nullptr )
    {
        mMultiView = strstr(allExtensions, "GL_OVR_multiview2") &&
                                  strstr(allExtensions, "GL_OVR_multiview_multisampled_render_to_texture" );

        mEXTTextureBorderClamp = strstr(allExtensions, "GL_EXT_texture_border_clamp") ||
                                                strstr(allExtensions, "GL_OES_texture_border_clamp" );
    }
}
