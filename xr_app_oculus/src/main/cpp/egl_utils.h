//
// Created by fcx@pingxingyun.com on 2019/10/24.
//

#ifndef MY_APPLICATION_EGL_UTILS_H
#define MY_APPLICATION_EGL_UTILS_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <string.h>
#include "log.h"


#if !defined( GL_EXT_multisampled_render_to_texture )
typedef void (GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);
#endif

#if !defined( GL_OVR_multiview )
//static const int GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR       = 0x9630;
//static const int GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR = 0x9632;
//static const int GL_MAX_VIEWS_OVR                                      = 0x9631;
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews);
#endif

#if !defined( GL_OVR_multiview_multisampled_render_to_texture )
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseViewIndex, GLsizei numViews);
#endif

// EXT_texture_border_clamp
#ifndef GL_CLAMP_TO_BORDER
#define GL_CLAMP_TO_BORDER			0x812D
#endif

#ifndef GL_TEXTURE_BORDER_COLOR
#define GL_TEXTURE_BORDER_COLOR		0x1004
#endif

#ifdef CHECK_GL_ERRORS

#define GL( func )		func; egl_utils::GLCheckErrors( __LINE__ );

#else // CHECK_GL_ERRORS

#define GL( func )		func;

#endif // CHECK_GL_ERRORS

namespace egl_utils {
    typedef struct
    {
        bool multi_view;					// GL_OVR_multiview, GL_OVR_multiview2
        bool EXT_texture_border_clamp;		// GL_EXT_texture_border_clamp, GL_OES_texture_border_clamp
    } OpenGLExtensions_t;
    static OpenGLExtensions_t glExtensions;

    static inline void EglInitExtensions()
    {
        const char * allExtensions = (const char *)glGetString( GL_EXTENSIONS );
        if ( allExtensions != NULL )
        {
            glExtensions.multi_view = strstr( allExtensions, "GL_OVR_multiview2" ) &&
                                      strstr( allExtensions, "GL_OVR_multiview_multisampled_render_to_texture" );

            glExtensions.EXT_texture_border_clamp = strstr( allExtensions, "GL_EXT_texture_border_clamp" ) ||
                                                    strstr( allExtensions, "GL_OES_texture_border_clamp" );
        }
    }

    static inline const char * EglErrorString( const EGLint error )
    {
        switch ( error )
        {
            case EGL_SUCCESS:				return "EGL_SUCCESS";
            case EGL_NOT_INITIALIZED:		return "EGL_NOT_INITIALIZED";
            case EGL_BAD_ACCESS:			return "EGL_BAD_ACCESS";
            case EGL_BAD_ALLOC:				return "EGL_BAD_ALLOC";
            case EGL_BAD_ATTRIBUTE:			return "EGL_BAD_ATTRIBUTE";
            case EGL_BAD_CONTEXT:			return "EGL_BAD_CONTEXT";
            case EGL_BAD_CONFIG:			return "EGL_BAD_CONFIG";
            case EGL_BAD_CURRENT_SURFACE:	return "EGL_BAD_CURRENT_SURFACE";
            case EGL_BAD_DISPLAY:			return "EGL_BAD_DISPLAY";
            case EGL_BAD_SURFACE:			return "EGL_BAD_SURFACE";
            case EGL_BAD_MATCH:				return "EGL_BAD_MATCH";
            case EGL_BAD_PARAMETER:			return "EGL_BAD_PARAMETER";
            case EGL_BAD_NATIVE_PIXMAP:		return "EGL_BAD_NATIVE_PIXMAP";
            case EGL_BAD_NATIVE_WINDOW:		return "EGL_BAD_NATIVE_WINDOW";
            case EGL_CONTEXT_LOST:			return "EGL_CONTEXT_LOST";
            default:						return "unknown";
        }
    }

    static inline const char * GlFrameBufferStatusString( GLenum status )
    {
        switch ( status )
        {
            case GL_FRAMEBUFFER_UNDEFINED:						return "GL_FRAMEBUFFER_UNDEFINED";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            case GL_FRAMEBUFFER_UNSUPPORTED:					return "GL_FRAMEBUFFER_UNSUPPORTED";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            default:											return "unknown";
        }
    }

    static const char * GlErrorString( GLenum error )
    {
        switch ( error )
        {
            case GL_NO_ERROR:						return "GL_NO_ERROR";
            case GL_INVALID_ENUM:					return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE:					return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION:				return "GL_INVALID_OPERATION";
            case GL_INVALID_FRAMEBUFFER_OPERATION:	return "GL_INVALID_FRAMEBUFFER_OPERATION";
            case GL_OUT_OF_MEMORY:					return "GL_OUT_OF_MEMORY";
            default: return "unknown";
        }
    }

    static void GLCheckErrors( int line )
    {
        for ( int i = 0; i < 10; i++ )
        {
            const GLenum error = glGetError();
            if ( error == GL_NO_ERROR )
            {
                break;
            }
            LOGE( "GL error on line %d: %s", line, GlErrorString( error ) );
        }
    }
};


#endif //MY_APPLICATION_EGL_UTILS_H
