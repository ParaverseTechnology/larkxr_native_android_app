//=============================================================================
// FILE: svrRenderExt.cpp
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#include <log.h>
#include "svrRenderExt.h"
#include "svrUtil.h"

namespace Svr
{
    PFNEGLCREATESYNCKHRPROC                 eglCreateSyncKHR;
    PFNEGLDESTROYSYNCKHRPROC                eglDestroySyncKHR;
    PFNEGLCLIENTWAITSYNCKHRPROC             eglClientWaitSyncKHR;

    PFNGLGENMEMORYOBJECTSKHR                glGenMemoryObjectsKHR;
    PFNGLIMPORTMEMORYFDKHR                  glImportMemoryFdKHR;
    PFNGLTEXSTORAGEMEM2DKHR                 glTexStorageMem2DKHR;
    PFNGLDELETEMEMORYOBJECTSKHR             glDeleteMemoryObjectsKHR;

    PFNEGLGETSYNCATTRIBKHRPROC              eglGetSyncAttribKHR;

    // For later
    PFNGLCREATEMEMORYOBJECTSEXT             glCreateMemoryObjectsEXT;
    PFNGLDELETEMEMORYOBJECTSEXT             glDeleteMemoryObjectsEXT;
    PFNGLIMPORTMEMORYFDEXT                  glImportMemoryFdEXT;
    PFNGLTEXSTORAGEMEM2DEXT                 glTexStorageMem2DEXT;

    PFNGLSTARTTILINGQCOMPROC                glStartTilingQCOM;
    PFNGLENDTILINGQCOMPROC                  glEndTilingQCOM;

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC     glEGLImageTargetTexture2DOES;

#ifdef ENABLE_MOTION_VECTORS
    PFNEGLCREATEIMAGEKHRPROC            eglCreateImageKHR = NULL;
    PFNEGLDESTROYIMAGEKHRPROC           eglDestroyImageKHR = NULL;
    // PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;
#endif //ENABLE_MOTION_VECTORS

    void* InitExtension(const char* extName, const char* alternateExtName)
    {
        LOGI("Initializing Function: %s", extName);
        void* pExt = (void*)eglGetProcAddress(extName);
        if (NULL == pExt)
        {
            if (NULL != alternateExtName)
            {
                LOGI("    Failed to locate extension: %s", extName);
                LOGI("    Trying Function: %s", alternateExtName);
                pExt = (void*)eglGetProcAddress(alternateExtName);
                if (NULL == pExt)
                {
                    LOGE("    Failed to locate extension: %s", alternateExtName);
                }
                else
                {
                    LOGI("    Function initialized: %s", alternateExtName);
                }
            }
            else
            {
                LOGE("    Failed to locate extension: %s", extName);
            }
        }
        else
        {
            LOGI("    Function initialized: %s", extName);
        }

        return pExt;
    }

    bool InitializeRenderExtensions()
    {
        eglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)InitExtension("eglCreateSyncKHR");
        eglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)InitExtension("eglDestroySyncKHR");
        eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)InitExtension("eglClientWaitSyncKHR");

        eglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)InitExtension("eglGetSyncAttribKHR");

        glGenMemoryObjectsKHR = (PFNGLGENMEMORYOBJECTSKHR)InitExtension("glGenMemoryObjectsKHR", "glCreateMemoryObjectsEXT");
        glImportMemoryFdKHR = (PFNGLIMPORTMEMORYFDKHR)InitExtension("glImportMemoryFdKHR", "glImportMemoryFdEXT");
        glTexStorageMem2DKHR = (PFNGLTEXSTORAGEMEM2DKHR)InitExtension("glTexStorageMem2DKHR", "glTexStorageMem2DEXT");
        glDeleteMemoryObjectsKHR = (PFNGLDELETEMEMORYOBJECTSKHR)InitExtension("glDeleteMemoryObjectsKHR", "glDeleteMemoryObjectsEXT");

        glStartTilingQCOM = (PFNGLSTARTTILINGQCOMPROC)InitExtension("glStartTilingQCOM");
        glEndTilingQCOM = (PFNGLENDTILINGQCOMPROC)InitExtension("glEndTilingQCOM");

        glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)InitExtension("glEGLImageTargetTexture2DOES");

#ifdef ENABLE_MOTION_VECTORS
        eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)InitExtension("eglCreateImageKHR");
        eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)InitExtension("eglDestroyImageKHR");
        // glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)InitExtension("glEGLImageTargetTexture2DOES");
#endif //ENABLE_MOTION_VECTORS

        return true;
    }
}//End namespace Svr