//
// Created by fcx@pingxingyun.com on 2019/11/22.
//

#ifndef CLOUDLARKVRMODULE_TYPES_H
#define CLOUDLARKVRMODULE_TYPES_H

#include <stdint.h>

#define LARK_XR_TRUE                    1
#define LARK_XR_FALSE                   0

#define LARK_XR_MAX_NAME_SIZE           256
#define LARK_XR_MAX_MACADDRESS_SIZE     256
#define LARK_XR_MAX_VERSION_SIZE        256
#define LARK_XR_MAX_PATH_SIZE           512

// GLOBAL
typedef int						        BOOL;

//
// 用户配置
//
typedef struct larkxrUserSetting_ {
    // 比特率。单位 kbps. 默认值为 30 * 1000 Kbps;
    int bitrateKbps;
    // steam vr 设置的初始房间高度
    float roomHight;
} larkxrUserSetting;
//

//
// 系统类型。VR 一体机头盔，或者手机等。
//
typedef enum larkxrSystemType_ {
    Larkxr_System_VR_HEADSET = 0,
    Larkxr_System_Mobile,
    Larkxr_System_Win32,
    Larkxr_System_Other,
}  larkxrSystemType;

//
// 系统平台类型，主要为不同的 VR 一体机。
//
typedef enum larkxrPlatFromType_ {
    Larkxr_Platform_HTC_FOCUS      = 0,
    Larkxr_Platform_HTC_FOCUS_PLUS = 1,
    Larkxr_Platform_PICO_NEO       = 2,
    Larkxr_Platform_PICO_NEO_2     = 3,
    Larkxr_Platform_PICO_NEO_3     = 4,
    Larkxr_Platform_PICO_G2        = 5,
    Larkxr_Platform_PICO_G2_4k     = 6,
    Larkxr_Platform_PICO_GOBLIN    = 7,
    Larkxr_Platform_DPVR_P1        = 8,
    Larkxr_Platform_Oculus_Quest   = 9,
    Larkxr_Platform_Oculus_Quest_2 = 10,
    Larkxr_Platform_GENERAL_UNITY_WIN32 = 900,
    Larkxr_Platform_GENERAL_WAVE   = 1000,
}  larkxrPlatFromType;

//
// 系统当前使用的 vr 头盔 sdk 类型
//
typedef enum larkxrSDKVersion_ {
    Larkxr_HEADSET_SDK_HTC_WAVE = 0,
    Larkxr_HEADSET_SDK_OCULUS,
    Larkxr_HEADSET_SDK_PICO,
    Larkxr_HEADSET_SDK_GOOGLE_CARDBOARD = 1000,
}  larkxrHeadSetSDKVersion;

//
// 系统基本信息。将在 sdk 最开始初始化。
//
typedef struct larkxrSystemInfo {
    BOOL                      initd;
    larkxrSystemType          sysType;
    larkxrPlatFromType        platFromType;
    larkxrHeadSetSDKVersion   headSetSdkVersion;
    char                      sdkVersionName[LARK_XR_MAX_NAME_SIZE];
    char                      supportServerVersion[LARK_XR_MAX_VERSION_SIZE];
    char                      deviceName[LARK_XR_MAX_NAME_SIZE];
    char                      macAddress[LARK_XR_MAX_MACADDRESS_SIZE];
    uint64_t                  diskTotal;
    uint64_t                  diskUsed;
    uint64_t                  sdTotal;
    uint64_t                  sdUsed;
    char                      dataPath[LARK_XR_MAX_PATH_SIZE];
}  larkxrSystemInfo;

//
// 系统当前内存信息
//
typedef struct larkxrDeviceMemInfo_ {
    uint64_t memTotal;
    uint64_t memUsed;
} larkxrDeviceMemInfo;

//
// 系统当前电量信息
//
typedef struct larkxrDeviceBatteryInfo_ {
    int hmd;
    int controlerLeft;
    int controlerRight;
} larkxrDeviceBatteryInfo;
// end system typs.

//
// vr 渲染左右眼类型
//
typedef enum larkxrEye_ {
    LARKXR_EYE_LEFT   = 0,
    LARKXR_EYE_RIGHT  = 1,
    LARKXR_EYE_COUNT  = 2,
    LARKXR_EYE_BOTH   = 3,
    LARKAR_EYE        = 4,
} larkxrEye;

//
// 提交给云端 vr 渲染用的 fov。fov 应从 vr 头盔sdk中获取正确的值。以度数为单位。
//
typedef struct larkxrRenderFov_ {
    float left;
    float right;
    float top;
    float bottom;
} larkxrRenderFov;


//
// vr 渲染相关信息，包括帧率，瞳距，分辨率，左右眼fov
//
typedef struct larkxrRenderInfo_ {
    int fps;
    float ipd;
    // 左右眼加一起的宽度
    int renderWidth;
    int renderHeight;
    // 渲染的视场角，以度数为单位。
    larkxrRenderFov fov[LARKXR_EYE_COUNT];
} larkxrRenderInfo;
// end render info

//
// device types.
//
// 设备类型
typedef enum larkxrDeviceType_ {
    // 头盔
    Larkxr_Device_Type_HMD = 0,
    // 1：左手柄
    Larkxr_Device_Type_Controller_Left = 1,
    // 2：右手柄
    Larkxr_Device_Type_Controller_Right = 2,
} larkxrDeviceType;

typedef enum larkxrControllerType_ {
    Larkxr_Controller_Left = Larkxr_Device_Type_Controller_Left,
    Larkxr_Controller_Right = Larkxr_Device_Type_Controller_Right
} larkxrControllerType;

//
typedef enum larkxrPoseOriginModel_ {
    Larkxr_PoseOriginModel_OriginOnHead                  = 0, /**< The origin of 6 DoF pose is on head. */
    Larkxr_PoseOriginModel_OriginOnGround                = 1, /**< The origin of 6 DoF pose is on ground. */
    Larkxr_PoseOriginModel_OriginOnTrackingObserver      = 2, /**< The raw pose from tracking system. */
    Larkxr_PoseOriginModel_OriginOnHead_3DoF             = 3, /**< The origin of 3 DoF pose is on head. */
} larkxrPoseOriginModel;

typedef enum larkxrNumDoF_ {
    Larkxr_NumDoF_3DoF = 0,
    Larkxr_NumDoF_6DoF = 1,
} larkxrNumDoF;

typedef enum larkxrIntensity_ {
    Larkxr_Intensity_Weak       = 1,   /**< The Intensity of vibrate is Weak. */
    Larkxr_Intensity_Light      = 2,   /**< The Intensity of vibrate is Light. */
    Larkxr_Intensity_Normal     = 3,   /**< The Intensity of vibrate is Normal. */
    Larkxr_Intensity_Strong     = 4,   /**< The Intensity of vibrate is Strong. */
    Larkxr_Intensity_Severe     = 5,   /**< The Intensity of vibrate is Severe. */
} larkxrIntensity;

/**
* math 
* same as glm
*/
typedef struct larkxrVec2f {
    float x;
    float y;

#ifdef __cplusplus
    larkxrVec2f() : x(0), y(0) {}

    larkxrVec2f(float x, float y) : x(x), y(y){}

#ifdef GLM_SETUP_INCLUDED
    larkxrVec2f(const glm::vec2& vec2) : x(vec2.x), y(vec2.y) {}

    glm::vec2 toGlm() const { return glm::vec2(x, y); }

    void operator = (const glm::vec2& vec2) {
        x = vec2.x;
        y = vec2.y;
    }
    void operator = (const larkxrVec2f& vec2) {
        x = vec2.x;
        y = vec2.y;
    }
#endif // GLM_SETUP_INCLUDED

#endif
} larkxrVec2f;

typedef struct larkxrVec3f {
    float x;
    float y;
    float z;

#ifdef __cplusplus
    larkxrVec3f(): x(0), y(0), z(0) {}

    larkxrVec3f(float x, float y, float z): x(x), y(y), z(z) {}

#ifdef GLM_SETUP_INCLUDED
    larkxrVec3f(const glm::vec3& vec3) : x(vec3.x), y(vec3.y), z(vec3.z) {}

    glm::vec3 toGlm() const { return glm::vec3(x, y, z);  }

    void operator = (const glm::vec3& vec3) {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
    }

    void operator = (const larkxrVec3f& vec3) {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
    }
#endif // GLM_SETUP_INCLUDED

#endif

} larkxrVec3f;

typedef struct larkxrQuatf {
    float x;
    float y;
    float z;
    float w;

#ifdef __cplusplus
    larkxrQuatf(): x(0), y(0), z(0), w(0) {}

    larkxrQuatf(float w, float x, float y, float z): w(w), x(x), y(y), z(z) {}

#ifdef GLM_SETUP_INCLUDED
    larkxrQuatf(const glm::quat& quat): x(quat.x), y(quat.y), z(quat.z), w(quat.w) {}

    glm::quat toGlm() const { return glm::quat(w, x, y, z); }

    void operator = (const glm::quat& quat) {
        x = quat.x;
        y = quat.y;
        z = quat.z;
        w = quat.w;
    }

    void operator = (const larkxrQuatf& quat) {
        x = quat.x;
        y = quat.y;
        z = quat.z;
        w = quat.w;
    }
#endif // GLM_SETUP_INCLUDED

#endif
} larkxrQuatf;

typedef struct larkxrMatrix4x4f {
    float m[16];

#ifdef __cplusplus
    larkxrMatrix4x4f() {
        m[0] = m[1] = m[2] = m[3] = m[4] = m[5] = m[6] = m[7] = m[8] = m[9] = m[10] = m[11] = m[12] = m[13] = m[14] = m[15] = 0;
    }

    larkxrMatrix4x4f(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
        ) {
        m[0] = m00;
        m[1] = m01;
        m[2] = m02;
        m[3] = m03;

        m[4] = m10;
        m[5] = m11;
        m[6] = m12;
        m[7] = m13;

        m[8] = m20;
        m[9] = m21;
        m[10] = m22;
        m[11] = m23;

        m[12] = m30;
        m[13] = m31;
        m[14] = m32;
        m[15] = m33;
    }
#ifdef GLM_SETUP_INCLUDED
    larkxrMatrix4x4f(const glm::mat4& mat4) {
        m[0] = mat4[0][0];
        m[1] = mat4[0][1];
        m[2] = mat4[0][2];
        m[3] = mat4[0][3];

        m[4] = mat4[1][0];
        m[5] = mat4[1][1];
        m[6] = mat4[1][2];
        m[7] = mat4[1][3];

        m[8] = mat4[2][0];
        m[9] = mat4[2][1];
        m[10] = mat4[2][2];
        m[11] = mat4[2][3];

        m[12] = mat4[3][0];
        m[13] = mat4[3][1];
        m[14] = mat4[3][2];
        m[15] = mat4[3][3];
    }

    glm::mat4 toGlm() const {
        return glm::mat4(
            m[0],  m[1],  m[2],  m[3],  // col0 x0 y0 z0 w0
            m[4],  m[5],  m[6],  m[7],  // col1 x1 y1 z1 w1
            m[8],  m[9],  m[10], m[11], // col2 x2 y2 z2 w2
            m[12], m[13], m[14], m[15]  // col3 x3 y3 z3 w3
        );
    }

    void operator = (const larkxrMatrix4x4f& matrix4X4f) {
        m[0] = matrix4X4f.m[0];
        m[1] = matrix4X4f.m[1];
        m[2] = matrix4X4f.m[2];
        m[3] = matrix4X4f.m[3];

        m[4] = matrix4X4f.m[4];
        m[5] = matrix4X4f.m[5];
        m[6] = matrix4X4f.m[6];
        m[7] = matrix4X4f.m[7];

        m[8] = matrix4X4f.m[8];
        m[9] = matrix4X4f.m[9];
        m[10] = matrix4X4f.m[10];
        m[11] = matrix4X4f.m[11];

        m[12] = matrix4X4f.m[12];
        m[13] = matrix4X4f.m[13];
        m[14] = matrix4X4f.m[14];
        m[15] = matrix4X4f.m[15];
    }

    void operator = (const glm::mat4& mat4) {
        m[0] = mat4[0][0];
        m[1] = mat4[0][1];
        m[2] = mat4[0][2];
        m[3] = mat4[0][3];

        m[4] = mat4[1][0];
        m[5] = mat4[1][1];
        m[6] = mat4[1][2];
        m[7] = mat4[1][3];

        m[8] = mat4[2][0];
        m[9] = mat4[2][1];
        m[10] = mat4[2][2];
        m[11] = mat4[2][3];

        m[12] = mat4[3][0];
        m[13] = mat4[3][1];
        m[14] = mat4[3][2];
        m[15] = mat4[3][3];
    }

#endif // GLM_SETUP_INCLUDED
#endif
} larkxrMatrix4x4f;

/**
 * 设备姿态数据。
 *  test data from wvr
 *  rotation:     -0.275494039 0.103380702 0.0448519662 0.95467472
 *  raw rotation: 0.275494069 -0.10338071 -0.0448519699 0.954674661
 *   is6dof: 1; rawX: 0.124201; rawY: -0.027059; rawZ: -0.041025;
 *                 X: -0.124201; Y: 0.027059; Z: 0.041025;
 */
typedef struct larkxrTrackedPose_ {
    larkxrDeviceType device;
    BOOL isConnected;
    BOOL is6Dof;
    BOOL isValidPose;
    int status;
    int64_t timestamp;                  /**< Absolute time (in nanosecond) of pose. */
    int64_t poseFetchTime;
    int64_t expectedDisplayTime;
    float predictedMilliSec;            /**< Number of milliseconds from now to predict poses. */
    larkxrPoseOriginModel poseOriginModel;
    larkxrVec3f position;
    larkxrQuatf rotation;
    larkxrVec3f velocity;
    larkxrVec3f angularVelocity;
    larkxrVec3f acceleration;
    larkxrVec3f angularAcceleration;
    // raw porstion from sene
    larkxrVec3f rawPosition;
    larkxrQuatf rawRotation;
    // pose matrix for htc wave
    larkxrMatrix4x4f rawPoseMatrix;
    struct
    {
        larkxrVec3f       viewPosition; // view position for openxr stereo view pose
        larkxrQuatf       viewRotation; // view rotation for openxr stereo view pose
        larkxrMatrix4x4f  projectionMatrix;
        larkxrMatrix4x4f  viewMatrix;
    } eye[ LARKXR_EYE_COUNT ];
#if defined(NOLO_6DOF)
    larkxrVec3f baseLineOffset; // 头部基准线偏移。nolo 使用
    larkxrVec3f noloPosition;   // nolo position.
    larkxrQuatf noloRotation;
#endif
    int battery; // battery 0 - 100;
} larkxrTrackedPose;

typedef enum larkxrNoloInputId_ {
    Larkxr_InputId_0     = 0,
    Larkxr_InputId_1     = 1,
    Larkxr_InputId_2     = 2,
    Larkxr_InputId_3     = 3,
    Larkxr_InputId_4     = 4,
    Larkxr_InputId_5     = 5,

    //alias group mapping
    Larkxr_InputId_TouchPad_Press   = Larkxr_InputId_0,
    Larkxr_InputId_Trigger          = Larkxr_InputId_1,
    Larkxr_InputId_AppMenu          = Larkxr_InputId_2,
    Larkxr_InputId_Home             = Larkxr_InputId_3,
    Larkxr_InputId_Grip             = Larkxr_InputId_4,
    Larkxr_InputId_TouchPadTouch    = Larkxr_InputId_5,
} larkxrNoloInputId;

//
// 设备输入状态
//
typedef enum larkxrInput_ {
    larkxr_Input_System_Click,
    larkxr_Input_Application_Menu_Click,
    larkxr_Input_Grip_Click,
    larkxr_Input_Grip_Value,
    larkxr_Input_Grip_Touch,
    larkxr_Input_DPad_Left_Click,
    larkxr_Input_DPad_Up_click,
    larkxr_Input_DPad_Right_Click,
    larkxr_Input_DPad_Down_Click,
    larkxr_Input_A_Click,
    larkxr_Input_A_Touch,
    larkxr_Input_B_Click,
    larkxr_Input_B_Touch,
    larkxr_Input_X_Click,
    larkxr_Input_X_Touch,
    larkxr_Input_Y_Click,
    larkxr_Input_Y_Touch,
    larkxr_Input_Trigger_Left_Value,
    larkxr_Input_Trigger_Right_Value,
    larkxr_Input_Shoulder_Left_Click,
    larkxr_Input_Shoulder_Right_Click,
    larkxr_Input_Joystick_Left_Click,
    larkxr_Input_Joystick_Left_X,
    larkxr_Input_Joystick_Left_Y,
    larkxr_Input_Joystick_Right_Click,
    larkxr_Input_Joystick_Right_X,
    larkxr_Input_Joystick_Right_Y,
    larkxr_Input_Joystick_Click,
    larkxr_Input_Joystick_X,
    larkxr_Input_Joystick_Y,
    larkxr_Input_Joystick_Touch,
    larkxr_Input_Back_Click,
    larkxr_Input_Guide_Click,
    larkxr_Input_Start_Click,
    larkxr_Input_Trigger_Click,
    larkxr_Input_Trigger_Value,
    larkxr_Input_Trigger_Touch,
    larkxr_Input_Trackpad_X,
    larkxr_Input_Trackpad_Y,
    larkxr_Input_Trackpad_Click,
    larkxr_Input_Trackpad_Touch,

    larkxr_Input_MAX = larkxr_Input_Trackpad_Touch,
    larkxr_Input_COUNT = larkxr_Input_MAX + 1
} larkxrInput;
#define LARKXR_BUTTON_FLAG(input) (1ULL << input)

//
// 手柄的数据输入状态
//
typedef struct larkxrControllerInputState_ {
    larkxrDeviceType deviceType;
    BOOL isConnected;
    uint64_t buttons;
    /// d          ! y = 1
    /// d          |
    /// d          |
    /// d  --------|--------> x 1
    /// d          |
    /// d          | -1
    /// d        openvr
    larkxrVec2f touchPadAxis;
    float triggerValue; // trigger axis 0 - 1.0f
    float gripValue;
    uint8_t batteryPercentRemaining;
} larkxrControllerInputState;

//
// 手柄设备状态
//
typedef struct larkxrControllerDeviceState_ {
    larkxrControllerType deviceType; // controller left or right
    larkxrTrackedPose pose;
    larkxrControllerInputState inputState;
    float rotateDeg; // rotate the controller.
    larkxrVec3f rotateAxis;
} larkxrControllerDeviceState;

//
// 设备电量
//
typedef enum larkxrBatteryStatus_ {
    Larkxr_BatteryStatus_Unknown = 0,
    Larkxr_BatteryStatus_Normal = 1,
    Larkxr_BatteryStatus_Low = 2,
    Larkxr_BatteryStatus_UltraLow = 3,
} larkxrBatteryStatus;

const uint8_t LARKVR_BATTERY_STATUS_NUMBER_NORMAL = 100;
const uint8_t LARKVR_BATTERY_STATUS_NUMBER_LOW = 50;
const uint8_t LARKVR_BATTERY_STATUS_NUMBER_ULTRALLOW = 10;

typedef enum larkxrAsyncTaskType_ {
    Larkxr_Vibrator_Controller_Left = 0,
    Larkxr_Vibrator_Controller_Right,
} larkxrAsyncTaskType;

const int LARKVR_TOTAL_CONTROLLER_COUNT = 2;

//
// 包括头盔和手柄在内的完整设备信息.
//
typedef struct larkxrDevicePair_ {
    larkxrTrackedPose hmdPose;
    larkxrControllerDeviceState controllerState[LARKVR_TOTAL_CONTROLLER_COUNT];
} larkxrDevicePair;
// end deive types.

//
// 所有设备的姿态数据帧
//
typedef struct larkxrTrackingDevicePairFrame_ {
    uint64_t frameIndex;
    uint64_t fetchTime;
    double displayTime;
    larkxrDevicePair devicePair;
} larkxrTrackingDevicePairFrame;

//
// 姿态数帧
//
typedef struct larkxrTrackingFrame_ {
    larkxrTrackedPose tracking;
    uint64_t frameIndex;
    uint64_t fetchTime;
    double displayTime;
} larkxrTrackingFrame;

//
// Player space data
//
typedef struct larkxrPlaySpace_
{
    int             recenterCount;
    larkxrVec3f     position;
    larkxrQuatf     rotation;
    float			area_width;
    float			area_height;
    uint32_t		perimeterPointsCount;						 //default  =  0
    float           (*perimeterPoints)[3];
} larkxrPlaySpace;

//
// 硬件视频解码纹理类型
//
typedef enum larkxrHwRenderTextureType_ {
    // 不包含纹理
    larkxrHwRenderTextureType_None                 = -1,
    // Android gltexture2d 纹理 左右眼在一起
    larkxrHwRenderTextureType_Android_Multiview    = 4,
    // android gltexture2d opengl 双眼分开，左眼一个纹理右眼一个纹理
    larkxrHwRenderTextureType_Android_Stereo       = 5,
    // windows d3d11 native texture ShaderResourceView 左右眼在一起
    larkxrHwRenderTextureType_D3D11_Multiview      = 6,
    // windows d3d11 native texture ShaderResourceView 双眼分开
    larkxrHwRenderTextureType_D3D11_Stereo         = 7,
    // windows d3d11 nv12 native texture 左右眼在一起
    larkxrHwRenderTextureType_D3D11_NV12           = 8,
    // windows d3d11 yuv texture, ShaderResourceView 类型
    // 分成 y 和 uv 两个纹理，从 nv12 纹理中取出。左右眼在一起
    larkxrHwRenderTextureType_D3D11_Y_UV_SRV       = 9,
} larkxrHwRenderTextureType;

//
// 硬件视频解码的纹理
// 
typedef struct larkxrHwRenderTexture_
{
    larkxrHwRenderTextureType type;
    void* textureSlot1;
    void* textureSlot2;
    int width;
    int height;
} larkxrHwRenderTexture;
//#pragma pack(pop) // end pack
#endif //CLOUDLARKVRMODULE_TYPES_H
