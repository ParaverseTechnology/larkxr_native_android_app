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
        strcpy_s(actionInfo.actionName, "touchpad");
        strcpy_s(actionInfo.localizedActionName, "Touchpad");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &touchpadAction));

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

        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "sidekey");
        strcpy_s(actionInfo.localizedActionName, "Sidekey");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &sideAction));

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "trigger");
        strcpy_s(actionInfo.localizedActionName, "Trigger");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &triggerAction));

        actionInfo.actionType = XR_ACTION_TYPE_VECTOR2F_INPUT;
        strcpy_s(actionInfo.actionName, "joystick");
        strcpy_s(actionInfo.localizedActionName, "Joystick");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &joystickAction));

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "battery");
        strcpy_s(actionInfo.localizedActionName, "battery");
        actionInfo.countSubactionPaths = uint32_t(handSubactionPath.size());
        actionInfo.subactionPaths = handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(actionSet, &actionInfo, &batteryAction));
        /**********************************pico***************************************/

    }

    std::array<XrPath, Side::COUNT> selectPath;
    std::array<XrPath, Side::COUNT> squeezeValuePath;
    std::array<XrPath, Side::COUNT> squeezeClickPath;
    std::array<XrPath, Side::COUNT> posePath;
    std::array<XrPath, Side::COUNT> hapticPath;
    std::array<XrPath, Side::COUNT> menuClickPath;
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
    /**************************pico************************************/
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/select/click", &selectPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/select/click", &selectPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/squeeze/value", &squeezeValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/squeeze/value", &squeezeValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/squeeze/click", &squeezeClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/squeeze/click", &squeezeClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/grip/pose", &posePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/grip/pose", &posePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/output/haptic", &hapticPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/output/haptic", &hapticPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/menu/click", &menuClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/menu/click", &menuClickPath[Side::RIGHT]));


    /**************************pico************************************/
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/touchpad/click", &touchpadPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/touchpad/click", &touchpadPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/ax/click", &AXValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/ax/click", &AXValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/home/click", &homeClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/home/click", &homeClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/by/click", &BYValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/by/click", &BYValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/back/click", &backPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/back/click", &backPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/side/click", &sideClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/side/click", &sideClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/trigger/pose", &triggerPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/trigger/pose", &triggerPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/joystick/pose", &joystickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/joystick/pose", &joystickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/left/input/battery/value", &batteryPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(instance, "/user/hand/right/input/battery/value", &batteryPath[Side::RIGHT]));
    /**************************pico************************************/
    // Suggest bindings for KHR Simple.
    /* {
        XrPath khrSimpleInteractionProfilePath;
         CHECK_XRCMD(
             xrStringToPath(instance, "/interaction_profiles/khr/simple_controller", &khrSimpleInteractionProfilePath));
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
        CHECK_XRCMD(xrStringToPath(instance, "/interaction_profiles/pico/cv2_controller",
                                   &picoMixedRealityInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{{
                                                               {poseAction, posePath[Side::LEFT]},
                                                               {poseAction, posePath[Side::RIGHT]},

                                                               {touchpadAction, touchpadPath[Side::LEFT]},
                                                               {touchpadAction, touchpadPath[Side::RIGHT]},
                                                               {AXAction, AXValuePath[Side::LEFT]},
                                                               {AXAction, AXValuePath[Side::RIGHT]},
                                                               {homeAction, homeClickPath[Side::LEFT]},
                                                               {homeAction, homeClickPath[Side::RIGHT]},
                                                               {BYAction, BYValuePath[Side::LEFT]},
                                                               {BYAction, BYValuePath[Side::RIGHT]},
                                                               {backAction, backPath[Side::LEFT]},
                                                               {backAction, backPath[Side::RIGHT]},
                                                               {sideAction, sideClickPath[Side::LEFT]},
                                                               {sideAction, sideClickPath[Side::RIGHT]},
                                                               {triggerAction, triggerPath[Side::LEFT]},
                                                               {triggerAction, triggerPath[Side::RIGHT]},
                                                               {joystickAction, joystickPath[Side::LEFT]},
                                                               {joystickAction, joystickPath[Side::RIGHT]},
                                                               {batteryAction, batteryPath[Side::LEFT]},
                                                               {batteryAction, batteryPath[Side::RIGHT]}
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

    XrSessionActionSetsAttachInfo attachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(session, &attachInfo));
}

