//
// Created by fcx@pingxingyun.com on 2019/11/12.
//

#ifndef MY_APPLICATION_OVR_UTILS_H
#define MY_APPLICATION_OVR_UTILS_H

/**
* Conversion between GLM and Oculus math types
*/
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VrApi_Types.h>
#include <VrApi_Input.h>
#include <VrApi_Ext.h>
//#include "cloudlarkvr/types.h"
#include "log.h"
#include "lark_xr/types.h"
#include "utils.h"

namespace ovr {

    inline glm::mat4 toGlm(const ovrMatrix4f & om) {
        return glm::transpose(glm::make_mat4(&om.M[0][0]));
    }


    inline glm::vec3 toGlm(const ovrVector3f & ov) {
        return glm::make_vec3(&ov.x);
    }

    inline glm::vec2 toGlm(const ovrVector2f & ov) {
        return glm::make_vec2(&ov.x);
    }

    inline glm::quat toGlm(const ovrQuatf & oq) {
        return glm::make_quat(&oq.x);
    }

    inline glm::mat4 toGlm(const ovrPosef & op) {
        glm::mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
        glm::mat4 translation = glm::translate(glm::mat4(), ovr::toGlm(op.Position));
        return translation * orientation;
    }

    inline ovrMatrix4f fromGlm(const glm::mat4 & m) {
        ovrMatrix4f result = {};
        glm::mat4 transposed(glm::transpose(m));
        memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
        return result;
    }

    inline ovrVector3f fromGlm(const glm::vec3 & v) {
        ovrVector3f result;
        result.x = v.x;
        result.y = v.y;
        result.z = v.z;
        return result;
    }

    inline ovrVector2f fromGlm(const glm::vec2 & v) {
        ovrVector2f result;
        result.x = v.x;
        result.y = v.y;
        return result;
    }

    inline ovrQuatf fromGlm(const glm::quat & q) {
        ovrQuatf result;
        result.x = q.x;
        result.y = q.y;
        result.z = q.z;
        result.w = q.w;
        return result;
    }
    //
    inline larkxrTrackedPose toLarkTrackedPose(const ovrRigidBodyPosef& rigidBodyPosef) {
        larkxrTrackedPose pose;
        pose.isConnected = true;
        pose.is6Dof = true;
        pose.isValidPose = true;
        pose.timestamp = (int64_t)(rigidBodyPosef.TimeInSeconds * 1000 * 1000 * 1000);
        pose.predictedMilliSec = (float)(rigidBodyPosef.TimeInSeconds * 1000);
        pose.poseOriginModel = larkxrPoseOriginModel::Larkxr_PoseOriginModel_OriginOnHead;
        pose.position = toGlm(rigidBodyPosef.Pose.Position);
        pose.rotation = toGlm(rigidBodyPosef.Pose.Orientation);
        pose.velocity = toGlm(rigidBodyPosef.LinearVelocity);
        pose.angularVelocity = toGlm(rigidBodyPosef.AngularVelocity);
        pose.acceleration = toGlm(rigidBodyPosef.LinearAcceleration);
        pose.angularAcceleration = toGlm(rigidBodyPosef.AngularAcceleration);
        return pose;
    }
    //    GLFWwindow * createRiftRenderingWindow(ovrHmd hmd, glm::uvec2 & outSize, glm::ivec2 & outPosition);
    //  ovr pose to lark vr hmd pose.
    inline larkxrTrackedPose toLarkHMDTrakedPose(const ovrTracking2& tracking) {
        larkxrTrackedPose pose = toLarkTrackedPose(tracking.HeadPose);
        pose.device = Larkxr_Device_Type_HMD;
        for (int i = 0; i < LARKVR_EYE_COUNT; i ++ ) {
            pose.eye[i].viewMatrix = toGlm(tracking.Eye[i].ViewMatrix);
            pose.eye[i].projectionMatrix = toGlm(tracking.Eye[i].ProjectionMatrix);
        }
        return pose;
    }
    // to larrk controller pose
    inline larkxrTrackedPose toLarkControllerTrackedPose(bool isLeft, const ovrTracking& tracking) {
        larkxrTrackedPose pose = toLarkTrackedPose(tracking.HeadPose);
        pose.device = isLeft ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;
        pose.status = tracking.Status;
        return pose;
    }

    inline ovrRigidBodyPosef fromLarkvrTrackedPose(const larkxrTrackedPose& larkvrPose) {
        ovrRigidBodyPosef pose = {};
        pose.Pose.Position = fromGlm(larkvrPose.position);
        pose.Pose.Orientation = fromGlm(larkvrPose.rotation);
        pose.AngularVelocity = fromGlm(larkvrPose.angularVelocity);
        pose.LinearVelocity = fromGlm(larkvrPose.velocity);
        pose.AngularAcceleration = fromGlm(larkvrPose.angularAcceleration);
        pose.LinearAcceleration = fromGlm(larkvrPose.acceleration);
        pose.TimeInSeconds = (double)larkvrPose.timestamp / (1000.0F * 1000.0F * 1000.0F);
        pose.PredictionInSeconds = (double)larkvrPose.predictedMilliSec / 1000.0F;
        return pose;
    }

    inline ovrTracking2 fromtLarkvrTrackedHMDPose(const larkxrTrackedPose& larkvrPose) {
        ovrTracking2 tracking;
        tracking.HeadPose = fromLarkvrTrackedPose(larkvrPose);
        tracking.Status = (uint)larkvrPose.status;
        for (int i = 0; i < LARKVR_EYE_COUNT; i++) {
            tracking.Eye[i].ViewMatrix = fromGlm(larkvrPose.eye[i].viewMatrix);
            tracking.Eye[i].ProjectionMatrix = fromGlm(larkvrPose.eye[i].projectionMatrix);
        }
        return tracking;
    }

    inline larkxrControllerInputState toLarkvrInputState(bool isLeft, const ovrInputStateTrackedRemote& trackedRemote) {
        larkxrControllerInputState state = {};
        state.isConnected = true;
        state.deviceType = isLeft ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;

        if (ovrButton::ovrButton_A & trackedRemote.Buttons) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click);
        }
        if (ovrButton::ovrButton_B & trackedRemote.Buttons) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click);
        }
        if (ovrButton::ovrButton_X & trackedRemote.Buttons) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click);
        }
        if (ovrButton::ovrButton_Y & trackedRemote.Buttons) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click);
        }

        if (ovrTouch::ovrTouch_A & trackedRemote.Touches) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Touch);
        }
        if (ovrTouch::ovrTouch_B & trackedRemote.Touches) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Touch);
        }
        if (ovrTouch::ovrTouch_X & trackedRemote.Touches) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Touch);
        }
        if (ovrTouch::ovrTouch_Y & trackedRemote.Touches) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Touch);
        }

        if (ovrButton::ovrButton_Trigger & trackedRemote.Buttons) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);
        }
        if (ovrButton::ovrButton_GripTrigger& trackedRemote.Buttons) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Grip_Click);
        }

        state.triggerValue = trackedRemote.IndexTrigger;
        state.gripValue = trackedRemote.GripTrigger;

        state.touchPadAxis.x = trackedRemote.Joystick.x;
        state.touchPadAxis.y = trackedRemote.Joystick.y;

        if (ovrButton::ovrButton_Joystick & trackedRemote.Buttons) {
//            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trackpad_Click);
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click);
        }
        if (ovrTouch::ovrTouch_Joystick & trackedRemote.Touches) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Touch);
        }

        state.batteryPercentRemaining = trackedRemote.BatteryPercentRemaining;
        return state;
    }

    inline void TriggerHaptic(ovrMobile* ovr, bool isLeft, uint64_t startTime, float amplitude, float duration,
                              float frequency) {
        if (duration <= 0)
            return;

        uint32_t deviceIndex = 0;
        ovrInputCapabilityHeader capsHeader;
        while (vrapi_EnumerateInputDevices(ovr, deviceIndex, &capsHeader) >= 0)
        {
            ++deviceIndex;

            if (capsHeader.Type == ovrControllerType_TrackedRemote)
            {
                ovrInputTrackedRemoteCapabilities remoteCaps;
                remoteCaps.Header = capsHeader;
                vrapi_GetInputDeviceCapabilities(ovr, &remoteCaps.Header);

                if (remoteCaps.ControllerCapabilities&ovrControllerCaps_LeftHand && !isLeft)
                {
                    continue;
                }

                if (remoteCaps.ControllerCapabilities&ovrControllerCaps_RightHand && isLeft)
                {
                    continue;
                }

                if (0 == (remoteCaps.ControllerCapabilities&
                          ovrControllerCaps_HasBufferedHapticVibration))
                {
                    continue;
                }

                ovrHapticBuffer hapticBuffer;
                hapticBuffer.BufferTime = utils::GetTimeInSeconds() + 0.03;
                hapticBuffer.NumSamples = remoteCaps.HapticSamplesMax;
                hapticBuffer.HapticBuffer =
                        reinterpret_cast<uint8_t*>(alloca(remoteCaps.HapticSamplesMax));
                hapticBuffer.Terminated = true;

                for (uint32_t i = 0; i < hapticBuffer.NumSamples; i++)
                {
                    hapticBuffer.HapticBuffer[i] =
                            static_cast<uint8_t>(amplitude*255.f);
                }

                vrapi_SetHapticVibrationBuffer(ovr, capsHeader.DeviceID, &hapticBuffer);
            }
        }
    }
}
#endif //MY_APPLICATION_OVR_UTILS_H
