//
// Created by Administrator on 2021/7/7.
//

#ifndef CLOUDLARKXR_XR_INPUT_STATE_H
#define CLOUDLARKXR_XR_INPUT_STATE_H

#include "openxr/openxr.h"
#include "array"

namespace Side {
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
}  // namespace Side

struct InputState {
    XrActionSet actionSet{XR_NULL_HANDLE};
    XrAction grabAction{XR_NULL_HANDLE};
    XrAction poseAction{XR_NULL_HANDLE};
    XrAction vibrateAction{XR_NULL_HANDLE};
    XrAction quitAction{XR_NULL_HANDLE};
    /*************************pico******************/
    XrAction touchpadAction{XR_NULL_HANDLE};
    XrAction AXAction{XR_NULL_HANDLE};
    XrAction homeAction{XR_NULL_HANDLE};
    XrAction BYAction{XR_NULL_HANDLE};
    XrAction backAction{XR_NULL_HANDLE};
    XrAction sideAction{XR_NULL_HANDLE};
    XrAction triggerAction{XR_NULL_HANDLE};
    XrAction joystickAction{XR_NULL_HANDLE};
    XrAction batteryAction{XR_NULL_HANDLE};
    //---add new----------
    XrAction AXTouchAction{XR_NULL_HANDLE};
    XrAction BYTouchAction{XR_NULL_HANDLE};
    XrAction RockerTouchAction{XR_NULL_HANDLE};
    XrAction TriggerTouchAction{XR_NULL_HANDLE};
    XrAction ThumbrestTouchAction{XR_NULL_HANDLE};
    XrAction GripAction{XR_NULL_HANDLE};
    //---add new----------zgt
    XrAction AAction{XR_NULL_HANDLE};
    XrAction BAction{XR_NULL_HANDLE};
    XrAction XAction{XR_NULL_HANDLE};
    XrAction YAction{XR_NULL_HANDLE};
    XrAction ATouchAction{XR_NULL_HANDLE};
    XrAction BTouchAction{XR_NULL_HANDLE};
    XrAction XTouchAction{XR_NULL_HANDLE};
    XrAction YTouchAction{XR_NULL_HANDLE};
    XrAction aimAction{XR_NULL_HANDLE};
    /*************************pico******************/
    std::array<XrSpace, Side::COUNT> aimSpace;
    std::array<XrPath, Side::COUNT> handSubactionPath;
    std::array<XrSpace, Side::COUNT> handSpace;
    std::array<float, Side::COUNT> handScale = {{1.0f, 1.0f}};
    std::array<XrVector2f, Side::COUNT> handXYPos = {0.0f};
    std::array<XrBool32, Side::COUNT> handActive;

    void InitializeActions(const XrInstance& instance, const XrSession& session);
};
#endif //CLOUDLARKXR_XR_INPUT_STATE_H
