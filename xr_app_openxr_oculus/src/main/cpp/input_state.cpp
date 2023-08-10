//
// Created by fcx@pingxingyung.com on 2022/11/10.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <cassert>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <lark_xr/xr_client.h>
#include "oxr_utils.h"
#include "input_state.h"
#include "openxr/fb_touch_controller_pro.h"

namespace oxr {
void InputState::InitializeActions(const XrInstance &instance, const XrSession &session) {
    OXR(xrStringToPath(instance, "/user/hand/left", &leftHandPath));
    OXR(xrStringToPath(instance, "/user/hand/right", &rightHandPath));
    handSubactionPaths[0] = leftHandPath;
    handSubactionPaths[1] = rightHandPath;

    // Actions
    runningActionSet =
            CreateActionSet(instance, 1, "running_action_set", "Action Set used on main loop");

    AAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "a", "A button", 0, NULL);
    BAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "b", "B button", 0, NULL);
    XAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "x", "X button", 0, NULL);
    YAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "y", "Y button", 0, NULL);

    ATouchAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "a_touch", "A touch", 0, NULL);
    BTouchAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "b_touch", "B touch", 0, NULL);
    XTouchAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "x_touch", "X touch", 0, NULL);
    YTouchAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "y_touch", "Y touch", 0, NULL);

    TriggerLeftClickAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "trigger_left", "trigger left click", 0, NULL);
    TriggerRightClickAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "trigger_right", "trigger right click", 0, NULL);
    //TriggerTouchAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "trigger_touch", "trigger touch", 2, handSubactionPaths);
    TriggerLeftValueAction = CreateAction(runningActionSet, XR_ACTION_TYPE_FLOAT_INPUT, "trigger_left_value", "trigger left value", 0, NULL);
    TriggerRightValueAction = CreateAction(runningActionSet, XR_ACTION_TYPE_FLOAT_INPUT, "trigger_right_value", "trigger right value", 0, NULL);

    ThumbClickLeftAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "thumb_left_click", "thumb_left_click", 0, NULL);
    ThumbClickRightAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "thumb_right_click", "thumb_right_click", 0, NULL);
    //ThumbTouchAction = CreateAction(runningActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "thumb_touch", "thumb_touch", 2, handSubactionPaths);
    ThumbStickLeftAction = CreateAction(runningActionSet, XR_ACTION_TYPE_VECTOR2F_INPUT, "thumb_left_stick", "thumb_left_stick", 0, NULL);
    ThumbStickRightAction = CreateAction(runningActionSet, XR_ACTION_TYPE_VECTOR2F_INPUT, "thumb_right_stick", "thumb_right_stick", 0, NULL);

    SqueezeLeftAction = CreateAction(
            runningActionSet, XR_ACTION_TYPE_FLOAT_INPUT, "squeeze_left", "squeeze_left", 0, NULL);
    SqueezeRightAction = CreateAction(
            runningActionSet, XR_ACTION_TYPE_FLOAT_INPUT, "squeeze_right", "squeeze_right", 0, NULL);

//    BatteryLeftAction = CreateAction(
//            runningActionSet, XR_ACTION_TYPE_FLOAT_INPUT, "battery_controller_left", "battery_controller_left", 0, NULL);
//    BatteryRightAction = CreateAction(
//            runningActionSet, XR_ACTION_TYPE_FLOAT_INPUT, "battery_controller_right", "battery_controller_right", 0, NULL);

    vibrateLeftToggle = CreateAction(
            runningActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "vibrate_left",
            "Vibrate Left Controller",
            0,
            NULL);

    vibrateRightToggle = CreateAction(
            runningActionSet,
            XR_ACTION_TYPE_BOOLEAN_INPUT,
            "vibrate_right",
            "Vibrate Right Controller",
            0,
            NULL);

    vibrateLeftFeedback = CreateAction(
            runningActionSet,
            XR_ACTION_TYPE_VIBRATION_OUTPUT,
            "vibrate_left_feedback",
            "Vibrate Left Controller Feedback",
            0,
            NULL);

    vibrateRightFeedback = CreateAction(
            runningActionSet,
            XR_ACTION_TYPE_VIBRATION_OUTPUT,
            "vibrate_right_feedback",
            "Vibrate Right Controller Feedback",
            0,
            NULL);

    aimPoseAction = CreateAction(
            runningActionSet, XR_ACTION_TYPE_POSE_INPUT, "aim_pose", NULL, 2, handSubactionPaths);
    gripPoseAction = CreateAction(
            runningActionSet, XR_ACTION_TYPE_POSE_INPUT, "grip_pose", NULL, 2, handSubactionPaths);

    XrPath interactionProfilePath = XR_NULL_PATH;
    XrPath interactionProfilePathTouch = XR_NULL_PATH;
    XrPath interactionProfilePathKHRSimple = XR_NULL_PATH;

    OXR(xrStringToPath(
            instance,
            "/interaction_profiles/oculus/touch_controller",
            &interactionProfilePathTouch));
    OXR(xrStringToPath(
            instance,
            "/interaction_profiles/khr/simple_controller",
            &interactionProfilePathKHRSimple));

    // Toggle this to force simple as a first choice, otherwise use it as a last resort
    bool useSimpleProfile = false; /// true;
    if (useSimpleProfile) {
        ALOGV("xrSuggestInteractionProfileBindings found bindings for Khronos SIMPLE controller");
        interactionProfilePath = interactionProfilePathKHRSimple;
    } else {
        // Query Set
        XrActionSet queryActionSet =
                CreateActionSet(instance, 1, "query_action_set", "Action Set used to query device caps");
        XrAction dummyAction = CreateAction(
                queryActionSet, XR_ACTION_TYPE_BOOLEAN_INPUT, "dummy_action", "Dummy Action", 0, NULL);

        // Map bindings
        XrActionSuggestedBinding bindings[1];
        int currBinding = 0;
        bindings[currBinding++] =
                ActionSuggestedBinding(instance, dummyAction, "/user/hand/right/input/system/click");

        XrInteractionProfileSuggestedBinding suggestedBindings = {};
        suggestedBindings.type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING;
        suggestedBindings.next = NULL;
        suggestedBindings.suggestedBindings = bindings;
        suggestedBindings.countSuggestedBindings = currBinding;

        // Try all
        suggestedBindings.interactionProfile = interactionProfilePathTouch;
        XrResult suggestTouchResult =
                xrSuggestInteractionProfileBindings(instance, &suggestedBindings);
        OXR(suggestTouchResult);

        if (XR_SUCCESS == suggestTouchResult) {
            ALOGV("xrSuggestInteractionProfileBindings found bindings for QUEST controller");
            interactionProfilePath = interactionProfilePathTouch;
        } else {
            ALOGE("interactionProfilePathTouch empty. not support input path");
        }
    }

    // Action creation
    {
        // Map bindings

        XrActionSuggestedBinding bindings[50]; // large enough for all profiles
        int currBinding = 0;

        {
            if (interactionProfilePath == interactionProfilePathTouch) {

                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, YAction, "/user/hand/left/input/y/click");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, BAction, "/user/hand/right/input/b/click");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, XAction, "/user/hand/left/input/x/click");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, AAction, "/user/hand/right/input/a/click");

                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, YTouchAction, "/user/hand/left/input/y/touch");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, BTouchAction, "/user/hand/right/input/b/touch");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, XTouchAction, "/user/hand/left/input/x/touch");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, ATouchAction, "/user/hand/right/input/a/touch");

                // bindings[currBinding++] = ActionSuggestedBinding(instance, TriggerTouchAction, "/user/hand/left/input/touch");
                // bindings[currBinding++] = ActionSuggestedBinding(instance, TriggerTouchAction, "/user/hand/right/input/touch");

                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, TriggerLeftClickAction, "/user/hand/left/input/trigger");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, TriggerRightClickAction, "/user/hand/right/input/trigger");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, TriggerLeftValueAction, "/user/hand/left/input/trigger/value");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, TriggerRightValueAction, "/user/hand/right/input/trigger/value");

                bindings[currBinding++] = ActionSuggestedBinding(
                        instance, ThumbClickLeftAction, "/user/hand/left/input/thumbstick/click");
                bindings[currBinding++] = ActionSuggestedBinding(
                        instance, ThumbClickRightAction, "/user/hand/right/input/thumbstick/click");
                //bindings[currBinding++] = ActionSuggestedBinding(instance, ThumbTouchAction, "/user/hand/right/input/thumbstick/touch");
                //[currBinding++] = ActionSuggestedBinding(instance, ThumbTouchAction, "/user/hand/left/input/thumbstick/touch");
                bindings[currBinding++] = ActionSuggestedBinding(
                        instance, ThumbStickLeftAction, "/user/hand/left/input/thumbstick");
                bindings[currBinding++] = ActionSuggestedBinding(
                        instance, ThumbStickRightAction, "/user/hand/right/input/thumbstick");

                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, SqueezeLeftAction, "/user/hand/left/input/squeeze/value");
                bindings[currBinding++] =
                        ActionSuggestedBinding(instance, SqueezeRightAction, "/user/hand/right/input/squeeze/value");

                bindings[currBinding++] =
                        ActionSuggestedBinding(
                                instance, vibrateLeftFeedback, "/user/hand/left/output/haptic");
                bindings[currBinding++] =
                        ActionSuggestedBinding(
                                instance, vibrateRightFeedback, "/user/hand/right/output/haptic");
                bindings[currBinding++] =
                        ActionSuggestedBinding(
                                instance, aimPoseAction, "/user/hand/left/input/aim/pose");
                bindings[currBinding++] =
                        ActionSuggestedBinding(
                                instance, aimPoseAction, "/user/hand/right/input/aim/pose");
                bindings[currBinding++] =
                        ActionSuggestedBinding(
                                instance, gripPoseAction, "/user/hand/left/input/grip/pose");
                bindings[currBinding++] =
                        ActionSuggestedBinding(
                                instance, gripPoseAction, "/user/hand/right/input/grip/pose");

//                bindings[currBinding++] =
//                        ActionSuggestedBinding(
//                                instance, BatteryLeftAction, "/user/hand/left/input/battery/value");
//                bindings[currBinding++] =
//                        ActionSuggestedBinding(
//                                instance, BatteryRightAction, "/user/hand/right/input/battery/value");
            }
        }

        XrInteractionProfileSuggestedBinding suggestedBindings = {};
        suggestedBindings.type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING;
        suggestedBindings.next = NULL;
        suggestedBindings.interactionProfile = interactionProfilePath;
        suggestedBindings.suggestedBindings = bindings;
        suggestedBindings.countSuggestedBindings = currBinding;
        OXR(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));

        // Attach to session
        XrSessionActionSetsAttachInfo attachInfo = {};
        attachInfo.type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO;
        attachInfo.next = NULL;
        attachInfo.countActionSets = 1;
        attachInfo.actionSets = &runningActionSet;
        OXR(xrAttachSessionActionSets(session, &attachInfo));

        // Enumerate actions
        XrPath actionPathsBuffer[16];
        char stringBuffer[256];
        XrAction actionsToEnumerate[] = {
                AAction,
                BAction,
                XAction,
                YAction,

                ATouchAction,
                BTouchAction,
                XTouchAction,
                YTouchAction,

                TriggerLeftClickAction,
                TriggerRightClickAction,
                //TriggerTouchAction,
                TriggerLeftClickAction,
                TriggerRightClickAction,

                ThumbClickLeftAction,
                ThumbClickRightAction,
                //ThumbTouchAction,
                ThumbStickLeftAction,
                ThumbStickRightAction,

                SqueezeLeftAction,
                SqueezeRightAction,
                vibrateLeftToggle,
                vibrateRightToggle,
                vibrateLeftFeedback,
                vibrateRightFeedback,
                aimPoseAction,
                gripPoseAction,

//                BatteryLeftAction,
//                BatteryRightAction,
        };
        for (size_t i = 0; i < sizeof(actionsToEnumerate) / sizeof(actionsToEnumerate[0]); ++i) {
            XrBoundSourcesForActionEnumerateInfo enumerateInfo = {};
            enumerateInfo.type = XR_TYPE_BOUND_SOURCES_FOR_ACTION_ENUMERATE_INFO;
            enumerateInfo.next = NULL;
            enumerateInfo.action = actionsToEnumerate[i];

            // Get Count
            uint32_t countOutput = 0;
            OXR(xrEnumerateBoundSourcesForAction(
                    session, &enumerateInfo, 0 /* request size */, &countOutput, NULL));
            ALOGV(
                    "xrEnumerateBoundSourcesForAction action=%lld count=%u",
                    (long long)enumerateInfo.action,
                    countOutput);

            if (countOutput < 16) {
                OXR(xrEnumerateBoundSourcesForAction(
                        session, &enumerateInfo, 16, &countOutput, actionPathsBuffer));
                for (uint32_t a = 0; a < countOutput; ++a) {
                    XrInputSourceLocalizedNameGetInfo nameGetInfo = {};
                    nameGetInfo.type = XR_TYPE_INPUT_SOURCE_LOCALIZED_NAME_GET_INFO;
                    nameGetInfo.next = NULL;
                    nameGetInfo.sourcePath = actionPathsBuffer[a];
                    nameGetInfo.whichComponents = XR_INPUT_SOURCE_LOCALIZED_NAME_USER_PATH_BIT |
                                                  XR_INPUT_SOURCE_LOCALIZED_NAME_INTERACTION_PROFILE_BIT |
                                                  XR_INPUT_SOURCE_LOCALIZED_NAME_COMPONENT_BIT;

                    uint32_t stringCount = 0u;
                    OXR(xrGetInputSourceLocalizedName(
                            session, &nameGetInfo, 0, &stringCount, NULL));
                    if (stringCount < 256) {
                        OXR(xrGetInputSourceLocalizedName(
                                session, &nameGetInfo, 256, &stringCount, stringBuffer));
                        char pathStr[256];
                        uint32_t strLen = 0;
                        OXR(xrPathToString(
                                instance,
                                actionPathsBuffer[a],
                                (uint32_t)sizeof(pathStr),
                                &strLen,
                                pathStr));
                        ALOGV(
                                "  -> path = %lld `%s` -> `%s`",
                                (long long)actionPathsBuffer[a],
                                pathStr,
                                stringBuffer);
                    }
                }
            }
        }
    }

}

void InputState::InitializeControllerSpace(const XrSession& session) {
    if (leftControllerAimSpace == XR_NULL_HANDLE) {
        leftControllerAimSpace = CreateActionSpace(session, aimPoseAction, leftHandPath);
    }
    if (rightControllerAimSpace == XR_NULL_HANDLE) {
        rightControllerAimSpace = CreateActionSpace(session, aimPoseAction, rightHandPath);
    }
    if (leftControllerGripSpace == XR_NULL_HANDLE) {
        leftControllerGripSpace = CreateActionSpace(session, gripPoseAction, leftHandPath);
    }
    if (rightControllerGripSpace == XR_NULL_HANDLE) {
        rightControllerGripSpace = CreateActionSpace(session, gripPoseAction, rightHandPath);
    }
}

void InputState::UpDate(const XrSession& session, XrSpace const &space, const XrTime& time) {
    InitializeControllerSpace(session);

    // sync action data
    XrActiveActionSet activeActionSet = {};
    activeActionSet.actionSet = runningActionSet;
    activeActionSet.subactionPath = XR_NULL_PATH;

    XrActionsSyncInfo syncInfo = {};
    syncInfo.type = XR_TYPE_ACTIONS_SYNC_INFO;
    syncInfo.next = NULL;
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    OXR(xrSyncActions(session, &syncInfo));

    for (int hand = 0; hand < 2; hand++) {
        bool isLeft = hand == 0;

//        bool isActive = InputState::ActionPoseIsActive(session, aimPoseAction, isLeft ? leftHandPath : rightHandPath);
//
//        if (!isActive) {
//            ControllerActive[hand] = false;
//            continue;
//        }

        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        XrSpaceVelocity spaceVelocity{ XR_TYPE_SPACE_VELOCITY};
        spaceLocation.next = &spaceVelocity;
        XrResult res;
        OXR(res = xrLocateSpace(isLeft ? leftControllerAimSpace : rightControllerAimSpace,
                                space, time, &spaceLocation));
        if (!XR_UNQUALIFIED_SUCCESS(res)) {
            LOGI("xrLocateSpace failed hand %d res %d", hand, res);
            ControllerActive[hand] = false;

            ControllerState[hand] = {};
            continue;
        }

        if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
            (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
            ControllerPose[hand] = spaceLocation;
            ControllerActive[hand] = true;
        } else {
//            LOGI("xrLocateSpace failed flags %ld", spaceLocation.locationFlags);
            ControllerActive[hand] = false;

            ControllerState[hand] = {};
            continue;
        }

        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.subactionPath = handSubactionPaths[hand];

        // button-> y
        getInfo.action = YAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &YClick));

        // button->b
        getInfo.action = BAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &BClick));
//        LOGI("BValue %d YValue %d", BValue.currentState, YValue.currentState);

        // button->x
        getInfo.action = XAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &XClick));

        // button->a
        getInfo.action = AAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &AClick));
//        LOGI("XValue %d AValue %d", XValue.currentState, AValue.currentState);
//
        // y touch
        getInfo.action = YTouchAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &YTouch));

        // b touch
        getInfo.action = BTouchAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &BTouch));

        // x touch
        getInfo.action = XTouchAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &XTouch));

        // a touch
        getInfo.action = ATouchAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &ATouch));

        getInfo.action = isLeft ? TriggerLeftClickAction : TriggerRightClickAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &TriggerClick[hand]));

        getInfo.action = isLeft ? TriggerLeftValueAction : TriggerRightValueAction;
        OXR(xrGetActionStateFloat(session, &getInfo, &TriggerValue[hand]));

        getInfo.action = isLeft ? SqueezeLeftAction : SqueezeRightAction;
        OXR(xrGetActionStateFloat(session, &getInfo, &Squeeze[hand]));

        getInfo.action = isLeft ? ThumbClickLeftAction : ThumbClickRightAction;
        OXR(xrGetActionStateBoolean(session, &getInfo, &ThumbClick[hand]));

        getInfo.action = isLeft ? ThumbStickLeftAction : ThumbStickRightAction;
        OXR(xrGetActionStateVector2f(session, &getInfo, &ThumbValue[hand]));

//        XrActionStateFloat battery = {XR_TYPE_ACTION_STATE_FLOAT};
//        getInfo.action = isLeft ? BatteryLeftAction : BatteryRightAction;
//        OXR(xrGetActionStateFloat(session, &getInfo, &battery));

//        LOGV("battery hand[%d] currentState %f isActive %d changedSinceLastSync %d",
//             hand, battery.currentState, battery.isActive, battery.changedSinceLastSync);

        // update larkxr state
        ControllerState[hand].deviceType = hand == 0 ? Larkxr_Controller_Left : Larkxr_Controller_Right;
        ControllerState[hand].pose = {};
        ControllerState[hand].pose.isValidPose = true;
        ControllerState[hand].pose.isConnected = true;
        ControllerState[hand].pose.is6Dof = true;
        ControllerState[hand].pose.position = toGlm(ControllerPose[hand].pose.position);
        ControllerState[hand].pose.rotation = toGlm(ControllerPose[hand].pose.orientation);

        ControllerState[hand].inputState = toLarkXRInputState(hand, *this);
    }
}

// statics...
XrActionSet
InputState::CreateActionSet(XrInstance const &instance, int priority, const char *name,
                            const char *localizedName) {
    XrActionSetCreateInfo asci = {};
    asci.type = XR_TYPE_ACTION_SET_CREATE_INFO;
    asci.next = NULL;
    asci.priority = priority;
    strcpy(asci.actionSetName, name);
    strcpy(asci.localizedActionSetName, localizedName);
    XrActionSet actionSet = XR_NULL_HANDLE;
    OXR(xrCreateActionSet(instance, &asci, &actionSet));
    return actionSet;
}

XrAction
InputState::CreateAction(XrActionSet actionSet, XrActionType type, const char *actionName,
                         const char *localizedName, int countSubactionPaths,
                         XrPath *subactionPaths)  {
    ALOGV("CreateAction %s, %" PRIi32, actionName, countSubactionPaths);

    XrActionCreateInfo aci = {};
    aci.type = XR_TYPE_ACTION_CREATE_INFO;
    aci.next = NULL;
    aci.actionType = type;
    if (countSubactionPaths > 0) {
        aci.countSubactionPaths = countSubactionPaths;
        aci.subactionPaths = subactionPaths;
    }
    strcpy(aci.actionName, actionName);
    strcpy(aci.localizedActionName, localizedName ? localizedName : actionName);
    XrAction action = XR_NULL_HANDLE;
    OXR(xrCreateAction(actionSet, &aci, &action));
    return action;
}

XrActionSuggestedBinding
InputState::ActionSuggestedBinding(XrInstance const &instance, XrAction action,
                                   const char *bindingString)  {
    XrActionSuggestedBinding asb;
    asb.action = action;
    XrPath bindingPath;
    OXR(xrStringToPath(instance, bindingString, &bindingPath));
    asb.binding = bindingPath;
    return asb;
}

XrSpace InputState::CreateActionSpace(XrSession const &session, XrAction poseAction,
                                      XrPath subactionPath)  {
    XrActionSpaceCreateInfo asci = {};
    asci.type = XR_TYPE_ACTION_SPACE_CREATE_INFO;
    asci.action = poseAction;
    asci.poseInActionSpace.orientation.w = 1.0f;
    asci.subactionPath = subactionPath;
    XrSpace actionSpace = XR_NULL_HANDLE;
    OXR(xrCreateActionSpace(session, &asci, &actionSpace));
    return actionSpace;
}

XrActionStateBoolean
InputState::GetActionStateBoolean(XrSession const &session, XrAction action)  {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;

    XrActionStateBoolean state = {};
    state.type = XR_TYPE_ACTION_STATE_BOOLEAN;

    OXR(xrGetActionStateBoolean(session, &getInfo, &state));
    return state;
}

XrActionStateFloat InputState::GetActionStateFloat(XrSession const &session, XrAction action) {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;

    XrActionStateFloat state = {};
    state.type = XR_TYPE_ACTION_STATE_FLOAT;

    OXR(xrGetActionStateFloat(session, &getInfo, &state));
    return state;
}

XrActionStateVector2f
InputState::GetActionStateVector2(XrSession const &session, XrAction action) {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;

    XrActionStateVector2f state = {};
    state.type = XR_TYPE_ACTION_STATE_VECTOR2F;

    OXR(xrGetActionStateVector2f(session, &getInfo, &state));
    return state;
}

bool InputState::ActionPoseIsActive(XrSession const &session, XrAction action,
                                    XrPath subactionPath)  {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;
    getInfo.subactionPath = subactionPath;

    XrActionStatePose state = {};
    state.type = XR_TYPE_ACTION_STATE_POSE;
    OXR(xrGetActionStatePose(session, &getInfo, &state));
    return state.isActive != XR_FALSE;
}

larkxrControllerInputState InputState::toLarkXRInputState(int hand, const InputState &inputState) {
    larkxrControllerInputState state = {};
    state.isConnected = inputState.ControllerActive[hand];
    state.deviceType = hand == 0 ? Larkxr_Device_Type_Controller_Left : Larkxr_Device_Type_Controller_Right;

    if (inputState.AClick.currentState && inputState.AClick.isActive) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_A_Click);
    }
    if (inputState.BClick.currentState && inputState.BClick.isActive) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_B_Click);
    }
    if (inputState.XClick.currentState && inputState.XClick.isActive) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_X_Click);
    }
    if (inputState.YClick.currentState && inputState.YClick.isActive) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Y_Click);
    }

    if (inputState.TriggerClick[hand].currentState && inputState.TriggerClick[hand].isActive) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);
        LOGV("input trigger click hand[%d]", hand);
    }
    if (inputState.Squeeze[hand].currentState != 0 && inputState.Squeeze[hand].isActive) {
        if (inputState.Squeeze[hand].currentState == 1) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Grip_Click);
        }
        state.gripValue = inputState.Squeeze[hand].currentState;
    }

    state.triggerValue = inputState.TriggerValue[hand].currentState;

    state.touchPadAxis.x = inputState.ThumbValue[hand].currentState.x;
    state.touchPadAxis.y = inputState.ThumbValue[hand].currentState.y;

//        LOGV("touchPadAxis %f %f triggerValue %f gripValue %f", state.touchPadAxis.x, state.touchPadAxis.y, state.triggerValue, state.gripValue);

    if (inputState.ThumbClick[hand].currentState && inputState.ThumbClick[hand].isActive) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click);
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Touch);
    }

    // TODO get controller battery
    state.batteryPercentRemaining = 100;
    lark::XRClient::SetControlerBatteryLevel(hand == 0, state.batteryPercentRemaining);

    return state;
}
}
