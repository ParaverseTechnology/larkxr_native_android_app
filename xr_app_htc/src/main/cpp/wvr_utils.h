//
// Created by fcx on 2020/5/7.
//

#ifndef CLOUDLARKXR_WVR_UTILS_H
#define CLOUDLARKXR_WVR_UTILS_H

#include <wvr/wvr_render.h>
#include <wvr/wvr_device.h>
#include "wvr/wvr_projection.h"
#include "wvr/wvr_device.h"
#include "matrices.h"
#include <wvr/wvr_types.h>
#include <wvr/wvr_events.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <lark_xr/types.h>

namespace wvr {
    inline glm::quat dot(const glm::mat4& mat4, const glm::quat& quat) {
        return glm::quat(
                mat4[0][0] * quat.z + mat4[1][0] * quat.z + mat4[2][0] * quat.z + mat4[3][0] * quat.w,
                mat4[0][1] * quat.z + mat4[1][1] * quat.z + mat4[2][1] * quat.z + mat4[3][1] * quat.w,
                mat4[0][2] * quat.z + mat4[1][2] * quat.z + mat4[2][2] * quat.z + mat4[3][2] * quat.w,
                mat4[0][3] * quat.z + mat4[1][3] * quat.z + mat4[2][3] * quat.z + mat4[3][3] * quat.w);
    }

    inline glm::mat4 toGlm(const WVR_Matrix4f_t& wm) {
        return glm::transpose(glm::make_mat4(&wm.m[0][0]));
    }
    inline glm::vec3 toGlm(const WVR_Vector3f& wv) {
        glm::vec3 result{};
        result.x = wv.v[0];
        result.y = wv.v[1];
        result.z = wv.v[2];
        return result;
    }
    inline glm::vec2 toGlm(const WVR_Vector2f& wv) {
        glm::vec2 result{};
        result.x = wv.v[0];
        result.y = wv.v[1];
        return result;
    }
    inline glm::quat toGlm(const WVR_Quatf& wq) {
        glm::quat result{};
        result.x = wq.x;
        result.y = wq.y;
        result.z = wq.z;
        result.w = wq.w;
        return result;
    }

    inline WVR_Matrix4f_t fromGlm(const glm::mat4 & m) {
        WVR_Matrix4f_t result = {};
        glm::mat4 transposed(glm::transpose(m));
        memcpy(result.m, &(transposed[0][0]), sizeof(float) * 16);
        return result;
    }

    inline WVR_Vector3f fromGlm(const glm::vec3 & v) {
        WVR_Vector3f result;
        result.v[0] = v.x;
        result.v[1] = v.y;
        result.v[2] = v.z;
        return result;
    }

    inline WVR_Vector2f fromGlm(const glm::vec2 & v) {
        WVR_Vector2f result;
        result.v[0] = v.x;
        result.v[1] = v.y;
        return result;
    }

    inline WVR_Quatf fromGlm(const glm::quat & q) {
        WVR_Quatf result;
        result.x = q.x;
        result.y = q.y;
        result.z = q.z;
        result.w = q.w;
        return result;
    }

    inline glm::vec3 getPosition(const WVR_Matrix4f_t& matrix4) {
        glm::vec3 result;
        result.x = matrix4.m[0][3];
        result.y = matrix4.m[1][3];
        result.z = matrix4.m[2][3];
        return result;
    }

    inline glm::quat getRotation(const WVR_Matrix4f_t& matrix4) {
        return glm::quat_cast<float, glm::defaultp>(wvr::toGlm(matrix4));
    }

    //
    inline larkxrTrackedPose wToLarkTrackedPose(const WVR_PoseState& wvrPoseState) {
        larkxrTrackedPose pose;
        pose.isConnected = wvrPoseState.isValidPose;
        pose.is6Dof = wvrPoseState.is6DoFPose;
        pose.isValidPose = wvrPoseState.isValidPose;
        pose.timestamp = wvrPoseState.timestamp;
        pose.predictedMilliSec = wvrPoseState.predictedMilliSec;
        pose.poseOriginModel = static_cast<larkxrPoseOriginModel>(wvrPoseState.originModel);
        pose.position = getPosition(wvrPoseState.poseMatrix);
        pose.rotation = getRotation(wvrPoseState.poseMatrix);
        pose.velocity = toGlm(wvrPoseState.velocity);
        pose.angularVelocity = toGlm(wvrPoseState.angularVelocity);
        pose.acceleration = toGlm(wvrPoseState.acceleration);
        pose.angularAcceleration = toGlm(wvrPoseState.angularVelocity);
        pose.rawPosition = toGlm(wvrPoseState.rawPose.position);
        pose.rawRotation = toGlm(wvrPoseState.rawPose.rotation);
        pose.rawPoseMatrix = toGlm(wvrPoseState.poseMatrix);
        return pose;
    }
    //    GLFWwindow * createRiftRenderingWindow(ovrHmd hmd, glm::uvec2 & outSize, glm::ivec2 & outPosition);
    //  ovr pose to lark vr hmd pose.
    inline larkxrTrackedPose wToLarkHMDTrakedPose(const WVR_PoseState& tracking) {
        larkxrTrackedPose pose = wToLarkTrackedPose(tracking);
        pose.device = Larkxr_Device_Type_HMD;
        return pose;
    }
    // to larrk controller pose
    inline larkxrTrackedPose wToLarkControllerTrackedPose(bool isLeft, const WVR_PoseState& tracking) {
        larkxrTrackedPose pose = wToLarkTrackedPose(tracking);
        pose.device = isLeft ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;
        return pose;
    }

    inline WVR_PoseState fromLarkvrTrackedPose(const larkxrTrackedPose& larkvrPose) {
        WVR_PoseState pose = {};
        pose.isValidPose = larkvrPose.isValidPose;
        pose.is6DoFPose = larkvrPose.is6Dof;
        pose.timestamp = larkvrPose.timestamp;
        pose.predictedMilliSec = larkvrPose.predictedMilliSec;
        pose.poseMatrix = fromGlm(larkvrPose.rawPoseMatrix);
        pose.angularVelocity = fromGlm(larkvrPose.angularVelocity);
        pose.velocity = fromGlm(larkvrPose.velocity);
        pose.angularVelocity = fromGlm(larkvrPose.angularAcceleration);
        pose.acceleration = fromGlm(larkvrPose.acceleration);
        pose.rawPose.position = fromGlm(larkvrPose.rawPosition);
        pose.rawPose.rotation = fromGlm(larkvrPose.rawRotation);
        return pose;
    }

    inline Matrix4 simuControllerArm(const Matrix4 &hmdMat, const Matrix4 &controllerMat, int offset) {
        Matrix4 deviceMat(controllerMat);
        deviceMat.setColumn(3, Vector4(0,0,0,1));
        Matrix4 rotMat;
        rotMat.identity();

        float angleY = atan2f(-hmdMat[8], hmdMat[10]);  // Yaw
        float angleX = asin(-hmdMat[9]);                // Pitch
        float angleZ = atan2f(hmdMat[1], hmdMat[5]);    // Roll

        deviceMat.rotateY(static_cast<float>(angleY / M_PI * 180.0f));
        deviceMat.rotateX(static_cast<float>(-angleX / M_PI * 180.0f));
        deviceMat.rotateZ(static_cast<float>(-angleZ / M_PI * 180.0f));

        float arm = 0.2f;
        float x = (offset % 2) == 0 ? 0.28f : -0.28f;
        float y = -0.28f;
        float z = -0.25f;

        deviceMat.setColumn(3, Vector4(x,y,z,1));
        rotMat.setColumn(3, Vector4(0,0,z - arm,1));

        return hmdMat * deviceMat * rotMat;
    }

    inline void SetArmControllerMatrix(const glm::mat4& hmd, WVR_DeviceType deviceType, glm::mat4* mat) {
        int offset = deviceType == WVR_DeviceType_Controller_Left ? 1 : 0;
        glm::mat4 deviceMat = *mat;
        deviceMat[3][0] = 0;
        deviceMat[3][1] = 0;
        deviceMat[3][2] = 0;

        float arm = 0.2f;
        float x = (offset % 2) == 0 ? 0.28f : -0.28f;
        float y = -0.28f;
        float z = -0.25f;

        glm::mat4 tmp = glm::identity<glm::mat4>();
        tmp[3][0] = x;
        tmp[3][1] = y;
        tmp[3][2] = z;

        glm::mat4 rotMat = glm::identity<glm::mat4>();
        rotMat[3][0] = 0;
        rotMat[3][1] = 0;
        rotMat[3][2] = z-arm;

        *mat = deviceMat * tmp * rotMat;
    }

    inline int GetBatteryPrecent(WVR_BatteryStatus status) {
        switch(status)
        {
            case WVR_BatteryStatus_Unknown:
                return 0;
            case WVR_BatteryStatus_Normal:
                return 100;
            case WVR_BatteryStatus_Low:
                return 50;
            case WVR_BatteryStatus_UltraLow:
                return 10;
        }
    }

    inline glm::vec3 GetPositionFromMatrix4x4(const Matrix4 &matrix4) {
        glm::vec3 position;
        position.x = matrix4[12];
        position.y = matrix4[13];
        position.z = matrix4[14];
        return position;
    }

    inline glm::quat GetRotationFromMatrix4x4(const Matrix4 &matrix4) {
        glm::quat rotation;
        float qw = sqrt(1.0f + matrix4[0] + matrix4[5] + matrix4[10]) / 2.0f;
        float w = 4 * qw;
        float qx = (matrix4[6] - matrix4[9]) / w;
        float qy = (matrix4[8] - matrix4[2]) / w;
        float qz = (matrix4[1] - matrix4[4]) / w;

        rotation.w = qw;
        rotation.x = qx;
        rotation.y = qy;
        rotation.z = qz;
        return rotation;
    }
}

#endif //CLOUDLARKXR_WVR_UTILS_H
