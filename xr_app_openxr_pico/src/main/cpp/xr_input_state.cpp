//
// Created by Administrator on 2021/7/20.
//

#include "xr_input_state.h"
#include <cstring>
#include "pch.h"
#include "common.h"
#include "vector"
#include "array"

#if !defined(XR_USE_PLATFORM_WIN32)
#define strcpy_s(dest, source) strncpy((dest), (source), sizeof(dest))
#endif

void InputState::InitializeActions(const XrInstance& instance, const XrSession& session) {
    // Create an action set.
    {
        XrActionSetCreateInfo actionSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
        strcpy_s(actionSetInfo.actionSetName, "gameplay");
        strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
        actionSetInfo.priority = 0;
        CHECK_XRCMD(xrCreateActionSet(instance, &actionSetInfo, &actionSet));
    }

    // Get the XrPath for the left and right hands - we will use them as subaction paths.
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left", &handSubactionPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right", &handSubactionPath[Side::RIGHT]));

    // Create actions.
    {
        // Create an input action for grabbing objects with the left and right hands.
        XrActionCreateInfo actionInfo{XR_TYPE_ACTION_CREATE_INFO};
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "grab_object");
        strcpy_s(actionInfo.localizedActionName, "Grab Object");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &grabAction));

        // Create an input action getting the left and right hand poses.
        actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
        strcpy_s(actionInfo.actionName, "hand_pose");
        strcpy_s(actionInfo.localizedActionName, "Hand Pose");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &poseAction));

        actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
        strcpy_s(actionInfo.actionName, "aim_pose");
        strcpy_s(actionInfo.localizedActionName, "Aim Pose");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &aimAction));

        // Create output actions for vibrating the left and right controller.
        actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
        strcpy_s(actionInfo.actionName, "vibrate_hand");
        strcpy_s(actionInfo.localizedActionName, "Vibrate Hand");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &vibrateAction));

        // Create input actions for quitting the session using the left and right controller.
        // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
        // We will just suggest bindings for both hands, where possible.
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "quit_session");
        strcpy_s(actionInfo.localizedActionName, "Quit Session");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &quitAction));
        /**********************************pico***************************************/
        // Create input actions for toucpad key using the left and right controller.
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "axkey");
        strcpy_s(actionInfo.localizedActionName, "AXkey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &AXAction));


        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "homekey");
        strcpy_s(actionInfo.localizedActionName, "Homekey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &homeAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "bykey");
        strcpy_s(actionInfo.localizedActionName, "BYkey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &BYAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "backkey");
        strcpy_s(actionInfo.localizedActionName, "Backkey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &backAction));

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "trigger");
        strcpy_s(actionInfo.localizedActionName, "Trigger");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &triggerAction));

        actionInfo.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
        strcpy_s(actionInfo.actionName, "joystick_left");
        strcpy_s(actionInfo.localizedActionName, "JoystickLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &JoystickValueLeftAction));

        actionInfo.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
        strcpy_s(actionInfo.actionName, "joystick_right");
        strcpy_s(actionInfo.localizedActionName, "JoystickRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &JoystickValueRightAction));

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "battery");
        strcpy_s(actionInfo.localizedActionName, "battery");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &batteryAction));
        //------------------------add new---------------------------------
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "axtouch");
        strcpy_s(actionInfo.localizedActionName, "AXtouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &AXTouchAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "bytouch");
        strcpy_s(actionInfo.localizedActionName, "BYtouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &BYTouchAction));

        // trigger touch left
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "triggertouch_left");
        strcpy_s(actionInfo.localizedActionName, "TriggertouchLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &TriggerTouchLeftAction));

        // trigger touch right
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "triggertouch_right");
        strcpy_s(actionInfo.localizedActionName, "TriggertouchRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &TriggerTouchRightAction));

        // trigger left click
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "triggerclickleft");
        strcpy_s(actionInfo.localizedActionName, "TriggerClickLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &TriggerClickLeftAction));

        // trigger right click
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "triggerclickright");
        strcpy_s(actionInfo.localizedActionName, "TriggerClickRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &TriggerClickRightAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "thumbresttouch");
        strcpy_s(actionInfo.localizedActionName, "Thumbresttouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &ThumbrestTouchAction));

        // grip left touch
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "gripvalue_left");
        strcpy_s(actionInfo.localizedActionName, "GripValueLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &GripValueLeftAction));

        // grip right touch
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "gripvalue_right");
        strcpy_s(actionInfo.localizedActionName, "GripValueRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &GripValueRightAction));

        // grip left click
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "grip_click_left");
        strcpy_s(actionInfo.localizedActionName, "GripClickLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &GripClickLeftAction));

        // grip right click
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "grip_click_right");
        strcpy_s(actionInfo.localizedActionName, "GripClickRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &GripClickRightAction));

        // joyleft click
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "joystick_click_left");
        strcpy_s(actionInfo.localizedActionName, "JoystikClickLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &JoystickClickLeftAction));

        // joyright click
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "joystick_click_right");
        strcpy_s(actionInfo.localizedActionName, "JoystikClickRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &JoystickClickRightAction));

        // joystick left touch
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "joystick_touch_left");
        strcpy_s(actionInfo.localizedActionName, "JoystickTouchLeft");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &JoystickTouchLeftAction));

        // joystick right touch
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "joystick_touch_right");
        strcpy_s(actionInfo.localizedActionName, "JoystickTouchRight");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &JoystickTouchRightAction));

        //--------------add new----------zgt
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "akey");
        strcpy_s(actionInfo.localizedActionName, "Akey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &AAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "bkey");
        strcpy_s(actionInfo.localizedActionName, "Bkey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &BAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "xkey");
        strcpy_s(actionInfo.localizedActionName, "Xkey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &XAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "ykey");
        strcpy_s(actionInfo.localizedActionName, "Ykey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &YAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "atouch");
        strcpy_s(actionInfo.localizedActionName, "Atouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &ATouchAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "btouch");
        strcpy_s(actionInfo.localizedActionName, "Btouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &BTouchAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "xtouch");
        strcpy_s(actionInfo.localizedActionName, "Xtouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &XTouchAction));

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "ytouch");
        strcpy_s(actionInfo.localizedActionName, "Ytouch");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &YTouchAction));
        /**********************************pico***************************************/


    }

    std::array<XrPath, Side::COUNT> selectPath;
    std::array<XrPath, Side::COUNT> squeezeValuePath;
    std::array<XrPath, Side::COUNT> squeezeClickPath;
    std::array<XrPath, Side::COUNT> posePath;
    std::array<XrPath, Side::COUNT> hapticPath;
    std::array<XrPath, Side::COUNT> menuClickPath;
    std::array<XrPath, Side::COUNT> systemPath;
    std::array<XrPath, Side::COUNT> thumbrestPath;
    std::array<XrPath, Side::COUNT> triggerTouchPath;
    std::array<XrPath, Side::COUNT> triggerClickPath;
    std::array<XrPath, Side::COUNT> triggerValuePath;
    std::array<XrPath, Side::COUNT> thumbstickClickPath;
    std::array<XrPath, Side::COUNT> thumbstickTouchPath;
    std::array<XrPath, Side::COUNT> thumbstickPosPath;
    std::array<XrPath, Side::COUNT> aimPath;

    /**************************pico************************************/
    std::array<XrPath, Side::COUNT> touchpadPath;
    std::array<XrPath, Side::COUNT> AXValuePath;
    std::array<XrPath, Side::COUNT> homeClickPath;
    std::array<XrPath, Side::COUNT> BYValuePath;
    std::array<XrPath, Side::COUNT> backPath;
    std::array<XrPath, Side::COUNT> sideClickPath;
    std::array<XrPath, Side::COUNT> triggerPath;
    std::array<XrPath, Side::COUNT> joystickPath;
    std::array<XrPath, Side::COUNT> batteryPath;
    //--------------add new----------
    std::array<XrPath, Side::COUNT> GripPath;
    std::array<XrPath, Side::COUNT> AXTouchPath;
    std::array<XrPath, Side::COUNT> BYTouchPath;
    std::array<XrPath, Side::COUNT> RockerTouchPath;
    std::array<XrPath, Side::COUNT> TriggerTouchPath;
    std::array<XrPath, Side::COUNT> ThumbresetTouchPath;
    /**************************pico************************************/
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/select/click", &selectPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/select/click", &selectPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/menu/click", &menuClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/menu/click", &menuClickPath[Side::RIGHT]));

    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/squeeze/value", &squeezeValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/squeeze/value", &squeezeValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/squeeze/click", &squeezeClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/squeeze/click", &squeezeClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/grip/pose", &posePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/grip/pose", &posePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/aim/pose", &aimPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/aim/pose", &aimPath[Side::RIGHT]));

    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/output/haptic", &hapticPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/output/haptic", &hapticPath[Side::RIGHT]));

    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/trigger/touch", &triggerTouchPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/trigger/touch", &triggerTouchPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/trigger/click", &triggerClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/trigger/click", &triggerClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/trigger/value", &triggerValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/trigger/value", &triggerValuePath[Side::RIGHT]));

    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/thumbstick/click", &thumbstickClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/thumbstick/click", &thumbstickClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/thumbstick/touch", &thumbstickTouchPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/thumbstick/touch", &thumbstickTouchPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/thumbstick", &thumbstickPosPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/thumbstick", &thumbstickPosPath[Side::RIGHT]));

    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/system/click", &systemPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/system/click", &systemPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/thumbrest/touch", &thumbrestPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/thumbrest/touch", &thumbrestPath[Side::RIGHT]));

    /**************************pico************************************/
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/back/click", &backPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/back/click", &backPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/battery/value", &batteryPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/battery/value", &batteryPath[Side::RIGHT]));

    //--------------add new----------zgt
    std::array<XrPath, Side::COUNT> AValuePath;
    std::array<XrPath, Side::COUNT> BValuePath;
    std::array<XrPath, Side::COUNT> XValuePath;
    std::array<XrPath, Side::COUNT> YValuePath;
    std::array<XrPath, Side::COUNT> ATouchPath;
    std::array<XrPath, Side::COUNT> BTouchPath;
    std::array<XrPath, Side::COUNT> XTouchPath;
    std::array<XrPath, Side::COUNT> YTouchPath;
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/x/click", &XValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/y/click", &YValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/a/click", &AValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/b/click", &BValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/x/touch", &XTouchPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/y/touch", &YTouchPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/a/touch", &ATouchPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/b/touch", &BTouchPath[Side::RIGHT]));
    /**************************pico************************************/
    // Suggest bindings for KHR Simple.
    /* {
        XrPath khrSimpleInteractionProfilePath;
         CHECK_XRCMD(
             xrStringToPath(up.mInstance, "/interaction_profiles/khr/simple_controller", &khrSimpleInteractionProfilePath));
         std::vector<XrActionSuggestedBinding> bindings{{// Fall back to a click input for the grab action.
                                                         {grabAction, selectPath[Side::LEFT]},
                                                         {grabAction, selectPath[Side::RIGHT]},
                                                         {poseAction, posePath[Side::LEFT]},
                                                         {poseAction, posePath[Side::RIGHT]},
                                                         {quitAction, menuClickPath[Side::LEFT]},
                                                         {quitAction, menuClickPath[Side::RIGHT]},
                                                         {vibrateAction, hapticPath[Side::LEFT]},
                                                         {vibrateAction, hapticPath[Side::RIGHT]}}};
         XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
         suggestedBindings.interactionProfile = khrSimpleInteractionProfilePath;
         suggestedBindings.suggestedBindings = bindings.data();
         suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
         CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));
     }
     // Suggest bindings for the Oculus Touch.
     {
         XrPath oculusTouchInteractionProfilePath;
         CHECK_XRCMD(
             xrStringToPath(instance, "/interaction_profiles/oculus/touch_controller", &oculusTouchInteractionProfilePath));
         std::vector<XrActionSuggestedBinding> bindings{{{grabAction, squeezeValuePath[Side::LEFT]},
                                                         {grabAction, squeezeValuePath[Side::RIGHT]},
                                                         {poseAction, posePath[Side::LEFT]},
                                                         {poseAction, posePath[Side::RIGHT]},
                                                         {quitAction, menuClickPath[Side::LEFT]},
                                                         {vibrateAction, hapticPath[Side::LEFT]},
                                                         {vibrateAction, hapticPath[Side::RIGHT]}}};
         XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
         suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
         suggestedBindings.suggestedBindings = bindings.data();
         suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
         CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));
     }
     // Suggest bindings for the Vive Controller.
     {
         XrPath viveControllerInteractionProfilePath;
         CHECK_XRCMD(
             xrStringToPath(instance, "/interaction_profiles/htc/vive_controller", &viveControllerInteractionProfilePath));
         std::vector<XrActionSuggestedBinding> bindings{{{grabAction, squeezeClickPath[Side::LEFT]},
                                                         {grabAction, squeezeClickPath[Side::RIGHT]},
                                                         {poseAction, posePath[Side::LEFT]},
                                                         {poseAction, posePath[Side::RIGHT]},
                                                         {quitAction, menuClickPath[Side::LEFT]},
                                                         {quitAction, menuClickPath[Side::RIGHT]},
                                                         {vibrateAction, hapticPath[Side::LEFT]},
                                                         {vibrateAction, hapticPath[Side::RIGHT]}}};
         XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
         suggestedBindings.interactionProfile = viveControllerInteractionProfilePath;
         suggestedBindings.suggestedBindings = bindings.data();
         suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
         CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));
     }

     // Suggest bindings for the Microsoft Mixed Reality Motion Controller.
     {
         XrPath microsoftMixedRealityInteractionProfilePath;
         CHECK_XRCMD(xrStringToPath(instance, "/interaction_profiles/microsoft/motion_controller",
                                    &microsoftMixedRealityInteractionProfilePath));
         std::vector<XrActionSuggestedBinding> bindings{{{grabAction, squeezeClickPath[Side::LEFT]},
                                                         {grabAction, squeezeClickPath[Side::RIGHT]},
                                                         {poseAction, posePath[Side::LEFT]},
                                                         {poseAction, posePath[Side::RIGHT]},
                                                         {quitAction, menuClickPath[Side::LEFT]},
                                                         {quitAction, menuClickPath[Side::RIGHT]},
                                                         {vibrateAction, hapticPath[Side::LEFT]},
                                                         {vibrateAction, hapticPath[Side::RIGHT]}}};
         XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
         suggestedBindings.interactionProfile = microsoftMixedRealityInteractionProfilePath;
         suggestedBindings.suggestedBindings = bindings.data();
         suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
         CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));
     }*/
    // Suggest bindings for the Microsoft Mixed Reality Motion Controller.
    {
        XrPath picoMixedRealityInteractionProfilePath;
        CHECK_XRCMD(xrStringToPath(instance, "/interaction_profiles/pico/neo3_controller",
                                   &picoMixedRealityInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{{

                                                               {JoystickClickLeftAction, thumbstickClickPath[Side::LEFT]},
                                                               {JoystickClickRightAction, thumbstickClickPath[Side::RIGHT]},
                                                               {JoystickValueLeftAction, thumbstickPosPath[Side::LEFT]},
                                                               {JoystickValueRightAction, thumbstickPosPath[Side::RIGHT]},
                                                               {JoystickTouchLeftAction, thumbstickTouchPath[Side::LEFT]},
                                                               {JoystickTouchRightAction, thumbstickTouchPath[Side::RIGHT]},

                                                               {triggerAction, triggerValuePath[Side::LEFT]},
                                                               {triggerAction, triggerValuePath[Side::RIGHT]},
                                                               {TriggerClickLeftAction, triggerClickPath[Side::LEFT]},
                                                               {TriggerClickRightAction, triggerClickPath[Side::RIGHT]},
                                                               {TriggerTouchLeftAction, triggerTouchPath[Side::LEFT]},
                                                               {TriggerTouchRightAction, triggerTouchPath[Side::RIGHT]},

                                                               {GripClickLeftAction, squeezeClickPath[Side::LEFT]},
                                                               {GripClickRightAction, squeezeClickPath[Side::RIGHT]},
                                                               {GripValueLeftAction, squeezeValuePath[Side::LEFT]},
                                                               {GripValueRightAction, squeezeValuePath[Side::RIGHT]},
                                                               {poseAction, posePath[Side::LEFT]},
                                                               {poseAction, posePath[Side::RIGHT]},

                                                               {homeAction, systemPath[Side::LEFT]},
                                                               {homeAction, systemPath[Side::RIGHT]},
                                                               {backAction, backPath[Side::LEFT]},
                                                               {backAction, backPath[Side::RIGHT]},
                                                               {batteryAction, batteryPath[Side::LEFT]},
                                                               {batteryAction, batteryPath[Side::RIGHT]},

                                                               {ThumbrestTouchAction, thumbrestPath[Side::LEFT]},
                                                               {ThumbrestTouchAction, thumbrestPath[Side::RIGHT]},

                                                               /*{AXAction, AXValuePath[Side::LEFT]},
                                                               {AXAction, AXValuePath[Side::RIGHT]},
                                                               {BYAction, BYValuePath[Side::LEFT]},
                                                               {BYAction, BYValuePath[Side::RIGHT]},
                                                               {AXTouchAction, AXTouchPath[Side::LEFT]},
                                                               {AXTouchAction, AXTouchPath[Side::RIGHT]},
                                                               {BYTouchAction, BYTouchPath[Side::LEFT]},
                                                               {BYTouchAction, BYTouchPath[Side::RIGHT]},*/
                                                               //--------------add new------------------zgt
                                                               {XTouchAction, XTouchPath[Side::LEFT]},
                                                               {YTouchAction, YTouchPath[Side::LEFT]},
                                                               {ATouchAction, ATouchPath[Side::RIGHT]},
                                                               {BTouchAction, BTouchPath[Side::RIGHT]},
                                                               {XAction, XValuePath[Side::LEFT]},
                                                               {YAction, YValuePath[Side::LEFT]},
                                                               {AAction, AValuePath[Side::RIGHT]},
                                                               {BAction, BValuePath[Side::RIGHT]},
                                                               {aimAction, aimPath[Side::LEFT]},
                                                               {aimAction, aimPath[Side::RIGHT]}
                                                       }};
        XrInteractionProfileSuggestedBinding suggestedBindings{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
        suggestedBindings.interactionProfile = picoMixedRealityInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(instance, &suggestedBindings));
    }

    XrActionSpaceCreateInfo actionSpaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    actionSpaceInfo.action = poseAction;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = handSubactionPath[Side::LEFT];
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &handSpace[Side::LEFT]));
    actionSpaceInfo.subactionPath = handSubactionPath[Side::RIGHT];
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &handSpace[Side::RIGHT]));
    actionSpaceInfo.action = aimAction;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = handSubactionPath[Side::LEFT];
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &aimSpace[Side::LEFT]));
    actionSpaceInfo.subactionPath = handSubactionPath[Side::RIGHT];
    CHECK_XRCMD(xrCreateActionSpace(session, &actionSpaceInfo, &aimSpace[Side::RIGHT]));

    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(session, &attachInfo));
}

