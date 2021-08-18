//=============================================================================
// FILE: svrUtil.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//============================================================================
#pragma once

#include <math.h>
#include <android/log.h>

#include "glm/glm.hpp"

#include "svrApi.h"

//#define VERBOSE_LOGGING_ENABLED 0
//
//#define LOGE(...) __android_log_print( ANDROID_LOG_ERROR, "cxr", __VA_ARGS__ )
//#define LOGI(...) __android_log_print( ANDROID_LOG_INFO, "cxr", __VA_ARGS__ )
//#define LOGW(...) __android_log_print( ANDROID_LOG_WARN, "cxr", __VA_ARGS__ )
//
//#if defined(NDEBUG) || !VERBOSE_LOGGING_ENABLED
//#define LOGV(...)
//#else
//#define LOGV(...) __android_log_print( ANDROID_LOG_VERBOSE, "svr", __VA_ARGS__ )
//#endif

// Some TypeDefs for variable types
typedef unsigned int    uint32;

namespace Svr
{
    void SvrCheckGlError(const char* file, int line);
    void SvrCheckEglError(const char* file, int line);

    void SvrGetEyeViewMatrices(const svrHeadPoseState& poseState, bool bUseHeadModel,
                                float ipd, float headHeight, float headDepth,
                                glm::mat4& outLeftEyeMatrix, glm::mat4& outRightEyeMatrix);
								
	glm::vec3 SvrPosePositionCameraSpaceToWorldSpace(const glm::vec3& posePosition);
	glm::quat SvrPoseRotationCameraSpaceToWorldSpace(const glm::quat& poseRotation);

	void L_CreateLayout(float centerX, float centerY, float radiusX, float radiusY, svrLayoutCoords *pLayout);
};

#if defined(NDEBUG)
#define GL( func ) func;
#define EGL( func ) func;
#else
#define GL( func ) func; Svr::SvrCheckGlError(__FILE__,__LINE__);
#define EGL( func ) func; Svr::SvrCheckEglError(__FILE__,__LINE__);
#endif


#ifndef DEG_TO_RAD
#define DEG_TO_RAD (float)M_PI / 180.0f
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (float)180.0f / M_PI
#endif

#ifndef NANOSECONDS_TO_MILLISECONDS
#define NANOSECONDS_TO_MILLISECONDS 0.000001f
#define MILLISECONDS_TO_NANOSECONDS 1000000.0f
#endif

#define DEFAULT_HEAD_HEIGHT     0.0750f
#define DEFAULT_HEAD_DEPTH      0.0805f
#define DEFAULT_IPD             0.0620f

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// Need a string to tell us are architecture (https://github.com/googlesamples/android-ndk/blob/master/hello-jni/app/src/main/cpp/hello-jni.c)
#if defined(__arm__)
    #if defined(__ARM_ARCH_7A__)
        #if defined(__ARM_NEON__)
            #if defined(__ARM_PCS_VFP)
                #define ABI_STRING "armeabi-v7a/NEON (hard-float)"
            #else
                #define ABI_STRING "armeabi-v7a/NEON"
            #endif
        #else
            #if defined(__ARM_PCS_VFP)
                #define ABI_STRING "armeabi-v7a (hard-float)"
            #else
                #define ABI_STRING "armeabi-v7a"
            #endif
        #endif
    #else
        #define ABI_STRING "armeabi"
    #endif
#elif defined(__i386__)
#define ABI_STRING "x86"
#elif defined(__x86_64__)
#define ABI_STRING "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define ABI_STRING "mips64"
#elif defined(__mips__)
#define ABI_STRING "mips"
#elif defined(__aarch64__)
#define ABI_STRING "arm64-v8a"
#else
#define ABI_STRING "Unknown ABI"
#endif


