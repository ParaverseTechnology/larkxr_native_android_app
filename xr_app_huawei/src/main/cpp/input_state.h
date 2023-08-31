//
// Created by fcx@pingxingyung.com on 2022/11/10.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_INPUT_STATE_H
#define LARKXR_INPUT_STATE_H

#include <jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <cstring>
#include "inttypes.h"
#include <map>
#include <lark_xr/types.h>

namespace hxr {

typedef struct HandleAction {
    XrActionType actionType;
    std::string ctrlType;
    std::string actionName;
    std::string pathType;
    uint32_t countSubactionPaths;
    XrPath xrPath[2];
    XrAction action;
} HandleAction;

struct InputState {
    static XrActionStateBoolean GetActionStateBoolean(const XrSession& session, XrAction action);

    static XrActionStateFloat GetActionStateFloat(const XrSession& session, XrAction action);

    static XrActionStateVector2f GetActionStateVector2(const XrSession& session, XrAction action);

    static bool ActionPoseIsActive(const XrSession& session, XrAction action, XrPath subactionPath);

    static larkxrControllerInputState toLarkXRInputState(int hand, const InputState& inputState);


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
    XrActionStateFloat ThumbXValue[2] = { {XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT} };
    XrActionStateFloat ThumbYValue[2] = { {XR_TYPE_ACTION_STATE_FLOAT}, {XR_TYPE_ACTION_STATE_FLOAT} };
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

    // 3dof
    XrActionStateBoolean HomeClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean BackClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean VolumeUpClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean VolumeDownClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean TriggerClick3Dof{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateFloat TriggerValue3Dof = {XR_TYPE_ACTION_STATE_FLOAT};
    XrActionStateBoolean TrackPadClick{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateBoolean TrackPadTouch{XR_TYPE_ACTION_STATE_BOOLEAN};
    XrActionStateFloat TrackPadX = {XR_TYPE_ACTION_STATE_FLOAT};
    XrActionStateFloat TrackPadY = {XR_TYPE_ACTION_STATE_FLOAT};

    larkxrControllerDeviceState ControllerState[2] = {};

    void InitializeActions(const XrInstance& instance, const XrSession& session);

    // call aftre session active;
    void InitializeControllerSpace(const XrSession& session);

    // Update input state every frame
    // sync action data
    // InitializeControllerSpace
    void UpDate(const XrSession& session, XrSpace const &space, const XrTime& time);

    XrActionSet m_actionSet;
    XrPath LRPath[2];
    XrPath headPath;
    XrSpace LRSpace[2];
    XrSpace HeadSpace;

    std::map<std::string, HandleAction> m_3DoF_CtrlHandleActions = {
            {"HomeClick",       {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "home",       "click", 1, {}, 0}},
            {"BackClick",       {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "back",       "click", 1, {}, 0}},
            {"VolumeUpClick",   {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "volume_up",  "click", 1, {}, 0}},
            {"VolumeDownClick", {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "volume_down","click", 1, {}, 0}},
            {"TriggerValue",    {XR_ACTION_TYPE_FLOAT_INPUT,      "3Dof",    "trigger",    "value", 1, {}, 0}},
            {"TriggerClick",    {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "trigger",    "click", 1, {}, 0}},
            {"TrackPadX",       {XR_ACTION_TYPE_FLOAT_INPUT,      "3Dof",    "trackpad",   "x",     1, {}, 0}},
            {"TrackPadY",       {XR_ACTION_TYPE_FLOAT_INPUT,      "3Dof",    "trackpad",   "y",     1, {}, 0}},
            {"TrackPadClick",   {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "trackpad",   "click", 1, {}, 0}},
            {"TrackPadTouch",   {XR_ACTION_TYPE_BOOLEAN_INPUT,    "3Dof",    "trackpad",   "touch", 1, {}, 0}},
            {"AimPose",         {XR_ACTION_TYPE_POSE_INPUT,       "3Dof",    "aim",        "pose",  1, {}, 0}}
    };

    std::map<std::string, HandleAction> m_6DoF_CtrlHandleActions = {
            {"XClick",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Left",   "x",            "click", 1, {}, 0}},
            {"XTouch",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Left",   "x",            "touch", 1, {}, 0}},
            {"YClick",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Left",   "y",            "click", 1, {}, 0}},
            {"YTouch",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Left",   "y",            "touch", 1, {}, 0}},
            {"MeauClick",       {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Left",   "menu",         "click", 1, {}, 0}},
            {"AClick",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Right",  "a",            "click", 1, {}, 0}},
            {"ATouch",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Right",  "a",            "touch", 1, {}, 0}},
            {"BClick",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Right",  "b",            "click", 1, {}, 0}},
            {"BTouch",          {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Right",  "b",            "touch", 1, {}, 0}},
            {"HomeClick",       {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Right",  "home",         "click", 1, {}, 0}},
            {"TriggerTouch",    {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Both",   "trigger",      "touch", 2, {}, 0}},
            {"TriggerValue",    {XR_ACTION_TYPE_FLOAT_INPUT,      "6Dof_Both",   "trigger",      "value", 2, {}, 0}},
            {"TriggerClick",    {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Both",   "trigger",      "click", 2, {}, 0}},
            {"GripTouch",       {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Both",   "grip",         "touch", 2, {}, 0}},
            {"GripValue",       {XR_ACTION_TYPE_FLOAT_INPUT,      "6Dof_Both",   "grip",         "value", 2, {}, 0}},
            {"GripClick",       {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Both",   "grip",         "click", 2, {}, 0}},
            {"ThumbsticX",      {XR_ACTION_TYPE_FLOAT_INPUT,      "6Dof_Both",   "thumbstick",   "x",     2, {}, 0}},
            {"ThumbsticY",      {XR_ACTION_TYPE_FLOAT_INPUT,      "6Dof_Both",   "thumbstick",   "y",     2, {}, 0}},
            {"ThumbsticClick",  {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Both",   "thumbstick",   "click", 2, {}, 0}},
            {"ThumbsticTouch",  {XR_ACTION_TYPE_BOOLEAN_INPUT,    "6Dof_Both",   "thumbstick",   "touch", 2, {}, 0}},
            {"AimPose",         {XR_ACTION_TYPE_POSE_INPUT,       "6Dof_Both",   "aim",          "pose",  2, {}, 0}},
    };

    std::map<std::string, HandleAction> m_6DoF_CtrlHandleOutputActions = {
            {"Haptic",          {XR_ACTION_TYPE_VIBRATION_OUTPUT, "6Dof_Both",   "output",       "haptic",2, {}, 0}}
    };

    bool m_isUse6DofProfile = false; // To use 3Dof, it must remove the setting about "com.huawei.vr.application.freeDegree" in AndroidManifest.xml.
    std::map<std::string, HandleAction> mCtrlHandleActions = m_isUse6DofProfile ? m_6DoF_CtrlHandleActions : m_3DoF_CtrlHandleActions;

    std::map<std::string, HandleAction> mHeadHandleActions = {
            {"HeadPose",       {XR_ACTION_TYPE_POSE_INPUT,        "36Dof",   "aim",        "pose",  1, {}, 0}}
    };

    XrPath khrSimpleInteractionProfilePath;
    XrPath huaweiInteractionProfilePath;
};
}

#endif //LARKXR_INPUT_STATE_H
