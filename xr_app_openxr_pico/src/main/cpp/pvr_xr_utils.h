//
// Created by Administrator on 2021/7/6.
//

#ifndef CLOUDLARKXR_PVR_XR_UTILS_H
#define CLOUDLARKXR_PVR_XR_UTILS_H

#include "pch.h"
#include "common.h"
#include "xr_input_state.h"
#include <common/xr_linear.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <lark_xr/types.h>
#include <lark_xr/xr_client.h>
#include <logger.h>

#ifdef CHECK_GL_ERRORS

static const char* GlErrorString(GLenum error) {
    switch (error) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        default:
            return "unknown";
    }
}

static void GLCheckErrors(int line) {
    for (int i = 0; i < 10; i++) {
        const GLenum error = glGetError();
        if (error == GL_NO_ERROR) {
            break;
        }
        ALOGE("GL error on line %d: %s", line, GlErrorString(error));
    }
}

#define GL(func) \
    func;        \
    GLCheckErrors(__LINE__);

#else // CHECK_GL_ERRORS

#define GL(func) func;

#endif // CHECK_GL_ERRORS

#define OXR(func) func;


namespace Math {
    namespace Pose {
        inline XrPosef Identity() {
            XrPosef t{};
            t.orientation.w = 1;
            return t;
        }

        inline XrPosef Translation(const XrVector3f& translation) {
            XrPosef t = Identity();
            t.position = translation;
            return t;
        }

        inline XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation) {
            XrPosef t = Identity();
            t.orientation.x = 0.f;
            t.orientation.y = std::sin(radians * 0.5f);
            t.orientation.z = 0.f;
            t.orientation.w = std::cos(radians * 0.5f);
            t.position = translation;
            return t;
        }
    }  // namespace Pose
}  // namespace Math

namespace pxrutils {
    inline const char* GlFrameBufferStatusString(GLenum status) {
        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                return "GL_FRAMEBUFFER_UNDEFINED";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            case GL_FRAMEBUFFER_UNSUPPORTED:
                return "GL_FRAMEBUFFER_UNSUPPORTED";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            default:
                return "unknown";
        }
    }

    inline std::string GetXrVersionString(XrVersion ver) {
        return Fmt("%d.%d.%d", XR_VERSION_MAJOR(ver), XR_VERSION_MINOR(ver), XR_VERSION_PATCH(ver));
    }

    inline XrFormFactor GetXrFormFactor(const std::string& formFactorStr) {
        if (EqualsIgnoreCase(formFactorStr, "Hmd")) {
            return XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        }
        if (EqualsIgnoreCase(formFactorStr, "Handheld")) {
            return XR_FORM_FACTOR_HANDHELD_DISPLAY;
        }
        throw std::invalid_argument(Fmt("Unknown form factor '%s'", formFactorStr.c_str()));
    }

    inline XrViewConfigurationType GetXrViewConfigurationType(const std::string& viewConfigurationStr) {
        if (EqualsIgnoreCase(viewConfigurationStr, "Mono")) {
            return XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO;
        }
        if (EqualsIgnoreCase(viewConfigurationStr, "Stereo")) {
            return XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        }
        throw std::invalid_argument(Fmt("Unknown view configuration '%s'", viewConfigurationStr.c_str()));
    }

    inline XrEnvironmentBlendMode GetXrEnvironmentBlendMode(const std::string& environmentBlendModeStr) {
        if (EqualsIgnoreCase(environmentBlendModeStr, "Opaque")) {
            return XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
        }
        if (EqualsIgnoreCase(environmentBlendModeStr, "Additive")) {
            return XR_ENVIRONMENT_BLEND_MODE_ADDITIVE;
        }
        if (EqualsIgnoreCase(environmentBlendModeStr, "AlphaBlend")) {
            return XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND;
        }
        throw std::invalid_argument(Fmt("Unknown environment blend mode '%s'", environmentBlendModeStr.c_str()));
    }
    inline XrReferenceSpaceCreateInfo GetXrReferenceSpaceCreateInfo(const std::string& referenceSpaceTypeStr) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Identity();
        if (EqualsIgnoreCase(referenceSpaceTypeStr, "View")) {
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "ViewFront")) {
            // Render head-locked 2m in front of device.
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Translation({0.f, 0.f, -2.f}),
                    referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Local")) {
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Stage")) {
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeft")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, {-2.f, 0.f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRight")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, {2.f, 0.f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeftRotated")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(3.14f / 3.f, {-2.f, 0.5f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRightRotated")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(-3.14f / 3.f, {2.f, 0.5f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else {
            throw std::invalid_argument(Fmt("Unknown reference space type '%s'", referenceSpaceTypeStr.c_str()));
        }
        return referenceSpaceCreateInfo;
    }
}

namespace pvr {
    inline glm::mat4 toGlm(const XrMatrix4x4f& m) {
        return glm::mat4(
                m.m[0], m.m[1], m.m[2], m.m[3],
                m.m[4], m.m[5], m.m[6], m.m[7],
                m.m[8], m.m[9], m.m[10], m.m[11],
                m.m[12], m.m[13], m.m[14], m.m[15]
        );
    }
    inline glm::vec3 toGlm(const XrVector3f& v) {
        return glm::vec3(v.x, v.y, v.z);
    }
    inline glm::quat toGlm(const XrQuaternionf& q) {
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    inline XrVector3f fromGlm(const glm::vec3& v) {
        return  { v.x, v.y, v.z };
    }
    inline XrQuaternionf fromGlm(const glm::quat& q) {
        return { q.x, q.y, q.z, q.w };
    }
    inline larkxrControllerInputState toLarkvrInputState(int hand, const XrSession& session, const InputState& input_state) {
        larkxrControllerInputState state = {};
        state.isConnected = true;
        state.deviceType = hand == Side::LEFT ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;

        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.action = input_state.grabAction;
        getInfo.subactionPath = input_state.handSubactionPath[hand];

        // button-> y
        getInfo.action = input_state.YAction;
        XrActionStateBoolean YValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &YValue));

        // button->b
        getInfo.action = input_state.BAction;
        XrActionStateBoolean BValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &BValue));

        // button->x
        getInfo.action = input_state.XAction;
        XrActionStateBoolean XValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &XValue));

        // button->a
        getInfo.action = input_state.AAction;
        XrActionStateBoolean AValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &AValue));
//        LOGI("XValue %d AValue %d", XValue.currentState, AValue.currentState);

        getInfo.action = input_state.triggerAction;
        XrActionStateFloat triggerValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &triggerValue));

        getInfo.action = input_state.batteryAction;
        XrActionStateFloat batteryValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &batteryValue));

        getInfo.action = input_state.backAction;
        XrActionStateBoolean backValue{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &backValue));

        getInfo.action = hand == Side::LEFT ? input_state.TriggerTouchLeftAction : input_state.TriggerTouchRightAction;
        XrActionStateBoolean TriggerTouch{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &TriggerTouch));

        getInfo.action = hand == Side::LEFT ? input_state.TriggerClickLeftAction : input_state.TriggerClickRightAction;
        XrActionStateBoolean TriggerClick{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &TriggerClick));

        getInfo.action = hand == Side::LEFT ? input_state.GripClickLeftAction : input_state.GripClickRightAction;
        XrActionStateBoolean gripClick{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &gripClick));

        getInfo.action = hand == Side::LEFT ? input_state.GripValueLeftAction : input_state.GripValueRightAction;
        XrActionStateFloat gripValue{XR_TYPE_ACTION_STATE_FLOAT};
        CHECK_XRCMD(xrGetActionStateFloat(session, &getInfo, &gripValue));

        getInfo.action = hand == Side::LEFT ? input_state.JoystickClickLeftAction : input_state.JoystickClickRightAction;
        XrActionStateBoolean joystickClick{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &joystickClick));

        getInfo.action = hand == Side::LEFT ? input_state.JoystickTouchLeftAction : input_state.JoystickTouchRightAction;
        XrActionStateBoolean joystickTouch{XR_TYPE_ACTION_STATE_BOOLEAN};
        CHECK_XRCMD(xrGetActionStateBoolean(session, &getInfo, &joystickTouch));

        getInfo.action = hand == Side::LEFT ? input_state.JoystickValueLeftAction : input_state.JoystickValueRightAction;
        XrActionStateVector2f joystickValue{XR_TYPE_ACTION_STATE_VECTOR2F};
        CHECK_XRCMD(xrGetActionStateVector2f(session, &getInfo, &joystickValue));

        if (AValue.currentState && AValue.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click);
        }

        if (BValue.currentState && BValue.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click);
        }

        if (XValue.currentState && XValue.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click);
        }

        if (YValue.currentState && YValue.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click);
        }

        if (backValue.currentState && backValue.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_System_Click);
        }

        if (TriggerTouch.currentState && TriggerTouch.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Touch);
        }

        if (TriggerClick.currentState && TriggerClick.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);
        }

        if (gripClick.currentState && gripClick.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Grip_Click);
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Grip_Touch);
        }

        // TODO
        // gripValue from pico not good
        state.gripValue = gripValue.currentState;

        state.triggerValue = triggerValue.currentState;

        state.touchPadAxis.x = joystickValue.currentState.x;
        state.touchPadAxis.y = joystickValue.currentState.y;

//        LOGV("touchPadAxis %f %f triggerValue %f gripValue %f", state.touchPadAxis.x, state.touchPadAxis.y, state.triggerValue, state.gripValue);
//        LOGV("controller input hand [%d] gripValue %f gripClick %d triggerValue %f triggerClick %d TriggerTouch %d",
//             hand, gripValue.currentState, gripClick.currentState, triggerValue.currentState, TriggerClick.currentState, TriggerTouch.currentState);

//        LOGV("controller input hand [%d] joystickClick %d joystickTouch %d joystick:X %f :Y %f",
//             hand, joystickClick.currentState, joystickTouch.currentState, joystickValue.currentState.x, joystickValue.currentState.y);

        if (joystickClick.currentState && joystickClick.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click);
        }

        if (joystickTouch.currentState && joystickTouch.isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Touch);
        }

        state.batteryPercentRemaining = (batteryValue.currentState * 100 / 5);
//        state.batteryPercentRemaining = 100;

        // update battery info to lark system.
        lark::XRClient::SetControlerBatteryLevel(hand == Side::LEFT, state.batteryPercentRemaining);
//        lark::XRClient::SetControlerBatteryLevel(hand == Side::LEFT, 100);
        return state;
    }
} // namespace pvr



#endif //CLOUDLARKXR_PVR_XR_UTILS_H
