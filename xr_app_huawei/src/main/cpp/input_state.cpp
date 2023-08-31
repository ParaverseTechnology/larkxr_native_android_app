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
#include <vector>
#include "input_state.h"
#include "logger.h"
#include "hxr_utils.h"

#define LOG_TAG "input_state"

namespace hxr {
void InputState::InitializeActions(const XrInstance &instance, const XrSession &session) {
    XrResult result;

    LOGI("CreateActions");
    // Create an action set
    LOGW("Create an action set");
    XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
    strcpy(actionSetInfo.actionSetName, "XRDemo");
    strcpy(actionSetInfo.localizedActionSetName, "XRDemo");
    actionSetInfo.priority = 0;
    result = xrCreateActionSet(instance, &actionSetInfo, &m_actionSet);
    LOGI("xrCreateActionSet:%d", result);

    xrStringToPath(instance, "/user/hand/left", &LRPath[0]);
    xrStringToPath(instance, "/user/hand/right", &LRPath[1]);
    xrStringToPath(instance, "/user/head", &headPath);

    // Create actions
    // Suggested Bindings
    std::vector<XrActionSuggestedBinding> bindings;
    for (auto &it : mCtrlHandleActions) {
        XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
        actionInfo.actionType = it.second.actionType;
        strcpy(actionInfo.actionName, it.second.actionName.c_str());
        strcpy(actionInfo.localizedActionName, it.second.actionName.c_str());
        actionInfo.countSubactionPaths = it.second.countSubactionPaths;
        if (it.second.ctrlType == "6Dof_Both") {
            actionInfo.subactionPaths = LRPath;
            xrStringToPath(instance, ("/user/hand/left/input/"  + it.second.actionName + "/" + it.second.pathType).c_str(), &it.second.xrPath[0]);
            xrStringToPath(instance, ("/user/hand/right/input/" + it.second.actionName + "/" + it.second.pathType).c_str(), &it.second.xrPath[1]);
            result = xrCreateAction(m_actionSet, &actionInfo, &(it.second.action));
            LOGI("%s, xrCreateAction:%d", it.first.c_str(), result);
            bindings.push_back({ it.second.action, it.second.xrPath[0] });
            bindings.push_back({ it.second.action, it.second.xrPath[1] });
        } else if (it.second.ctrlType == "6Dof_Left") {
            actionInfo.subactionPaths = &LRPath[0];
            xrStringToPath(instance, ("/user/hand/left/input/"  + it.second.actionName + "/" + it.second.pathType).c_str(), &it.second.xrPath[0]);
            result = xrCreateAction(m_actionSet, &actionInfo, &(it.second.action));
            LOGI("%s, xrCreateAction:%d", it.first.c_str(), result);
            bindings.push_back({ it.second.action, it.second.xrPath[0] });
        } else if (it.second.ctrlType == "6Dof_Right" || it.second.ctrlType == "3Dof") {
            actionInfo.subactionPaths = &LRPath[1];
            xrStringToPath(instance, ("/user/hand/right/input/" + it.second.actionName + "/" + it.second.pathType).c_str(), &it.second.xrPath[1]);
            result = xrCreateAction(m_actionSet, &actionInfo, &(it.second.action));
            LOGI("%s, xrCreateAction:%d", it.first.c_str(), result);
            bindings.push_back({ it.second.action, it.second.xrPath[1] });
        }
    }

    for (auto &it : mHeadHandleActions) {
        XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
        actionInfo.actionType = it.second.actionType;
        strcpy(actionInfo.actionName, it.second.actionName.c_str());
        strcpy(actionInfo.localizedActionName, it.second.actionName.c_str());
        actionInfo.countSubactionPaths = 1;
        actionInfo.subactionPaths = &headPath;
        result = xrCreateAction(m_actionSet, &actionInfo, &(it.second.action));
        LOGI("%s, xrCreateAction:%d", it.first.c_str(), result);
    }

    if (m_isUse6DofProfile) {
        for (auto &it : m_6DoF_CtrlHandleOutputActions) {
            XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
            actionInfo.actionType = it.second.actionType;
            strcpy(actionInfo.actionName, it.second.actionName.c_str());
            strcpy(actionInfo.localizedActionName, it.second.actionName.c_str());
            actionInfo.countSubactionPaths = it.second.countSubactionPaths;
            actionInfo.subactionPaths = LRPath;
            xrStringToPath(instance, ("/user/hand/left/"  + it.second.actionName + "/" + it.second.pathType).c_str(), &it.second.xrPath[0]);
            xrStringToPath(instance, ("/user/hand/right/" + it.second.actionName + "/" + it.second.pathType).c_str(), &it.second.xrPath[1]);
            result = xrCreateAction(m_actionSet, &actionInfo, &(it.second.action));
            LOGI("%s, xrCreateAction:%d", it.first.c_str(), result);
            bindings.push_back({ it.second.action, it.second.xrPath[0] });
            bindings.push_back({ it.second.action, it.second.xrPath[1] });
        }
        xrStringToPath(instance, "/interaction_profiles/huawei/6dof_controller", &khrSimpleInteractionProfilePath);
    } else {
        xrStringToPath(instance, "/interaction_profiles/huawei/controller", &khrSimpleInteractionProfilePath);
    }

    XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
    suggestedBindings.interactionProfile = khrSimpleInteractionProfilePath;
    suggestedBindings.suggestedBindings = bindings.data();
    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    XrResult ret_xrSuggest = xrSuggestInteractionProfileBindings(instance, &suggestedBindings);
    LOGI("ret_xrSuggest:%d", ret_xrSuggest);

    //xrCreateActionSpace
    XrActionSpaceCreateInfo actionSpaceInfo_Ctrl{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo_Ctrl.action = mCtrlHandleActions.find("AimPose")->second.action;
    actionSpaceInfo_Ctrl.poseInActionSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
    actionSpaceInfo_Ctrl.subactionPath = LRPath[0];
    XrResult ret_xrCreateActionSpace_0 = xrCreateActionSpace(session, &actionSpaceInfo_Ctrl, &LRSpace[0]);
    LOGI("ret_xrCreateActionSpace_0:%d", ret_xrCreateActionSpace_0);
    actionSpaceInfo_Ctrl.subactionPath = LRPath[1];
    XrResult ret_xrCreateActionSpace_1 = xrCreateActionSpace(session, &actionSpaceInfo_Ctrl, &LRSpace[1]);
    LOGI("ret_xrCreateActionSpace_1:%d", ret_xrCreateActionSpace_1);

    XrActionSpaceCreateInfo actionSpaceInfo_Head{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo_Head.action = mHeadHandleActions.find("HeadPose")->second.action;
    actionSpaceInfo_Head.poseInActionSpace = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
    actionSpaceInfo_Head.subactionPath = headPath;
    XrResult ret_xrCreateActionSpace_2 = xrCreateActionSpace(session, &actionSpaceInfo_Head, &HeadSpace);
    LOGI("ret_xrCreateActionSpace_2:%d", ret_xrCreateActionSpace_2);

    //xrAttachSessionActionSets
    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &m_actionSet;
    XrResult ret_xrAttach = xrAttachSessionActionSets(session,&attachInfo);
    LOGI("ret_xrAttach:%d",ret_xrAttach);
}

void InputState::InitializeControllerSpace(const XrSession& session) {
}

void InputState::UpDate(const XrSession& session, XrSpace const &space, const XrTime& time) {
    InitializeControllerSpace(session);

    // LOGI("PollActions");
    const XrActiveActionSet activeActionSet{m_actionSet, XR_NULL_PATH};
    XrActionsSyncInfo syncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    xrSyncActions(session, &syncInfo);

    if (m_isUse6DofProfile) {
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};

        // 6Dof_Left
        // button-> y
        getInfo.action = mCtrlHandleActions["YClick"].action;
        getInfo.subactionPath = LRPath[0];
        xrGetActionStateBoolean(session, &getInfo, &YClick);
        // y touch
        getInfo.action = mCtrlHandleActions["YTouch"].action;
        xrGetActionStateBoolean(session, &getInfo, &YTouch);

        // button->x
        getInfo.action = mCtrlHandleActions["XClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &XClick);
        // x touch
        getInfo.action = mCtrlHandleActions["XTouch"].action;
        xrGetActionStateBoolean(session, &getInfo, &XTouch);

        // 6Dof_Right
        // button->b
        getInfo.action = mCtrlHandleActions["BClick"].action;
        getInfo.subactionPath = LRPath[1];
        xrGetActionStateBoolean(session, &getInfo, &BClick);
        //        LOGI("BValue %d YValue %d", BValue.currentState, YValue.currentState);
        // b touch
        getInfo.action = mCtrlHandleActions["BTouch"].action;
        xrGetActionStateBoolean(session, &getInfo, &BTouch);

        // button->a
        getInfo.action = mCtrlHandleActions["AClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &AClick);
        //        LOGI("XValue %d AValue %d", XValue.currentState, AValue.currentState);
        // a touch
        getInfo.action = mCtrlHandleActions["ATouch"].action;
        xrGetActionStateBoolean(session, &getInfo, &ATouch);
    } else {
        XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
        getInfo.subactionPath = LRPath[1];

        getInfo.action = mCtrlHandleActions["HomeClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &HomeClick);

        getInfo.action = mCtrlHandleActions["BackClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &BackClick);

        getInfo.action = mCtrlHandleActions["VolumeUpClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &VolumeUpClick);

        getInfo.action = mCtrlHandleActions["VolumeDownClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &VolumeDownClick);

        getInfo.action = mCtrlHandleActions["TriggerClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &TriggerClick3Dof);

        getInfo.action = mCtrlHandleActions["TrackPadClick"].action;
        xrGetActionStateBoolean(session, &getInfo, &TrackPadClick);

        getInfo.action = mCtrlHandleActions["TrackPadTouch"].action;
        xrGetActionStateBoolean(session, &getInfo, &TrackPadTouch);

        getInfo.action = mCtrlHandleActions["TriggerValue"].action;
        xrGetActionStateFloat(session, &getInfo, &TriggerValue3Dof);

        getInfo.action = mCtrlHandleActions["TrackPadX"].action;
        xrGetActionStateFloat(session, &getInfo, &TrackPadX);

        getInfo.action = mCtrlHandleActions["TrackPadY"].action;
        xrGetActionStateFloat(session, &getInfo, &TrackPadY);

        // pose
        XrActionStatePose State{XR_TYPE_ACTION_STATE_POSE};
        XrActionStateGetInfo Info{XR_TYPE_ACTION_STATE_GET_INFO};
        Info.subactionPath = LRPath[1];
        Info.action = mCtrlHandleActions["AimPose"].action;
        xrGetActionStatePose(session, &Info, &State);
        // LOGI("KEY_STATE_RIGHT: %s state is %d", mCtrlHandleActions["AimPose"].actionName.c_str(), State.isActive);

    }

/*    LOGV("input_state BackClick %d TriggerClick3Dof %d TrackPadClick %d HomeClick %d TriggerValue3Dof %f TrackPad %f %f",
         BackClick.currentState, TriggerClick3Dof.currentState, TrackPadClick.currentState, HomeClick.currentState,
         TriggerValue3Dof.currentState,
         TrackPadX.currentState, TrackPadY.currentState);*/

    for (int hand = 0; hand < 2; hand++) {
        // skip 0 when 3dof
        if (!m_isUse6DofProfile && hand == 0) {
            ControllerActive[hand] = false;

            ControllerState[hand] = {};
           continue;
        }

        bool isLeft = hand == 0;

//        bool isActive = InputState::ActionPoseIsActive(session, aimPoseAction, isLeft ? leftHandPath : rightHandPath);
//
//        if (!isActive) {
//            ControllerActive[hand] = false;
//            continue;
//        }

        XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
        spaceLocation.next = nullptr;
        XrResult res;
        res = xrLocateSpace(LRSpace[hand],space, time, &spaceLocation);
        if (!XR_UNQUALIFIED_SUCCESS(res)) {
            LOGI("xrLocateSpace failed hand %d res %d", hand, res);
            ControllerActive[hand] = false;

            ControllerState[hand] = {};
            continue;
        }

        // skip XR_SPACE_LOCATION_POSITION_VALID_BIT when 3dof
        if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
            ControllerPose[hand] = spaceLocation;
            ControllerActive[hand] = true;
        } else {
            LOGI("xrLocateSpace failed flags %ld; x %f y %f z %f w %f", spaceLocation.locationFlags,
                 spaceLocation.pose.orientation.x,
                 spaceLocation.pose.orientation.y,
                 spaceLocation.pose.orientation.z,
                 spaceLocation.pose.orientation.w);
            ControllerActive[hand] = false;

            ControllerState[hand] = {};
            continue;
        }

        if (m_isUse6DofProfile) {
            XrActionStateGetInfo getInfo{XR_TYPE_ACTION_STATE_GET_INFO};
            getInfo.subactionPath = LRPath[hand];

            getInfo.action = mCtrlHandleActions["TriggerClick"].action;
            xrGetActionStateBoolean(session, &getInfo, &TriggerClick[hand]);

            getInfo.action = mCtrlHandleActions["TriggerValue"].action;
            xrGetActionStateFloat(session, &getInfo, &TriggerValue[hand]);

            getInfo.action = mCtrlHandleActions["GripValue"].action;
            xrGetActionStateFloat(session, &getInfo, &Squeeze[hand]);

            getInfo.action = mCtrlHandleActions["ThumbsticClick"].action;
            xrGetActionStateBoolean(session, &getInfo, &ThumbClick[hand]);

            getInfo.action = mCtrlHandleActions["ThumbsticX"].action;
            xrGetActionStateFloat(session, &getInfo, &ThumbXValue[hand]);

            getInfo.action = mCtrlHandleActions["ThumbsticY"].action;
            xrGetActionStateFloat(session, &getInfo, &ThumbYValue[hand]);

            LOGV("trigger %d squee %f thumb %d", TriggerClick[hand].currentState, Squeeze[hand].currentState, ThumbClick[hand].currentState);
        }


//        XrActionStateFloat battery = {XR_TYPE_ACTION_STATE_FLOAT};
//        getInfo.action = isLeft ? BatteryLeftAction : BatteryRightAction;
//        OXR(xrGetActionStateFloat(session, &getInfo, &battery));

//        LOGV("battery hand[%d] currentState %f isActive %d changedSinceLastSync %d",
//             hand, battery.currentState, battery.isActive, battery.changedSinceLastSync);

        // update larkxr state
        ControllerState[hand].deviceType = hand == 0 ? Larkxr_Controller_Left : Larkxr_Controller_Right;
        ControllerState[hand].pose.isValidPose = true;
        ControllerState[hand].pose.isConnected = true;
        ControllerState[hand].pose.is6Dof = true;

        // simulate arm
        if (!m_isUse6DofProfile) {
            int offset = hand == 0 ? 1 : 0;
            float arm = 0.1f;
            float x = (offset % 2) == 0 ? 0.1f : -0.1f;
            float y = -0.1f;
            float z = -0.15f;

            // warning glm quat w x y z!!
            glm::mat4 rotMat = glm::mat4_cast(glm::quat(1, 0, 0, 0));

            rotMat = glm::translate(rotMat, glm::vec3(x, y, z - arm));

            glm::mat4 deviceMat = glm::mat4_cast(toGlm(ControllerPose[hand].pose.orientation));

            deviceMat = deviceMat * rotMat;

            XrSpaceLocation spaceLocation_h{XR_TYPE_SPACE_LOCATION};
            xrLocateSpace(HeadSpace, space, time, &spaceLocation_h);

            if ((spaceLocation_h.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                // LOGV("head pose invalid flag=%d", spaceLocation_h.locationFlags);
            }

            glm::quat head_rot = toGlm(spaceLocation_h.pose.orientation);
            glm::vec3 head_pos = toGlm(spaceLocation_h.pose.position);

            glm::mat4 headMat = glm::mat4_cast(head_rot);
            headMat = glm::translate(headMat, head_pos);

            glm::mat4 simMat = headMat * deviceMat * rotMat;

            ControllerState[hand].pose.rotation = glm::quat_cast(simMat);
            ControllerState[hand].pose.position.x = simMat[3][0];
            ControllerState[hand].pose.position.y = simMat[3][1];
            ControllerState[hand].pose.position.z = simMat[3][2];

/*            LOGV("ControllerState[%d] x=%f y=%f z=%f", hand, simMat[3][0],
                 simMat[3][1],
                 simMat[3][2]);*/
        } else {
            ControllerState[hand].pose.position = toGlm(ControllerPose[hand].pose.position);
            ControllerState[hand].pose.rotation = toGlm(ControllerPose[hand].pose.orientation);
        }

/*        LOGV("ControllerState[%d] x=%f y=%f z=%f", hand, ControllerState[hand].pose.position.x,
             ControllerState[hand].pose.position.y,
             ControllerState[hand].pose.position.z);*/

        ControllerState[hand].inputState = toLarkXRInputState(hand, *this);
    }
}

XrActionStateBoolean
InputState::GetActionStateBoolean(XrSession const &session, XrAction action)  {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;

    XrActionStateBoolean state = {};
    state.type = XR_TYPE_ACTION_STATE_BOOLEAN;

    xrGetActionStateBoolean(session, &getInfo, &state);
    return state;
}

XrActionStateFloat InputState::GetActionStateFloat(XrSession const &session, XrAction action) {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;

    XrActionStateFloat state = {};
    state.type = XR_TYPE_ACTION_STATE_FLOAT;

    xrGetActionStateFloat(session, &getInfo, &state);
    return state;
}

XrActionStateVector2f
InputState::GetActionStateVector2(XrSession const &session, XrAction action) {
    XrActionStateGetInfo getInfo = {};
    getInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
    getInfo.action = action;

    XrActionStateVector2f state = {};
    state.type = XR_TYPE_ACTION_STATE_VECTOR2F;

    xrGetActionStateVector2f(session, &getInfo, &state);
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
    xrGetActionStatePose(session, &getInfo, &state);
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
        LOGI("input trigger click hand[%d]", hand);
    }
    if (inputState.Squeeze[hand].currentState != 0 && inputState.Squeeze[hand].isActive) {
        if (inputState.Squeeze[hand].currentState == 1) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Grip_Click);
        }
        state.gripValue = inputState.Squeeze[hand].currentState;
    }

    if (inputState.TriggerClick3Dof.currentState) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Trigger_Click);
    }

    if (inputState.BClick.currentState) {
        state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Back_Click);
    }


    if (inputState.m_isUse6DofProfile) {
        state.triggerValue = inputState.TriggerValue[hand].currentState;

        state.touchPadAxis.x = inputState.ThumbXValue[hand].currentState;
        state.touchPadAxis.y = inputState.ThumbYValue[hand].currentState;

//        LOGV("touchPadAxis %f %f triggerValue %f gripValue %f", state.touchPadAxis.x, state.touchPadAxis.y, state.triggerValue, state.gripValue);

        if (inputState.ThumbClick[hand].currentState && inputState.ThumbClick[hand].isActive) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click);
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Touch);
        }
    } else {
        state.triggerValue = inputState.TriggerValue3Dof.currentState;

        state.touchPadAxis.x = inputState.TrackPadX.currentState;
        state.touchPadAxis.y = inputState.TrackPadY.currentState;

//        LOGV("touchPadAxis %f %f triggerValue %f gripValue %f", state.touchPadAxis.x, state.touchPadAxis.y, state.triggerValue, state.gripValue);

        if (inputState.TrackPadClick.currentState) {
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Click);
            state.buttons |= LARKXR_BUTTON_FLAG(larkxrInput::larkxr_Input_Joystick_Touch);
        }
    }

    // TODO get controller battery
    state.batteryPercentRemaining = 100;
    lark::XRClient::SetControlerBatteryLevel(hand == 0, state.batteryPercentRemaining);

    return state;
}
}
