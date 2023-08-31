/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: OpenXR Huawei EXT
 * Author: t00500781
 * Create: 2020-06-08
 */

#ifndef OPENXR_HW_H_
#define OPENXR_HW_H_ 1

#include "openxr.h"
#include "openxr_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XR_HW_EXT_VERSION 1
#define XR_HW_EXTENSION_VERSION 1

#define XR_HW_JUMPTOLAUNCHER_NAME "xrJumpToLauncherHW"
#define XR_HW_GETHELMETTYPE_NAME "xrGetHelmetTypeHW"
#define XR_HW_GETPHONECONTROILLERPOWERSTATE_NAME "xrGetPhoneControllerPowerStateHW"
#define XR_HW_RESETYAW_NAME "xrResetYawHW"
#define XR_HW_TURNOFFSHORTCUTS_NAME "xrTurnOffShortCutsHW"
#define XR_HW_ISCONTROLLERAVAIABLE_NAME "xrIsControllerAvailableHW"
#define XR_HW_GETCONTROLLERTYPE_NAME "xrGetControllerTypeHW"
#define XR_HW_SENDMESSAGE "xrSendCommandHW"
#define XR_HUAWEI_CONTROLLER_INTERACTION "XR_HUAWEI_controller_interaction"
#define XR_HW_FOVEATED_NAME "xrfoveatedHW"

#ifndef __XrDeviceTypeHW__
#define __XrDeviceTypeHW__
typedef enum XrDeviceTypeHW {
    XR_TYPE_DEVICE_UNKNOWN_HW = 0,
    XR_TYPE_HELMET_VR2_HW = 100,
    XR_TYPE_HELMET_VR3_HW = 101,
    XR_TYPE_CONTROLLER_PHONE_HW = 200,
    XR_TYPE_CONTROLLER_COMMON_HW = 201
} XrDeviceTypeHW;
#endif

#ifndef __XrStructureTypeHW__
#define __XrStructureTypeHW__
typedef enum XrStructureTypeHW {
    XR_TYPE_STRUCTURE_UNKNOWN_HW = 0,
    XR_TYPE_TURN_OFF_RESET_YAW_HW = 100,
    XR_TYPE_TURN_OFF_HOME_JUMP_TO_LAUNCHER_HW = 101,
    XR_TYPE_TURN_OFF_BACK_JUMP_TO_LAUNCHER_HW = 102
} XrStructureTypeHW;
#endif

typedef XrResult (XRAPI_PTR *PFN_xrJumpToLauncherHW)(XrInstance instance);
typedef XrResult (XRAPI_PTR *PFN_xrGetHelmetTypeHW)(XrInstance instance, XrDeviceTypeHW* type);
typedef XrResult (XRAPI_PTR *PFN_xrGetPhoneControllerPowerStateHW)(XrInstance instance, int32_t* state);
typedef XrResult (XRAPI_PTR *PFN_xrResetYawHW)(XrInstance instance, XrPath path);
typedef XrResult (XRAPI_PTR *PFN_xrTurnOffShortCutsHW)(XrInstance instance, XrStructureTypeHW type);
typedef XrResult (XRAPI_PTR *PFN_xrIsControllerAvailableHW)(XrInstance instance, XrPath path, int32_t* state);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerTypeHW)(XrInstance instance, XrDeviceTypeHW *type);
typedef XrResult (XRAPI_PTR *PFN_xrSendCommandHW)(XrInstance instance, uint32_t command);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrJumpToLauncherHW(
    XrInstance                                  instance);

XRAPI_ATTR XrResult XRAPI_CALL xrGetHelmetTypeHW(
    XrInstance                                  instance,
    XrDeviceTypeHW*                             type);

XRAPI_ATTR XrResult XRAPI_CALL xrGetPhoneControllerPowerStateHW(
    XrInstance                                  instance,
    int32_t*                                    state);

XRAPI_ATTR XrResult XRAPI_CALL xrResetYawHW(
    XrInstance                                  instance,
    XrPath                                      path);

XRAPI_ATTR XrResult XRAPI_CALL xrTurnOffShortCutsHW(
        XrInstance                                  instance,
        XrStructureTypeHW type);

XRAPI_ATTR XrResult XRAPI_CALL xrIsControllerAvailableHW(
        XrInstance instance,
        XrPath path,
        int32_t *state);

XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerTypeHW(
        XrInstance instance,
        XrDeviceTypeHW *type);
XRAPI_ATTR XrResult XRAPI_CALL xrSendCommandHW(
        XrInstance instance,
        uint32_t command);
#endif

#ifdef __cplusplus
}
#endif

#endif
