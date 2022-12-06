//
// Created by fcx@pingxingyung.com on 2022/11/10.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_INPUT_STATE_H
#define LARKXR_INPUT_STATE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <cstring>
#include "inttypes.h"

namespace oxr {
struct InputState {
    static XrActionSet CreateActionSet(const XrInstance& instance,
                                       int priority,
                                       const char* name,
                                       const char* localizedName);
    static XrAction CreateAction(
            XrActionSet actionSet,
            XrActionType type,
            const char* actionName,
            const char* localizedName,
            int countSubactionPaths,
            XrPath* subactionPaths);

    static XrActionSuggestedBinding ActionSuggestedBinding(const XrInstance& instance,
                                                           XrAction action,
                                                           const char* bindingString);

    static XrSpace CreateActionSpace(const XrSession& session, XrAction poseAction, XrPath subactionPath);

    static XrActionStateBoolean GetActionStateBoolean(const XrSession& session, XrAction action);

    static XrActionStateFloat GetActionStateFloat(const XrSession& session, XrAction action);

    static XrActionStateVector2f GetActionStateVector2(const XrSession& session, XrAction action);

    static bool ActionPoseIsActive(const XrSession& session, XrAction action, XrPath subactionPath);

    static larkxrControllerInputState toLarkXRInputState(int hand, const InputState& inputState);

    XrActionSet runningActionSet{XR_NULL_HANDLE};
    XrAction AAction{XR_NULL_HANDLE};
    XrAction BAction{XR_NULL_HANDLE};
    XrAction XAction{XR_NULL_HANDLE};
    XrAction YAction{XR_NULL_HANDLE};
    XrAction ATouchAction{XR_NULL_HANDLE};
    XrAction BTouchAction{XR_NULL_HANDLE};
    XrAction XTouchAction{XR_NULL_HANDLE};
    XrAction YTouchAction{XR_NULL_HANDLE};
    XrAction TriggerLeftClickAction{XR_NULL_HANDLE};
    XrAction TriggerRightClickAction{XR_NULL_HANDLE};
    // XrAction TriggerTouchAction{XR_NULL_HANDLE};
    XrAction TriggerLeftValueAction{XR_NULL_HANDLE};
    XrAction TriggerRightValueAction{XR_NULL_HANDLE};
    XrAction ThumbClickLeftAction{XR_NULL_HANDLE};
    XrAction ThumbClickRightAction{XR_NULL_HANDLE};
    // XrAction ThumbTouchAction{XR_NULL_HANDLE};
    XrAction ThumbStickLeftAction{XR_NULL_HANDLE};
    XrAction ThumbStickRightAction{XR_NULL_HANDLE};

    XrAction SqueezeLeftAction{XR_NULL_HANDLE};
    XrAction SqueezeRightAction{XR_NULL_HANDLE};

    XrAction vibrateLeftToggle{XR_NULL_HANDLE};
    XrAction vibrateRightToggle{XR_NULL_HANDLE};
    XrAction vibrateLeftFeedback{XR_NULL_HANDLE};
    XrAction vibrateRightFeedback{XR_NULL_HANDLE};

    XrAction aimPoseAction{XR_NULL_HANDLE};
    XrAction gripPoseAction{XR_NULL_HANDLE};

//    XrAction BatteryLeftAction{XR_NULL_HANDLE};
//    XrAction BatteryRightAction{XR_NULL_HANDLE};

    XrPath leftHandPath;
    XrPath rightHandPath;
    XrPath handSubactionPaths[2]; // left right

    XrSpace leftControllerAimSpace{XR_NULL_HANDLE};
    XrSpace rightControllerAimSpace{XR_NULL_HANDLE};
    XrSpace leftControllerGripSpace{XR_NULL_HANDLE};
    XrSpace rightControllerGripSpace{XR_NULL_HANDLE};

    // controller states
    // update by Update functions
    // left / right
    bool ControllerActive[2] = { false, false };
    XrSpaceLocation ControllerPose[2] = { {XR_TYPE_SPACE_LOCATION}, {XR_TYPE_SPACE_LOCATION} };
    XrActionStateFloat TriggerValue[2] = { {XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT} };
    XrActionStateBoolean TriggerClick[2] = { {XR_TYPE_ACTION_STATE_BOOLEAN}, {XR_TYPE_ACTION_STATE_BOOLEAN} };
    XrActionStateVector2f ThumbValue[2] = { {XR_TYPE_ACTION_STATE_VECTOR2F}, {XR_TYPE_ACTION_STATE_VECTOR2F} };
    XrActionStateBoolean ThumbClick[2] = { {XR_TYPE_ACTION_STATE_BOOLEAN}, {XR_TYPE_ACTION_STATE_BOOLEAN} };
    XrActionStateFloat Squeeze[2] = { {XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT} };

    XrActionStateBoolean YClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean YTouch{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean BClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean BTouch{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean XClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean XTouch{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean AClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean ATouch{XR_TYPE_ACTION_STATE_BOOLEAN};

    larkxrControllerDeviceState ControllerState[2] = {};

    void InitializeActions(const XrInstance& instance, const XrSession& session);

    // call aftre session active;
    void InitializeControllerSpace(const XrSession& session);

    // Update input state every frame
    // sync action data
    // InitializeControllerSpace
    void UpDate(const XrSession& session, XrSpace const &space, const XrTime& time);
};
}

#endif //LARKXR_INPUT_STATE_H
