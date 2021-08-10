//=============================================================================
// FILE: svrUtil.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//============================================================================

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <log.h>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "svrUtil.h"

namespace Svr
{

    void SvrCheckGlError(const char* file, int line)
    {
        for (GLint error = glGetError(); error; error = glGetError())
        {
            char *pError;
            switch (error)
            {
            case GL_NO_ERROR:                       pError = (char *)"GL_NO_ERROR";                         break;
            case GL_INVALID_ENUM:                   pError = (char *)"GL_INVALID_ENUM";                     break;
            case GL_INVALID_VALUE:                  pError = (char *)"GL_INVALID_VALUE";                    break;
            case GL_INVALID_OPERATION:              pError = (char *)"GL_INVALID_OPERATION";                break;
            case GL_OUT_OF_MEMORY:                  pError = (char *)"GL_OUT_OF_MEMORY";                    break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  pError = (char *)"GL_INVALID_FRAMEBUFFER_OPERATION";    break;

            default:
                LOGE("glError (0x%x) %s:%d\n", error, file, line);
                return;
            }

            LOGE("glError (%s) %s:%d\n", pError, file, line);
            return;
        }
        return;
    }

    void SvrCheckEglError(const char* file, int line)
    {
        for (int i = 0; i < 10; i++)
        {
            const EGLint error = eglGetError();
            if (error == EGL_SUCCESS)
            {
                break;
            }

            char *pError;
            switch (error)
            {
            case EGL_SUCCESS:				pError = (char *)"EGL_SUCCESS"; break;
            case EGL_NOT_INITIALIZED:		pError = (char *)"EGL_NOT_INITIALIZED"; break;
            case EGL_BAD_ACCESS:			pError = (char *)"EGL_BAD_ACCESS"; break;
            case EGL_BAD_ALLOC:				pError = (char *)"EGL_BAD_ALLOC"; break;
            case EGL_BAD_ATTRIBUTE:			pError = (char *)"EGL_BAD_ATTRIBUTE"; break;
            case EGL_BAD_CONTEXT:			pError = (char *)"EGL_BAD_CONTEXT"; break;
            case EGL_BAD_CONFIG:			pError = (char *)"EGL_BAD_CONFIG"; break;
            case EGL_BAD_CURRENT_SURFACE:	pError = (char *)"EGL_BAD_CURRENT_SURFACE"; break;
            case EGL_BAD_DISPLAY:			pError = (char *)"EGL_BAD_DISPLAY"; break;
            case EGL_BAD_SURFACE:			pError = (char *)"EGL_BAD_SURFACE"; break;
            case EGL_BAD_MATCH:				pError = (char *)"EGL_BAD_MATCH"; break;
            case EGL_BAD_PARAMETER:			pError = (char *)"EGL_BAD_PARAMETER"; break;
            case EGL_BAD_NATIVE_PIXMAP:		pError = (char *)"EGL_BAD_NATIVE_PIXMAP"; break;
            case EGL_BAD_NATIVE_WINDOW:		pError = (char *)"EGL_BAD_NATIVE_WINDOW"; break;
            case EGL_CONTEXT_LOST:			pError = (char *)"EGL_CONTEXT_LOST"; break;
            default:
                LOGE("eglError (0x%x) %s:%d\n", error, file, line);
                return;
            }
            LOGE("eglError (%s) %s:%d\n", pError, file, line);
            return;
        }
        return;
    }

    void SvrGetEyeViewMatrices(const svrHeadPoseState& poseState, bool bUseHeadModel,
                               float ipd, float headHeight, float headDepth, glm::mat4& outLeftEyeMatrix, glm::mat4& outRightEyeMatrix)
    {
        glm::fquat poseQuat = glm::fquat(poseState.pose.rotation.w, poseState.pose.rotation.x, poseState.pose.rotation.y, poseState.pose.rotation.z);
        glm::mat4 poseMat = glm::mat4_cast(poseQuat);

        glm::vec4 headOffset;

        if (bUseHeadModel && 
            ((poseState.poseStatus | kTrackingPosition) == 0 ))
        {
            //Only use a head model if the uses has chosen to enable it and positional tracking is not enabled
            headOffset = glm::vec4(0.0f, headHeight, -headDepth, 1.0f);
        }
        else
        {
            if ((poseState.poseStatus | kTrackingPosition) != 0)
            {
                //Positional tracking is enabled so use that positional data from the pose 
                headOffset.x = poseState.pose.position.x;
                headOffset.y = poseState.pose.position.y;
                headOffset.z = poseState.pose.position.z;
            }
            else
            {
                //No head model, no positional tracking data
                headOffset = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

                // But the user may have set the desired position in the pose state
                headOffset.x = poseState.pose.position.x;
                headOffset.y = poseState.pose.position.y;
                headOffset.z = poseState.pose.position.z;
            }
        }

        glm::mat4 leftEyeOffsetMat = glm::translate(glm::mat4(1.0f), glm::vec3((-0.5f * ipd) + headOffset.x, headOffset.y, headOffset.z));
        glm::mat4 rightEyeOffsetMat = glm::translate(glm::mat4(1.0f), glm::vec3((0.5f * ipd) + headOffset.x, headOffset.y, headOffset.z));

        outLeftEyeMatrix = poseMat * leftEyeOffsetMat;
        outRightEyeMatrix = poseMat * rightEyeOffsetMat;
    }

	glm::vec3 SvrPosePositionCameraSpaceToWorldSpace(const glm::vec3& posePosition)
	{
		return -posePosition;
	}
	glm::quat SvrPoseRotationCameraSpaceToWorldSpace(const glm::quat& poseRotation)
	{
		return conjugate(poseRotation);
	}

    void L_CreateLayout(float centerX, float centerY, float radiusX, float radiusY, svrLayoutCoords *pLayout)
    {
        // This is always in screen space so we want Z = 0 and W = 1
        float lowerLeftPos[4] = { centerX - radiusX, centerY - radiusY, 0.0f, 1.0f };
        float lowerRightPos[4] = { centerX + radiusX, centerY - radiusY, 0.0f, 1.0f };
        float upperLeftPos[4] = { centerX - radiusX, centerY + radiusY, 0.0f, 1.0f };
        float upperRightPos[4] = { centerX + radiusX, centerY + radiusY, 0.0f, 1.0f };

        float lowerUVs[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
        float upperUVs[4] = { 0.0f, 1.0f, 1.0f, 1.0f };

        memcpy(pLayout->LowerLeftPos, lowerLeftPos, sizeof(lowerLeftPos));
        memcpy(pLayout->LowerRightPos, lowerRightPos, sizeof(lowerRightPos));
        memcpy(pLayout->UpperLeftPos, upperLeftPos, sizeof(upperLeftPos));
        memcpy(pLayout->UpperRightPos, upperRightPos, sizeof(upperRightPos));

        memcpy(pLayout->LowerUVs, lowerUVs, sizeof(lowerUVs));
        memcpy(pLayout->UpperUVs, upperUVs, sizeof(upperUVs));

        float identTransform[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f };
        memcpy(pLayout->TransformMatrix, identTransform, sizeof(pLayout->TransformMatrix));

    }
}//End namespace Svr
