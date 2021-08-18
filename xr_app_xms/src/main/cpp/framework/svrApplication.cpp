//=============================================================================
// FILE: svrApplication.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "svrApplication.h"
#include "svrUtil.h"

namespace Svr
{
SvrApplication::SvrApplication()
{
    mAppContext.physicalWidth = 0;
    mAppContext.physicalHeight = 0;
    mAppContext.targetEyeWidth = 0;
    mAppContext.targetEyeHeight = 0;

    mAppContext.display = EGL_NO_DISPLAY;

    mAppContext.eyeRenderSurface = EGL_NO_SURFACE;
    mAppContext.eyeRenderContext = EGL_NO_CONTEXT;
   
    mAppContext.assetManager = NULL;
    mAppContext.nativeWindow = NULL;

    mAppContext.frameCount = 0;
    
	mAppContext.isProtectedContent = false;
    mAppContext.isColorSpaceSRGB = false;
}

SvrApplication::~SvrApplication()
{
}

void SvrApplication::Initialize()
{
    mAppContext.eyeBufferIndex = 0;
    mAppContext.cpuPerfLevel = kPerfMedium;
    mAppContext.gpuPerfLevel = kPerfMedium;
    mAppContext.trackingMode = (kTrackingRotation | kTrackingPosition);

    AllocateEyeBuffers();
}

void SvrApplication::AllocateEyeBuffers()
{
    //Default to separate render targets per eye 
    LOGI("SvrApplication - Allocating Separate Single Eye Buffers");
    for (int i = 0; i < SVR_NUM_EYE_BUFFERS; i++)
    {
        mAppContext.eyeBuffers[i].eyeTarget[kLeft].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, 1, GL_RGBA8, true);
        mAppContext.eyeBuffers[i].eyeTarget[kRight].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, 1, GL_RGBA8, true);
    }
}

void SvrApplication::Update()
{
}

void SvrApplication::Shutdown()
{
    //Free Eye Buffers
    for (int i = 0; i < SVR_NUM_EYE_BUFFERS; i++)
    {
		Svr::SvrEyeBuffer &eyeBuffer = mAppContext.eyeBuffers[i];
        for(int k=0;k<SVR_NUM_EYES;k++)
        {
            eyeBuffer.eyeTarget[k].Destroy();
        }
    }
}

SvrInput& SvrApplication::GetInput()
{
    return mInput;
}

SvrApplicationContext& SvrApplication::GetApplicationContext()
{
    return mAppContext;
}
}
