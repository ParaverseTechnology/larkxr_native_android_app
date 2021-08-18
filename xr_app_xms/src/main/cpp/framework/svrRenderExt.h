//=============================================================================
// FILE: svrRenderExt.h
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#ifndef GL_APIENTRYP
#define GL_APIENTRYP GL_APIENTRY*
#endif

// From GLES2/gl2ext.h
typedef void (GL_APIENTRYP PFNGLSTARTTILINGQCOMPROC) (GLuint x, GLuint y, GLuint width, GLuint height, GLbitfield preserveMask);
typedef void (GL_APIENTRYP PFNGLENDTILINGQCOMPROC) (GLbitfield preserveMask);

typedef void *GLeglImageOES;
typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) (GLenum target, GLeglImageOES image);

// From gl3extQCOM.h
typedef void (GL_APIENTRYP PFNGLGENMEMORYOBJECTSKHR)(GLsizei n, GLuint *memoryObjects);
typedef void (GL_APIENTRYP PFNGLDELETEMEMORYOBJECTSKHR)(GLsizei n, const GLuint *memoryObjects);

typedef void (GL_APIENTRYP PFNGLIMPORTMEMORYFDKHR)(GLuint memory, GLuint64 size, GLenum handleType, GLint fd);
typedef void (GL_APIENTRYP PFNGLTEXSTORAGEMEM2DKHR)(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset);

// For later
typedef void (GL_APIENTRYP PFNGLCREATEMEMORYOBJECTSEXT)(GLsizei n, GLuint* memoryObjects);
typedef void (GL_APIENTRYP PFNGLDELETEMEMORYOBJECTSEXT)(GLsizei n, const GLuint* memoryObjects);

typedef void (GL_APIENTRYP PFNGLIMPORTMEMORYFDEXT)(GLuint memory, GLuint64 size, GLenum handleType, GLint fd);
typedef void (GL_APIENTRYP PFNGLTEXSTORAGEMEM2DEXT)(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset);

#ifndef GL_COLOR_BUFFER_BIT0_QCOM
#define GL_COLOR_BUFFER_BIT0_QCOM       0x00000001
#endif 

namespace Svr
{
    void* InitExtension(const char* extName, const char* alternateExtName = NULL);
    bool InitializeRenderExtensions();

    extern PFNEGLCREATESYNCKHRPROC              eglCreateSyncKHR;
    extern PFNEGLDESTROYSYNCKHRPROC             eglDestroySyncKHR;
    extern PFNEGLCLIENTWAITSYNCKHRPROC          eglClientWaitSyncKHR;

    extern PFNEGLGETSYNCATTRIBKHRPROC           eglGetSyncAttribKHR;

    extern PFNGLSTARTTILINGQCOMPROC             glStartTilingQCOM;
    extern PFNGLENDTILINGQCOMPROC               glEndTilingQCOM;

    extern PFNGLEGLIMAGETARGETTEXTURE2DOESPROC  glEGLImageTargetTexture2DOES;

    extern PFNGLGENMEMORYOBJECTSKHR             glGenMemoryObjectsKHR;
    extern PFNGLIMPORTMEMORYFDKHR               glImportMemoryFdKHR;
    extern PFNGLTEXSTORAGEMEM2DKHR              glTexStorageMem2DKHR;
    extern PFNGLDELETEMEMORYOBJECTSKHR          glDeleteMemoryObjectsKHR;

#ifdef ENABLE_MOTION_VECTORS
    extern PFNEGLCREATEIMAGEKHRPROC            eglCreateImageKHR;
    extern PFNEGLDESTROYIMAGEKHRPROC           eglDestroyImageKHR;
    // extern PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
#endif //ENABLE_MOTION_VECTORS

    // For later
    extern PFNGLCREATEMEMORYOBJECTSEXT          glCreateMemoryObjectsEXT;
    extern PFNGLDELETEMEMORYOBJECTSEXT          glDeleteMemoryObjectsEXT;
    extern PFNGLIMPORTMEMORYFDEXT               glImportMemoryFdEXT;
    extern PFNGLTEXSTORAGEMEM2DEXT              glTexStorageMem2DEXT;

} //End namespace Svr
