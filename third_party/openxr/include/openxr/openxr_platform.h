#ifndef OPENXR_PLATFORM_H_
#define OPENXR_PLATFORM_H_ 1

/*
** Copyright (c) 2017-2020 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0 OR MIT
*/

/*
** This header is generated from the Khronos OpenXR XML API Registry.
**
*/

#include "openxr.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef XR_USE_PLATFORM_ANDROID

#define XR_KHR_android_thread_settings 1
#define XR_KHR_android_thread_settings_SPEC_VERSION 5
#define XR_KHR_ANDROID_THREAD_SETTINGS_EXTENSION_NAME "XR_KHR_android_thread_settings"

typedef enum XrAndroidThreadTypeKHR {
    XR_ANDROID_THREAD_TYPE_APPLICATION_MAIN_KHR = 1,
    XR_ANDROID_THREAD_TYPE_APPLICATION_WORKER_KHR = 2,
    XR_ANDROID_THREAD_TYPE_RENDERER_MAIN_KHR = 3,
    XR_ANDROID_THREAD_TYPE_RENDERER_WORKER_KHR = 4,
    XR_ANDROID_THREAD_TYPE_MAX_ENUM_KHR = 0x7FFFFFFF
} XrAndroidThreadTypeKHR;
typedef XrResult (XRAPI_PTR *PFN_xrSetAndroidApplicationThreadKHR)(XrSession session, XrAndroidThreadTypeKHR threadType, uint32_t threadId);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrSetAndroidApplicationThreadKHR(
    XrSession                                   session,
    XrAndroidThreadTypeKHR                      threadType,
    uint32_t                                    threadId);
#endif
#endif /* XR_USE_PLATFORM_ANDROID */

#ifdef XR_USE_PLATFORM_ANDROID

#define XR_KHR_android_surface_swapchain 1
#define XR_KHR_android_surface_swapchain_SPEC_VERSION 4
#define XR_KHR_ANDROID_SURFACE_SWAPCHAIN_EXTENSION_NAME "XR_KHR_android_surface_swapchain"
typedef XrResult (XRAPI_PTR *PFN_xrCreateSwapchainAndroidSurfaceKHR)(XrSession session, const XrSwapchainCreateInfo* info, XrSwapchain* swapchain, jobject* surface);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrCreateSwapchainAndroidSurfaceKHR(
    XrSession                                   session,
    const XrSwapchainCreateInfo*                info,
    XrSwapchain*                                swapchain,
    jobject*                                    surface);
#endif
#endif /* XR_USE_PLATFORM_ANDROID */

#ifdef XR_USE_PLATFORM_ANDROID

#define XR_KHR_android_create_instance 1
#define XR_KHR_android_create_instance_SPEC_VERSION 3
#define XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME "XR_KHR_android_create_instance"
typedef struct XrInstanceCreateInfoAndroidKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    void* XR_MAY_ALIAS          applicationVM;
    void* XR_MAY_ALIAS          applicationActivity;
} XrInstanceCreateInfoAndroidKHR;

#define XR_PICO_android_swapchain_ext_enable 1
#define XR_PICO_android_swapchain_ext_enable_SPEC_VERSION 1
#define XR_PICO_ANDROID_SWAPCHAIN_EXT_ENABLE_EXTENSION_NAME "XR_PICO_android_swapchain_ext_enable"
typedef struct XrSwapchainCreateInfoAndroidEXT {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    imageNumExt;
    uint32_t*                   imagesExt;
} XrSwapchainCreateInfoAndroidEXT;

typedef enum XrFoveationLevel{
    XR_FOVEATION_LEVEL_NONE = -1,
    XR_FOVEATION_LEVEL_LOW = 0,
    XR_FOVEATION_LEVEL_MID = 1,
    XR_FOVEATION_LEVEL_HIGH = 2,
    XR_FOVEATION_LEVEL_TOP_HIGH = 3
}XrFoveationLevel;

typedef enum XrFoveationType{
    XR_FOVEATION_LEVEL = 0,
    XR_FOVEATION_PARAMETERS = 1
}XrFoveationType;
typedef struct XrFoveationParametersEXT{
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    int                         textureIdCount;
    int*                        textureId;
    int*                        previousId;
    float                       focalPointX;
    float                       focalPointY;
    XrFoveationType             ffrType;
    float                       foveationGainX;
    float                       foveationGainY;
    float                       foveationArea;
    float                       foveationMinimum;
    XrFoveationLevel            level;
    int                         frameOffsetCount;
    float*                      frameOffset;
}XrFoveationParametersEXT;

#define XR_PICO_ipd  1
#define XR_PICO_ipd_SPEC_VERSION 1
#define XR_PICO_IPD_EXTENSION_NAME "XR_PICO_ipd"

typedef XrResult (XRAPI_PTR *PFN_xrSetIPDPICO)(XrSession session, float distance);
typedef XrResult (XRAPI_PTR *PFN_xrGetIPDPICO)(XrSession session, float* ipd);
typedef XrResult (XRAPI_PTR *PFN_xrSetTrackingIPDEnabledPICO)(XrSession session, bool enable);
typedef XrResult (XRAPI_PTR *PFN_xrGetTrackingIPDEnabledPICO)(XrSession session, bool* enable);
typedef XrResult (XRAPI_PTR *PFN_xrGetEyeTrackingAutoIPDPICO)(XrSession session, float* autoIPD);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrSetIPDPICO(XrSession session, float distance);
XRAPI_ATTR XrResult XRAPI_CALL xrGetIPDPICO(XrSession session, float* ipd);
XRAPI_ATTR XrResult XRAPI_CALL xrSetTrackingIPDEnabledPICO(XrSession session, bool enable);
XRAPI_ATTR XrResult XRAPI_CALL xrGetTrackingIPDEnabledPICO(XrSession session, bool* enable);
XRAPI_ATTR XrResult XRAPI_CALL xrGetEyeTrackingAutoIPDPICO(XrSession session, float* autoIPD);
#endif

#define XR_PICO_stencilmesh  1
#define XR_PICO_stencilmesh_SPEC_VERSION 1
#define XR_PICO_STENCILMESH_EXTENSION_NAME "XR_PICO_stencilmesh"

typedef XrResult (XRAPI_PTR *PFN_xrGetStencilmeshPICO)(
  XrSession                        session,
  int                              eye,
  int                              *vertsCount,
  int                              *indexCount,
  float                            **localVerts,
  unsigned int                     **localIndex
);

#ifndef XR_NO_PROTOTYPES

XRAPI_ATTR XrResult XRAPI_CALL xrGetStencilmeshPICO(
  XrSession                        session,
  int                              eye,
  int                              *vertsCount,
  int                              *indexCount,
  float                            **localVerts,
  unsigned int                     **localIndex
);
#endif

#define XR_PICO_view_frustum_ext 1
#define XR_PICO_view_frustum_ext_SPEC_VERSION 1
#define XR_PICO_VIEW_FRUSTUM_EXT_EXTENSION_NAME "XR_PICO_view_frustum_ext"

struct XrViewFrustum
{
    float                     left;//!< Left Plane of Frustum
    float                     right;//!< Right Plane of Frustum
    float                     top;//!< Top Plane of Frustum
    float                     bottom;//!< Bottom Plane of Frustum
    float                     near;//!< Near Plane of Frustum
    float                     far;//!< Far Plane of Frustum (Arbitrary)
    XrPosef                   frustumPose;
};

typedef XrResult (XRAPI_PTR *PFN_xrGetFrustumParametersPICO)(
        XrSession                              session,
        struct XrViewFrustum                   *pLeftFrustum,
        struct XrViewFrustum                   *pRightFrustum);

typedef XrResult (XRAPI_PTR *PFN_xrSetFrustumParametersPICO)(
        XrSession                              session,
        struct XrViewFrustum                   *pLeftFrustum,
        struct XrViewFrustum                   *pRightFrustum);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetFrustumParametersPICO(
        XrSession                              session,
        struct XrViewFrustum                   *pLeftFrustum,
        struct XrViewFrustum                   *pRightFrustum);

XRAPI_ATTR XrResult XRAPI_CALL xrSetFrustumParametersPICO(
        XrSession                              session,
        struct XrViewFrustum                   *pLeftFrustum,
        struct XrViewFrustum                   *pRightFrustum);

#endif

#define XR_PICO_configs_ext 1
#define XR_PICO_configs_ext_SPEC_VERSION 1
#define XR_PICO_CONFIGS_EXT_EXTENSION_NAME "XR_PICO_configs_ext"
enum ConfigsEXT
{
    RENDER_TEXTURE_WIDTH = 0,
    RENDER_TEXTURE_HEIGHT,
    SHOW_FPS,
    RUNTIME_LOG_LEVEL,
    PXRPLUGIN_LOG_LEVEL,
    UNITY_LOG_LEVEL,
    UNREAL_LOG_LEVEL,
    NATIVE_LOG_LEVEL,
    TARGET_FRAME_RATE,
    NECK_MODEL_X,
    NECK_MODEL_Y,
    NECK_MODEL_Z,
    DISPLAY_REFRESH_RATE,
    ENABLE_6DOF,
    CONTROLLER_TYPE
};

enum Platform
{
    UNITY = 0,
    UNREAL,
    NATIVE
};
enum ConfigsSetEXT
{
    UNREAL_VERSION = 0,
    TRACKING_ORIGIN,
    UNREAL_OPENGL_NOERROR,
    ENABLE_SIX_DOF,
    PRESENTATION_FLAG,
    ENABLE_CPT,
    PLATFORM
};
enum TrackingOrigin
{
    EYELEVEL = 0,
    FLOORLEVEL,
    STAGELEVEL
};
struct ConfigsSetPICO
{
    char*     engineVersion;
    int       trackingOrigin;
    bool      noErrorFlag;
    bool      enableSixDof;
    bool      presentationFlag;
    int       platform;
};
typedef XrResult (XRAPI_PTR *PFN_xrGetConfigPICO)(
        XrSession                              session,
        enum ConfigsEXT                        configIndex,
        float *                                configData);

typedef XrResult (XRAPI_PTR *PFN_xrGetConfigsPICO)(
        XrSession                              session,
        int  *                                 configCount,
        float **                               configArray);
typedef XrResult (XRAPI_PTR *PFN_xrSetConfigPICO) (
        XrSession                             session,
        enum ConfigsSetEXT                    configIndex,
        char *                                configData);
typedef XrResult (XRAPI_PTR *PFN_xrSetConfigsPICO) (
        XrSession                              session,
        struct ConfigsSetPICO *                configsData);
typedef XrResult (XRAPI_PTR *PFN_xrGetFoveationConfigPICO)(
        XrSession                              session,
        enum XrFoveationLevel                  level,
        float *                                gainX,
        float *                                gainY,
        float *                                area,
        float *                                minimum);
#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetConfigPICO(
        XrSession                              session,
        enum ConfigsEXT                        configIndex,
        float *                                configData);

XRAPI_ATTR XrResult XRAPI_CALL xrGetConfigsPICO(
        XrSession                              session,
        int  *                                 configCount,
        float **                               configArray);
XRAPI_ATTR XrResult XRAPI_CALL xrSetConfigPICO (
        XrSession                             session,
        enum ConfigsSetEXT                    configIndex,
        char *                                configData);
XRAPI_ATTR XrResult XRAPI_CALL xrSetConfigsPICO (
        XrSession                              session,
        struct ConfigsSetPICO *                configsData);
XRAPI_ATTR XrResult XRAPI_CALL  xrGetFoveationConfigPICO(
        XrSession                              session,
        enum XrFoveationLevel                  level,
        float *                                gainX,
        float *                                gainY,
        float *                                area,
        float *                                minimum);
#endif

#define XR_PICO_reset_sensor  1
#define XR_PICO_reset_sensor_SPEC_VERSION 1
#define XR_PICO_RESET_SENSOR_EXTENSION_NAME "XR_PICO_reset_sensor"

typedef enum XrResetSensorOption {
    XR_RESET_POSITION = 0,
    XR_RESET_ORIENTATION = 1,
    XR_RESET_ORIENTATION_Y_ONLY= 2,
    XR_RESET_ALL
} XrResetSensorOption;

typedef XrResult (XRAPI_PTR *PFN_xrResetSensorPICO)(XrSession session, XrResetSensorOption option);

#ifndef XR_NO_PROTOTYPES

XRAPI_ATTR XrResult XRAPI_CALL xrResetSensorPICO(
        XrSession                                   session,
        XrResetSensorOption                         option);

#endif

#define XR_PICO_boundary_ext  1
#define XR_PICO_boundary_ext_SPEC_VERSION 1
#define XR_PICO_BOUNDARY_EXT_EXTENSION_NAME "XR_PICO_boundary_ext"

typedef enum{
    XR_SET_SEETHROUGH_VISIBLE = 0,
    XR_SET_GUARDIANSYSTEM_DISABLE,
    XR_RESUME_GUARDIANSYSTEM_FOR_STS,
    XR_PAUSE_GUARDIANSYSTEM_FOR_STS,
    XR_SHUTDOWN_SDK_GUARDIANSYSTEM,
    XR_GET_CAMERA_DATA_EXT,
    XR_START_SDK_BOUNDARY,
    XR_SET_CONTROLLER_POSITION,  //unused
    XR_START_CAMERA_PREVIEW,
    XR_GET_ROOM_MODE_STATE,
    XR_DISABLE_BOUNDARY,
    XR_SET_MONO_MODE,
    XR_GET_BOUNDARY_CONFIGURED,
    XR_GET_BOUNDARY_ENABLED,
    XR_SET_BOUNDARY_VISIBLE,
    XR_SET_SEETHROUGH_BACKGROUND,
    XR_GET_BOUNDARY_VISIBLE,
} xrFuncitonName;

typedef XrResult (XRAPI_PTR *PFN_xrInvokeFunctionsPICO)(
        XrSession                                 session,
        xrFuncitonName                            name,
        void *                                    input,
        unsigned int                              size_in,
        void **                                   output,
        unsigned int                              size_out);

typedef XrResult (XRAPI_PTR *PFN_xrSetControllerPositionPICO)(
        XrSession                                 session,
        float                                     x,
        float                                     y,
        float                                     z,
        float                                     w,
        float                                     px,
        float                                     py,
        float                                     pz,
        int                                       hand,
        bool                                      valid,
        int                                       keyEvent);

typedef XrResult (XRAPI_PTR *PFN_xrBoundaryTestNodePICO)(
        XrSession                                 session,
        int                                       node,
        bool                                      isPlayArea,
        bool *                                    pisTriggering,
        float *                                   pclosestDistance,
        float *                                   ppx,
        float *                                   ppy,
        float *                                   ppz,
        float *                                   pnx,
        float *                                   pny,
        float *                                   pnz,
        int *                                     ret);

typedef XrResult (XRAPI_PTR *PFN_xrBoundaryTestPointPICO)(
        XrSession                                 session,
        float                                     x,
        float                                     y,
        float                                     z,
        bool                                      isPlayArea,
        bool *                                    pisTriggering,
        float *                                   pclosestDistance,
        float *                                   ppx,
        float *                                   ppy,
        float *                                   ppz,
        float *                                   pnx,
        float *                                   pny,
        float *                                   pnz,
        int *                                     ret);

typedef XrResult (XRAPI_PTR *PFN_xrGetBoundaryGeometryPICO)(
        XrSession                                 session,
        float **                                  outPointsFloat,
        bool                                      isPlayArea,
        int *                                     ret);

typedef XrResult (XRAPI_PTR *PFN_xrGetBoundaryDimensionsPICO)(
        XrSession                                 session,
        float *                                   x,
        float *                                   y,
        float *                                   z,
        bool                                      isPlayArea,
        int *                                     ret);

typedef XrResult (XRAPI_PTR *PFN_xrGetSeeThroughDataPICO)(
        XrSession                                 session,
        uint8_t *                                 leftEye,
        uint8_t *                                 rightEye,
        uint32_t *                                width,
        uint32_t *                                height,
        uint32_t *                                exposure,
        int64_t *                                 start_of_exposure_ts,
        int *                                     ret);

#ifndef XR_NO_PROTOTYPES

XRAPI_ATTR XrResult XRAPI_CALL xrInvokeFunctionsPICO(
        XrSession                                   session,
        xrFuncitonName                              name,
        void *                                      input,
        unsigned int                                size_in,
        void **                                     output,
        unsigned int                                size_out);

XRAPI_ATTR XrResult XRAPI_CALL xrSetControllerPositionPICO(
        XrSession                                   session,
        float                                       x,
        float                                       y,
        float                                       z,
        float                                       w,
        float                                       px,
        float                                       py,
        float                                       pz,
        int                                         hand,
        bool                                        valid,
        int                                         keyEvent);

XRAPI_ATTR XrResult XRAPI_CALL xrBoundaryTestNodePICO(
        XrSession                                   session,
        int                                         node,
        bool                                        isPlayArea,
        bool *                                      pisTriggering,
        float *                                     pclosestDistance,
        float *                                     ppx,
        float *                                     ppy,
        float *                                     ppz,
        float *                                     pnx,
        float *                                     pny,
        float *                                     pnz,
        int *                                       ret);

XRAPI_ATTR XrResult XRAPI_CALL xrBoundaryTestPointPICO(
        XrSession                                   session,
        float                                       x,
        float                                       y,
        float                                       z,
        bool                                        isPlayArea,
        bool *                                      pisTriggering,
        float *                                     pclosestDistance,
        float *                                     ppx,
        float *                                     ppy,
        float *                                     ppz,
        float *                                     pnx,
        float *                                     pny,
        float *                                     pnz,
        int *                                       ret);

XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryGeometryPICO(
        XrSession                                   session,
        float **                                    outPointsFloat,
        bool                                        isPlayArea,
        int *                                       ret);

XRAPI_ATTR XrResult XRAPI_CALL xrGetBoundaryDimensionsPICO(
        XrSession                                   session,
        float *                                     x,
        float *                                     y,
        float *                                     z,
        bool                                        isPlayArea,
        int *                                       ret);

XRAPI_ATTR XrResult XRAPI_CALL xrGetSeeThroughDataPICO(
        XrSession                                   session,
        uint8_t *                                   leftEye,
        uint8_t *                                   rightEye,
        uint32_t *                                  width,
        uint32_t *                                  height,
        uint32_t *                                  exposure,
        int64_t *                                   start_of_exposure_ts,
        int *                                       ret);


#endif

//performance settings
#define XR_PICO_performance_settings 1
#define XR_PICO_performance_settings_SPEC_VERSION 1
#define XR_PICO_PERFORMANCE_SETTINGS_EXTENSION_NAME "XR_PICO_performance_settings"

typedef XrResult (XRAPI_PTR *PFN_xrSetPerformanceLevelPICO)(
    XrSession                                   session,
    XrPerfSettingsDomainEXT                     domain,
    int                                         level);

typedef XrResult (XRAPI_PTR *PFN_xrGetPerformanceLevelPICO)(
    XrSession                                   session,
    XrPerfSettingsDomainEXT                     domain,
    int *                                       level);

#ifndef XR_NO_PROTOTYPES

XRAPI_ATTR XrResult XRAPI_CALL xrSetPerformanceLevelPICO(
        XrSession                               session,
        XrPerfSettingsDomainEXT                 domain,
        int                                     level);

XRAPI_ATTR XrResult XRAPI_CALL xrGetPerformanceLevelPICO(
        XrSession                               session,
        XrPerfSettingsDomainEXT                 domain,
        int *                                   level);
#endif

#define XR_PICO_android_create_instance_ext_enable 1
#define XR_PICO_android_create_instance_ext_enable_SPEC_VERSION 1
#define XR_PICO_ANDROID_CREATE_INSTANCE_EXT_ENABLE_EXTENSION_NAME "XR_PICO_android_create_instance_ext_enable"
typedef struct XrInstanceCreateInfoAndroidPICOEXT {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    jobject*                    surfaceView;
} XrInstanceCreateInfoAndroidPICOEXT;

#define XR_PICO_view_state_ext_enable 1
#define XR_PICO_view_state_ext_enable_SPEC_VERSION 1
#define XR_PICO_VIEW_STATE_EXT_ENABLE_EXTENSION_NAME "XR_PICO_view_state_ext_enable"
typedef struct XrViewStatePICOEXT {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrPosef                     headpose;
    int32_t             poseStatus;             //!< Bit field (sxrTrackingMode) indicating pose status
    uint64_t            poseTimeStampNs;        //!< Time stamp in which the head pose was generated (nanoseconds)
    uint64_t            poseFetchTimeNs;        //!< Time stamp when this pose was retrieved
    uint64_t            expectedDisplayTimeNs;  //!< Expected time when this pose should be on screen (nanoseconds)
    int                 gsIndex;
} XrViewStatePICOEXT;

#define XR_PICO_frame_end_info_ext  1
#define XR_PICO_frame_end_info_ext_SPEC_VERSION 1
#define XR_PICO_FRAME_END_INFO_EXT_EXTENSION_NAME "XR_PICO_frame_end_info_ext"
typedef struct XrFrameEndInfoEXT
{
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    enableFoveation;
    XrFoveationParametersEXT    foveationParametersEXT;
    uint32_t            useHeadposeExt;
    XrPosef             headpose;
    int32_t             poseStatus;             //!< Bit field (sxrTrackingMode) indicating pose status
    uint64_t            poseTimeStampNs;        //!< Time stamp in which the head pose was generated (nanoseconds)
    uint64_t            poseFetchTimeNs;        //!< Time stamp when this pose was retrieved
    uint64_t            expectedDisplayTimeNs;  //!< Expected time when this pose should be on screen (nanoseconds)
    int                 gsIndex;
}XrFrameEndInfoEXT;

#define XR_PICO_session_begin_info_ext_enable 1
#define XR_PICO_session_begin_info_ext_enable_SPEC_VERSION 1
#define XR_PICO_SESSION_BEGIN_INFO_EXT_ENABLE_EXTENSION_NAME "XR_PICO_session_begin_info_ext_enable"
enum XrColorSpace{    
	colorSpaceLinear = 0,    
	colorSpaceSRGB = 1,
};
typedef struct XrSessionBeginInfoEXT {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    enableSinglePass;
    enum XrColorSpace                colorSpace;
} XrSessionBeginInfoEXT;

#define XR_PICO_android_controller_function_ext_enable 1
#define XR_PICO_android_controller_function_ext_enable_SPEC_VERSION 1
#define XR_PICO_ANDROID_CONTROLLER_FUNCTION_EXT_ENABLE_EXTENSION_NAME "XR_PICO_android_controller_function_ext_enable"

typedef XrResult (XRAPI_PTR *PFN_xrGetControllerConnectionStatePico)(XrInstance instance,uint8_t controllerhandle,uint8_t* status);
typedef XrResult (XRAPI_PTR *PFN_xrSetEngineVersionPico)(XrInstance instance,const char* version);
typedef XrResult (XRAPI_PTR *PFN_xrSetControllerEventCallbackPico)(XrInstance instance,bool enable_controller_callback);
typedef XrResult (XRAPI_PTR *PFN_xrResetControllerSensorPico)(XrInstance instance,int controllerHandle);
typedef XrResult (XRAPI_PTR *PFN_xrGetConnectDeviceMacPico)(XrInstance instance,char* mac);
typedef XrResult (XRAPI_PTR *PFN_xrStartCVControllerThreadPico)(XrInstance instance,int headSensorState, int handSensorState);
typedef XrResult (XRAPI_PTR *PFN_xrStopCVControllerThreadPico)(XrInstance instance,int headSensorState, int handSensorState);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerAngularVelocityStatePico)(XrInstance instance,int controllerHandle,float* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerAccelerationStatePico)(XrInstance instance,int controllerHandle,float *data);
typedef XrResult (XRAPI_PTR *PFN_xrSetMainControllerHandlePico)(XrInstance instance,int controllerHandle);
typedef XrResult (XRAPI_PTR *PFN_xrGetMainControllerHandlePico)(XrInstance instance,int* controllerHandle);
typedef XrResult (XRAPI_PTR *PFN_xrResetHeadSensorForControllerPico)(XrInstance instance);
typedef XrResult (XRAPI_PTR *PFN_xrSetIsEnbleHomeKeyPico)(XrInstance instance,bool isEnable);
typedef XrResult (XRAPI_PTR *PFN_xrGetHeadSensorDataPico)(XrInstance instance,float* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerSensorDataPredictPico)(XrInstance instance,int controllerHandle, float headSensorData[], float predictTime,float* data);
typedef XrResult (XRAPI_PTR *PFN_xrVibrateControllerPico)(XrInstance instance,float strength ,int time,int controllerHandle);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerLinearVelocityStatePico)(XrInstance instance,int controllerHandle,float* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerSensorDataPico)(XrInstance instance,int controllerHandle, float headSensorData[],float* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerFixedSensorStatePico)(XrInstance instance,int controllerHandle,float* data);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerTouchValuePico)(XrInstance instance,int controllerSerialNum,int length,int* value);
typedef XrResult (XRAPI_PTR *PFN_xrGetControllerGripValuePico)(XrInstance instance,int controllerSerialNum,int *gripvalue);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerConnectionStatePico(
    XrInstance instance,uint8_t controllerhandle,uint8_t* status);
XRAPI_ATTR XrResult XRAPI_CALL xrSetEngineVersionPico(XrInstance instance,const char* version);
XRAPI_ATTR XrResult XRAPI_CALL xrSetControllerEventCallbackPico(XrInstance instance,bool enable_controller_callback);
XRAPI_ATTR XrResult XRAPI_CALL xrResetControllerSensorPico(XrInstance instance,int controllerHandle);
XRAPI_ATTR XrResult XRAPI_CALL xrGetConnectDeviceMacPico(XrInstance instance,char* mac);
XRAPI_ATTR XrResult XRAPI_CALL xrStartCVControllerThreadPico(XrInstance instance,int headSensorState, int handSensorState);
XRAPI_ATTR XrResult XRAPI_CALL xrStopCVControllerThreadPico(XrInstance instance,int headSensorState, int handSensorState);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerAngularVelocityStatePico(XrInstance instance,int controllerHandle,float* data);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerAccelerationStatePico(XrInstance instance,int controllerHandle,float *data);
XRAPI_ATTR XrResult XRAPI_CALL xrSetMainControllerHandlePico(XrInstance instance,int controllerHandle);
XRAPI_ATTR XrResult XRAPI_CALL xrGetMainControllerHandlePico(XrInstance instance,int* controllerHandle);
XRAPI_ATTR XrResult XRAPI_CALL xrResetHeadSensorForControllerPico(XrInstance instance);
XRAPI_ATTR XrResult XRAPI_CALL xrSetIsEnbleHomeKeyPico(XrInstance instance,bool isEnable);
XRAPI_ATTR XrResult XRAPI_CALL xrGetHeadSensorDataPico(XrInstance instance,float* data);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerSensorDataPredictPico(XrInstance instance,int controllerHandle, float headSensorData[], float predictTime,float* data);
XRAPI_ATTR XrResult XRAPI_CALL xrVibrateControllerPico(XrInstance instance,float strength ,int time,int controllerHandle);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerLinearVelocityStatePico(XrInstance instance,int controllerHandle,float* data);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerSensorDataPico(XrInstance instance,int controllerHandle, float headSensorData[],float* data);
XRAPI_ATTR XrResult XRAPI_CALL xrGetControllerFixedSensorStatePico(XrInstance instance,int controllerHandle,float* data);
#endif

#endif /* XR_USE_PLATFORM_ANDROID */

#ifdef XR_USE_GRAPHICS_API_VULKAN

#define XR_KHR_vulkan_swapchain_format_list 1
#define XR_KHR_vulkan_swapchain_format_list_SPEC_VERSION 3
#define XR_KHR_VULKAN_SWAPCHAIN_FORMAT_LIST_EXTENSION_NAME "XR_KHR_vulkan_swapchain_format_list"
typedef struct XrVulkanSwapchainFormatListCreateInfoKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    uint32_t                    viewFormatCount;
    const VkFormat*             viewFormats;
} XrVulkanSwapchainFormatListCreateInfoKHR;

#endif /* XR_USE_GRAPHICS_API_VULKAN */

#ifdef XR_USE_GRAPHICS_API_OPENGL

#define XR_KHR_opengl_enable 1
#define XR_KHR_opengl_enable_SPEC_VERSION 9
#define XR_KHR_OPENGL_ENABLE_EXTENSION_NAME "XR_KHR_opengl_enable"
#ifdef XR_USE_PLATFORM_WIN32
typedef struct XrGraphicsBindingOpenGLWin32KHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    HDC                         hDC;
    HGLRC                       hGLRC;
} XrGraphicsBindingOpenGLWin32KHR;
#endif // XR_USE_PLATFORM_WIN32

#ifdef XR_USE_PLATFORM_XLIB
typedef struct XrGraphicsBindingOpenGLXlibKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    Display*                    xDisplay;
    uint32_t                    visualid;
    GLXFBConfig                 glxFBConfig;
    GLXDrawable                 glxDrawable;
    GLXContext                  glxContext;
} XrGraphicsBindingOpenGLXlibKHR;
#endif // XR_USE_PLATFORM_XLIB

#ifdef XR_USE_PLATFORM_XCB
typedef struct XrGraphicsBindingOpenGLXcbKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    xcb_connection_t*           connection;
    uint32_t                    screenNumber;
    xcb_glx_fbconfig_t          fbconfigid;
    xcb_visualid_t              visualid;
    xcb_glx_drawable_t          glxDrawable;
    xcb_glx_context_t           glxContext;
} XrGraphicsBindingOpenGLXcbKHR;
#endif // XR_USE_PLATFORM_XCB

#ifdef XR_USE_PLATFORM_WAYLAND
typedef struct XrGraphicsBindingOpenGLWaylandKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    struct wl_display*          display;
} XrGraphicsBindingOpenGLWaylandKHR;
#endif // XR_USE_PLATFORM_WAYLAND

typedef struct XrSwapchainImageOpenGLKHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    uint32_t              image;
} XrSwapchainImageOpenGLKHR;

typedef struct XrGraphicsRequirementsOpenGLKHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrVersion             minApiVersionSupported;
    XrVersion             maxApiVersionSupported;
} XrGraphicsRequirementsOpenGLKHR;

typedef XrResult (XRAPI_PTR *PFN_xrGetOpenGLGraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLKHR* graphicsRequirements);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetOpenGLGraphicsRequirementsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    XrGraphicsRequirementsOpenGLKHR*            graphicsRequirements);
#endif
#endif /* XR_USE_GRAPHICS_API_OPENGL */

#ifdef XR_USE_GRAPHICS_API_OPENGL_ES

#define XR_KHR_opengl_es_enable 1
#define XR_KHR_opengl_es_enable_SPEC_VERSION 7
#define XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME "XR_KHR_opengl_es_enable"
#ifdef XR_USE_PLATFORM_ANDROID
typedef struct XrGraphicsBindingOpenGLESAndroidKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    EGLDisplay                  display;
    EGLConfig                   config;
    EGLContext                  context;
} XrGraphicsBindingOpenGLESAndroidKHR;
#endif // XR_USE_PLATFORM_ANDROID

typedef struct XrSwapchainImageOpenGLESKHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    uint32_t              image;
} XrSwapchainImageOpenGLESKHR;

typedef struct XrGraphicsRequirementsOpenGLESKHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrVersion             minApiVersionSupported;
    XrVersion             maxApiVersionSupported;
} XrGraphicsRequirementsOpenGLESKHR;

typedef XrResult (XRAPI_PTR *PFN_xrGetOpenGLESGraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsOpenGLESKHR* graphicsRequirements);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetOpenGLESGraphicsRequirementsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    XrGraphicsRequirementsOpenGLESKHR*          graphicsRequirements);
#endif
#endif /* XR_USE_GRAPHICS_API_OPENGL_ES */

#ifdef XR_USE_GRAPHICS_API_VULKAN

#define XR_KHR_vulkan_enable 1
#define XR_KHR_vulkan_enable_SPEC_VERSION 7
#define XR_KHR_VULKAN_ENABLE_EXTENSION_NAME "XR_KHR_vulkan_enable"
typedef struct XrGraphicsBindingVulkanKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    VkInstance                  instance;
    VkPhysicalDevice            physicalDevice;
    VkDevice                    device;
    uint32_t                    queueFamilyIndex;
    uint32_t                    queueIndex;
} XrGraphicsBindingVulkanKHR;

typedef struct XrSwapchainImageVulkanKHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    VkImage               image;
} XrSwapchainImageVulkanKHR;

typedef struct XrGraphicsRequirementsVulkanKHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    XrVersion             minApiVersionSupported;
    XrVersion             maxApiVersionSupported;
} XrGraphicsRequirementsVulkanKHR;

typedef XrResult (XRAPI_PTR *PFN_xrGetVulkanInstanceExtensionsKHR)(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
typedef XrResult (XRAPI_PTR *PFN_xrGetVulkanDeviceExtensionsKHR)(XrInstance instance, XrSystemId systemId, uint32_t bufferCapacityInput, uint32_t* bufferCountOutput, char* buffer);
typedef XrResult (XRAPI_PTR *PFN_xrGetVulkanGraphicsDeviceKHR)(XrInstance instance, XrSystemId systemId, VkInstance vkInstance, VkPhysicalDevice* vkPhysicalDevice);
typedef XrResult (XRAPI_PTR *PFN_xrGetVulkanGraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetVulkanInstanceExtensionsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    uint32_t                                    bufferCapacityInput,
    uint32_t*                                   bufferCountOutput,
    char*                                       buffer);

XRAPI_ATTR XrResult XRAPI_CALL xrGetVulkanDeviceExtensionsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    uint32_t                                    bufferCapacityInput,
    uint32_t*                                   bufferCountOutput,
    char*                                       buffer);

XRAPI_ATTR XrResult XRAPI_CALL xrGetVulkanGraphicsDeviceKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    VkInstance                                  vkInstance,
    VkPhysicalDevice*                           vkPhysicalDevice);

XRAPI_ATTR XrResult XRAPI_CALL xrGetVulkanGraphicsRequirementsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    XrGraphicsRequirementsVulkanKHR*            graphicsRequirements);
#endif
#endif /* XR_USE_GRAPHICS_API_VULKAN */

#ifdef XR_USE_GRAPHICS_API_D3D11

#define XR_KHR_D3D11_enable 1
#define XR_KHR_D3D11_enable_SPEC_VERSION  5
#define XR_KHR_D3D11_ENABLE_EXTENSION_NAME "XR_KHR_D3D11_enable"
typedef struct XrGraphicsBindingD3D11KHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    ID3D11Device*               device;
} XrGraphicsBindingD3D11KHR;

typedef struct XrSwapchainImageD3D11KHR {
     XrStructureType      type;
    void* XR_MAY_ALIAS    next;
    ID3D11Texture2D*      texture;
} XrSwapchainImageD3D11KHR;

typedef struct XrGraphicsRequirementsD3D11KHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    LUID                  adapterLuid;
    D3D_FEATURE_LEVEL     minFeatureLevel;
} XrGraphicsRequirementsD3D11KHR;

typedef XrResult (XRAPI_PTR *PFN_xrGetD3D11GraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D11KHR* graphicsRequirements);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetD3D11GraphicsRequirementsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    XrGraphicsRequirementsD3D11KHR*             graphicsRequirements);
#endif
#endif /* XR_USE_GRAPHICS_API_D3D11 */

#ifdef XR_USE_GRAPHICS_API_D3D12

#define XR_KHR_D3D12_enable 1
#define XR_KHR_D3D12_enable_SPEC_VERSION  7
#define XR_KHR_D3D12_ENABLE_EXTENSION_NAME "XR_KHR_D3D12_enable"
typedef struct XrGraphicsBindingD3D12KHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    ID3D12Device*               device;
    ID3D12CommandQueue*         queue;
} XrGraphicsBindingD3D12KHR;

typedef struct XrSwapchainImageD3D12KHR {
     XrStructureType      type;
    void* XR_MAY_ALIAS    next;
    ID3D12Resource*       texture;
} XrSwapchainImageD3D12KHR;

typedef struct XrGraphicsRequirementsD3D12KHR {
    XrStructureType       type;
    void* XR_MAY_ALIAS    next;
    LUID                  adapterLuid;
    D3D_FEATURE_LEVEL     minFeatureLevel;
} XrGraphicsRequirementsD3D12KHR;

typedef XrResult (XRAPI_PTR *PFN_xrGetD3D12GraphicsRequirementsKHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsD3D12KHR* graphicsRequirements);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrGetD3D12GraphicsRequirementsKHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    XrGraphicsRequirementsD3D12KHR*             graphicsRequirements);
#endif
#endif /* XR_USE_GRAPHICS_API_D3D12 */

#ifdef XR_USE_PLATFORM_WIN32

#define XR_KHR_win32_convert_performance_counter_time 1
#define XR_KHR_win32_convert_performance_counter_time_SPEC_VERSION 1
#define XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME "XR_KHR_win32_convert_performance_counter_time"
typedef XrResult (XRAPI_PTR *PFN_xrConvertWin32PerformanceCounterToTimeKHR)(XrInstance instance, const LARGE_INTEGER* performanceCounter, XrTime* time);
typedef XrResult (XRAPI_PTR *PFN_xrConvertTimeToWin32PerformanceCounterKHR)(XrInstance instance, XrTime   time, LARGE_INTEGER* performanceCounter);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrConvertWin32PerformanceCounterToTimeKHR(
    XrInstance                                  instance,
    const LARGE_INTEGER*                        performanceCounter,
    XrTime*                                     time);

XRAPI_ATTR XrResult XRAPI_CALL xrConvertTimeToWin32PerformanceCounterKHR(
    XrInstance                                  instance,
    XrTime                                      time,
    LARGE_INTEGER*                              performanceCounter);
#endif
#endif /* XR_USE_PLATFORM_WIN32 */

#ifdef XR_USE_TIMESPEC

#define XR_KHR_convert_timespec_time 1
#define XR_KHR_convert_timespec_time_SPEC_VERSION 1
#define XR_KHR_CONVERT_TIMESPEC_TIME_EXTENSION_NAME "XR_KHR_convert_timespec_time"
typedef XrResult (XRAPI_PTR *PFN_xrConvertTimespecTimeToTimeKHR)(XrInstance instance, const struct timespec* timespecTime, XrTime* time);
typedef XrResult (XRAPI_PTR *PFN_xrConvertTimeToTimespecTimeKHR)(XrInstance instance, XrTime   time, struct timespec* timespecTime);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrConvertTimespecTimeToTimeKHR(
    XrInstance                                  instance,
    const struct timespec*                      timespecTime,
    XrTime*                                     time);

XRAPI_ATTR XrResult XRAPI_CALL xrConvertTimeToTimespecTimeKHR(
    XrInstance                                  instance,
    XrTime                                      time,
    struct timespec*                            timespecTime);
#endif
#endif /* XR_USE_TIMESPEC */

#ifdef XR_USE_PLATFORM_ANDROID

#define XR_KHR_loader_init_android 1
#define XR_KHR_loader_init_android_SPEC_VERSION 1
#define XR_KHR_LOADER_INIT_ANDROID_EXTENSION_NAME "XR_KHR_loader_init_android"
typedef struct XrLoaderInitInfoAndroidKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    void* XR_MAY_ALIAS          applicationVM;
    void* XR_MAY_ALIAS          applicationContext;
} XrLoaderInitInfoAndroidKHR;

#endif /* XR_USE_PLATFORM_ANDROID */

#ifdef XR_USE_GRAPHICS_API_VULKAN

#define XR_KHR_vulkan_enable2 1
#define XR_KHR_vulkan_enable2_SPEC_VERSION 1
#define XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME "XR_KHR_vulkan_enable2"
typedef XrFlags64 XrVulkanInstanceCreateFlagsKHR;

// Flag bits for XrVulkanInstanceCreateFlagsKHR

typedef XrFlags64 XrVulkanDeviceCreateFlagsKHR;

// Flag bits for XrVulkanDeviceCreateFlagsKHR

typedef struct XrVulkanInstanceCreateInfoKHR {
    XrStructureType                   type;
    const void* XR_MAY_ALIAS          next;
    XrSystemId                        systemId;
    XrVulkanInstanceCreateFlagsKHR    createFlags;
    PFN_vkGetInstanceProcAddr         pfnGetInstanceProcAddr;
    const VkInstanceCreateInfo*       vulkanCreateInfo;
    const VkAllocationCallbacks*      vulkanAllocator;
} XrVulkanInstanceCreateInfoKHR;

typedef struct XrVulkanDeviceCreateInfoKHR {
    XrStructureType                 type;
    const void* XR_MAY_ALIAS        next;
    XrSystemId                      systemId;
    XrVulkanDeviceCreateFlagsKHR    createFlags;
    PFN_vkGetInstanceProcAddr       pfnGetInstanceProcAddr;
    VkPhysicalDevice                vulkanPhysicalDevice;
    const VkDeviceCreateInfo*       vulkanCreateInfo;
    const VkAllocationCallbacks*    vulkanAllocator;
} XrVulkanDeviceCreateInfoKHR;

typedef XrGraphicsBindingVulkanKHR XrGraphicsBindingVulkan2KHR;

typedef struct XrVulkanGraphicsDeviceGetInfoKHR {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    XrSystemId                  systemId;
    VkInstance                  vulkanInstance;
} XrVulkanGraphicsDeviceGetInfoKHR;

typedef XrSwapchainImageVulkanKHR XrSwapchainImageVulkan2KHR;

typedef XrGraphicsRequirementsVulkanKHR XrGraphicsRequirementsVulkan2KHR;

typedef XrResult (XRAPI_PTR *PFN_xrCreateVulkanInstanceKHR)(XrInstance                          instance, const XrVulkanInstanceCreateInfoKHR*createInfo, VkInstance*                         vulkanInstance, VkResult*                           vulkanResult);
typedef XrResult (XRAPI_PTR *PFN_xrCreateVulkanDeviceKHR)(XrInstance                          instance, const XrVulkanDeviceCreateInfoKHR*  createInfo, VkDevice*                           vulkanDevice, VkResult*                           vulkanResult);
typedef XrResult (XRAPI_PTR *PFN_xrGetVulkanGraphicsDevice2KHR)(XrInstance                              instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo, VkPhysicalDevice*                       vulkanPhysicalDevice);
typedef XrResult (XRAPI_PTR *PFN_xrGetVulkanGraphicsRequirements2KHR)(XrInstance instance, XrSystemId systemId, XrGraphicsRequirementsVulkanKHR* graphicsRequirements);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrCreateVulkanInstanceKHR(
    XrInstance                                  instance,
    const XrVulkanInstanceCreateInfoKHR*        createInfo,
    VkInstance*                                 vulkanInstance,
    VkResult*                                   vulkanResult);

XRAPI_ATTR XrResult XRAPI_CALL xrCreateVulkanDeviceKHR(
    XrInstance                                  instance,
    const XrVulkanDeviceCreateInfoKHR*          createInfo,
    VkDevice*                                   vulkanDevice,
    VkResult*                                   vulkanResult);

XRAPI_ATTR XrResult XRAPI_CALL xrGetVulkanGraphicsDevice2KHR(
    XrInstance                                  instance,
    const XrVulkanGraphicsDeviceGetInfoKHR*     getInfo,
    VkPhysicalDevice*                           vulkanPhysicalDevice);

XRAPI_ATTR XrResult XRAPI_CALL xrGetVulkanGraphicsRequirements2KHR(
    XrInstance                                  instance,
    XrSystemId                                  systemId,
    XrGraphicsRequirementsVulkanKHR*            graphicsRequirements);
#endif
#endif /* XR_USE_GRAPHICS_API_VULKAN */

#ifdef XR_USE_PLATFORM_EGL

#define XR_MNDX_egl_enable 1
#define XR_MNDX_egl_enable_SPEC_VERSION   1
#define XR_MNDX_EGL_ENABLE_EXTENSION_NAME "XR_MNDX_egl_enable"
typedef struct XrGraphicsBindingEGLMNDX {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    PFNEGLGETPROCADDRESSPROC    getProcAddress;
    EGLDisplay                  display;
    EGLConfig                   config;
    EGLContext                  context;
} XrGraphicsBindingEGLMNDX;

#endif /* XR_USE_PLATFORM_EGL */

#ifdef XR_USE_PLATFORM_WIN32

#define XR_MSFT_perception_anchor_interop 1
#define XR_MSFT_perception_anchor_interop_SPEC_VERSION 1
#define XR_MSFT_PERCEPTION_ANCHOR_INTEROP_EXTENSION_NAME "XR_MSFT_perception_anchor_interop"
typedef XrResult (XRAPI_PTR *PFN_xrCreateSpatialAnchorFromPerceptionAnchorMSFT)(XrSession session, IUnknown* perceptionAnchor, XrSpatialAnchorMSFT* anchor);
typedef XrResult (XRAPI_PTR *PFN_xrTryGetPerceptionAnchorFromSpatialAnchorMSFT)(XrSession session, XrSpatialAnchorMSFT anchor, IUnknown** perceptionAnchor);

#ifndef XR_NO_PROTOTYPES
XRAPI_ATTR XrResult XRAPI_CALL xrCreateSpatialAnchorFromPerceptionAnchorMSFT(
    XrSession                                   session,
    IUnknown*                                   perceptionAnchor,
    XrSpatialAnchorMSFT*                        anchor);

XRAPI_ATTR XrResult XRAPI_CALL xrTryGetPerceptionAnchorFromSpatialAnchorMSFT(
    XrSession                                   session,
    XrSpatialAnchorMSFT                         anchor,
    IUnknown**                                  perceptionAnchor);
#endif
#endif /* XR_USE_PLATFORM_WIN32 */

#ifdef XR_USE_PLATFORM_WIN32

#define XR_MSFT_holographic_window_attachment 1
#define XR_MSFT_holographic_window_attachment_SPEC_VERSION 1
#define XR_MSFT_HOLOGRAPHIC_WINDOW_ATTACHMENT_EXTENSION_NAME "XR_MSFT_holographic_window_attachment"
#ifdef XR_USE_PLATFORM_WIN32
typedef struct XrHolographicWindowAttachmentMSFT {
    XrStructureType             type;
    const void* XR_MAY_ALIAS    next;
    IUnknown*                   holographicSpace;
    IUnknown*                   coreWindow;
} XrHolographicWindowAttachmentMSFT;
#endif // XR_USE_PLATFORM_WIN32

#endif /* XR_USE_PLATFORM_WIN32 */

#ifdef __cplusplus
}
#endif

#endif
