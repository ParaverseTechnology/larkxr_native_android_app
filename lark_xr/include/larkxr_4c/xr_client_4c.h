//
// Created by fcx on 2020/5/17.
//

#ifndef CLOUDLARKXR_XR_CLIENT_4C_H
#define CLOUDLARKXR_XR_CLIENT_4C_H

#include "lark_xr/types.h"
#include "lark_xr/lk_common_types.h"
#include "lark_xr/lark_xr.h"
#ifdef WIN32
#include <d3d11.h>
#elif __ANDROID__
#include "jni.h"
#endif

#if defined( __cplusplus )
extern "C" {
#endif

/***
* 云雀 VR SDK 主要调用如下.
* larkxr_InitSdkAuthorization------------初始化SDK ID
*  larkxr_InitContext--------------------初始化系统上下文
*  larkxr_InitRenderInfo-----------------初始化渲染信息
*          |
*          |
*  larkxr_IsConnected--------------------是否连接成功
*          |
*          |
*    larkxr_IsFrameInited----------------渲染纹理是否初始
*    larkxr_GetRenderTexurte-------------获取纹理并初始化渲染
*          |
*  larkxr_UpdateDevicePose---------------更新姿态
*  larkxr_UpdateControllerInputState-----更新手柄状态
*          |
*          |-------| loop
*          |       |
*    larkxr_Render-| --------------------渲染。如果没有新的帧应跳过本次渲染
*          |       |
*          |-------|
*          |
*   larkxr_IsConnected------------------连接关闭结束渲染更新循环
*          |
*          |
*   larkxr_ReleaseContext------------------清理系统上下文
*/
//--------------------------------------------------------------------------------------------------
//
// callbacks
typedef void(*lk_on_connected)(void* user_data);
typedef void(*lk_on_close)(int code, void* user_data);
typedef void(*lk_on_info)(int info_code, const char* msg, void* user_data);
typedef void(*lk_on_error)(int error_code, const char* msg, void* user_data);

LARK_XR_API void DC_CALL larkxr_ClearCallback();
LARK_XR_API void DC_CALL larkxr_SetCallbackUserData(void* user_data);
LARK_XR_API void DC_CALL larkxr_SetCallbackOnConnected(lk_on_connected on_connectded);
LARK_XR_API void DC_CALL larkxr_SetCallbackOnClose(lk_on_close on_close);
LARK_XR_API void DC_CALL larkxr_SetCallbackOnInfo(lk_on_info on_info);
LARK_XR_API void DC_CALL larkxr_SetCallbackOnError(lk_on_error on_error);

//--------------------------------------------------------------------------------------------------
// main apis
// 初始化系统上下文
// 需要先调用 larkxr_InitContext

#ifdef __ANDROID__
LARK_XR_API void DC_CALL larkxr_InitContext(JavaVM *vm);

// should call InitGLShareContext/ReleaseGLShareContext
LARK_XR_API void DC_CALL larkxr_InitContextWithoutGl(JavaVM *vm);
LARK_XR_API void DC_CALL larkxr_InitGLShareContext();
LARK_XR_API void DC_CALL larkxr_ReleaseGLShareContext();
#else
LARK_XR_API void DC_CALL larkxr_InitContext();
LARK_XR_API void DC_CALL larkxr_InitContextDebug(BOOL is_debug, const char* log_path);
LARK_XR_API void DC_CALL larkxr_InitD3D11Device(ID3D11Device* d3d11_device);
LARK_XR_API void DC_CALL larkxr_ReleaseD3D11Device();
#endif

// 是否启用 debug 模式
LARK_XR_API void DC_CALL larkxr_EnableDebugMode(BOOL is_debug);

LARK_XR_API void DC_CALL larkxr_EnableDebugModePath(BOOL is_debug, const char* log_file);

// 清理系统上下文
LARK_XR_API void DC_CALL larkxr_ReleaseContext();

// 注册 sdk
LARK_XR_API BOOL DC_CALL larkxr_InitSdkAuthorization(const char* id);

// not use.
LARK_XR_API BOOL DC_CALL larkxr_InitSdkAuthorizationWithSecret(const char* id, const char* secret);

LARK_XR_API void DC_CALL larkxr_SetCertificate(const char* appKey, const char* appSecret);

// 查看错误码
LARK_XR_API int DC_CALL larkxr_GetLastError();

// 清理错误码
LARK_XR_API void DC_CALL larkxr_ClearError();

// 检查是否连接成功
LARK_XR_API BOOL DC_CALL larkxr_IsConnected();

// 检查是否暂停
LARK_XR_API BOOL DC_CALL larkxr_IsPause();

// 检查是否有帧初始化成功
LARK_XR_API BOOL DC_CALL larkxr_IsFrameInited();

// 获取渲染纹理。安卓中返回的是 oes texture。
// win32 返回 DXGI_FORMAT_NV12 类型
LARK_XR_API int DC_CALL larkxr_GetRenderTexture();

// 获取渲染

// 解码帧的类型 
// 具体为 XRVideoFrame::FrameType 中的类型
// win32 返回 DXGI_FORMAT_NV12 类型左右眼在一起的纹理
LARK_XR_API int DC_CALL larkxr_GetRenerTextureType();

// 
// 获取硬件解码纹理
LARK_XR_API larkxrHwRenderTexture DC_CALL larkxr_GetHwRenerTexture();

// 清理渲染纹理
LARK_XR_API void DC_CALL larkxr_ClearTexture();

// 检查是否有收到新的帧。也可以调用 larkxr_Render，省略该检查。
LARK_XR_API BOOL DC_CALL larkxr_HasNewFrame();

LARK_XR_API BOOL DC_CALL larkxr_WaitFroNewFrame(int timeoutMillisecond);

// 渲染云端返回的画面。通过参数 trakcingFrame 带回渲染所需的姿态信息。
// 该姿态信息是云端渲染该帧所用的姿态信息。一般用在最终的渲染提交中。
// 如果返回失败，请跳过该次渲染。
LARK_XR_API BOOL DC_CALL larkxr_Render(larkxrTrackingFrame *trackingFrame);

// unity 调用
LARK_XR_API BOOL DC_CALL larkxr_Render2(uint64_t *frameIndex, uint64_t *fetchTime, double *displayTime,
                    float *px, float *py, float *pz,
                    float *rx, float *ry, float *rz, float *rw);

LARK_XR_API BOOL DC_CALL larkxr_RenderQueue(larkxrHwRenderTexture* nativeTexture, uint64_t *frameIndex, uint64_t *fetchTime, double *displayTime,
                                        float *px, float *py, float *pz,
                                        float *rx, float *ry, float *rz, float *rw);
LARK_XR_API void DC_CALL larkxr_RenderQueueRelease();

// 设置服务器地址
LARK_XR_API void DC_CALL larkxr_SetServerAddr(const char* ip, int port);

// 连接云端应用
LARK_XR_API void DC_CALL larkxr_EnterAppli(const char* appliId);

/**
 * 使用 json 字符串进入应用，
 * 云端应用 id 从应用列表接口回调处获取。
 * json 中可添加的接口有
 * https://www.pingxingyun.com/online/api3_2.html?id=532
 * 1.2.2 进入应用接口
 * @param jsonStr
 */
LARK_XR_API void DC_CALL larkxr_EnterAppliWithJsonString(const char* jsonStr);

// 关闭云端应用
LARK_XR_API void DC_CALL larkxr_Close();

// 系统生命周期
// android 创建成功
LARK_XR_API void DC_CALL larkxr_OnCreated();
// android 恢复
LARK_XR_API void DC_CALL larkxr_OnResume();
// android 暂停
LARK_XR_API void DC_CALL larkxr_OnPause();
// android 销毁
LARK_XR_API void DC_CALL larkxr_OnDestory();
// deivces
// 更新设置姿态数据
LARK_XR_API void DC_CALL larkxr_UpdateDevicePose(larkxrDeviceType deviceType, larkxrTrackedPose *pose);

// 更新设置姿态数据
// unity 调用
LARK_XR_API void DC_CALL larkxr_UpdateDevicePose2(larkxrDeviceType deviceType, float px, float py, float pz,
                              float rx, float ry, float rz, float rw);

// 更新手柄状态。包括按钮状态等
LARK_XR_API void DC_CALL larkxr_UpdateControllerInputState(larkxrControllerType controllerType,
                                       larkxrControllerInputState *controllerInputState);

// 更新手柄状态。包括按钮状态等
// unity 调用
LARK_XR_API void DC_CALL larkxr_UpdateControllerInput2(larkxrControllerType controllerType, BOOL isConnected,
                                               BOOL touchPadPressed, BOOL triggerPressed,
                                               BOOL digitTriggerPressed, BOOL appMenuPressed,
                                               BOOL homePressed, BOOL gripPressed, BOOL volumUpPressed,
                                               BOOL volumDownPressed, BOOL touchPadTouched);

// 更新手柄整体状态，包括姿态和按钮状态。
LARK_XR_API void DC_CALL larkxr_UpdateControllerState(larkxrControllerType controllerType,
                                  larkxrControllerDeviceState *controllerDeviceState);

// 更新设备整体设备状态。
LARK_XR_API void DC_CALL larkxr_UpdateDevicePair(larkxrDevicePair *devicePair);

// 获取当前设备姿态。一般供云雀 sdk 内部调用.
LARK_XR_API BOOL DC_CALL larkxr_GetDevicePose(larkxrDeviceType deviceType,
                          larkxrTrackedPose *pose, uint32_t ipredictMillsec);

// 获取手柄状态。一般供云雀 sdk 内部调用.
LARK_XR_API BOOL DC_CALL larkxr_GetControllerInputState(larkxrControllerType deviceType,
                                    larkxrControllerInputState *inputState);

// 发送姿态数据.
LARK_XR_API void DC_CALL larkxr_SendDevicePair(uint64_t frameIndex, uint64_t fetchTime, double displayTime);

// 获取是否为 3dof 或 6dof。**待完善**
LARK_XR_API BOOL DC_CALL larkxr_DeviceDegreeOfDof(larkxrDeviceType deviceType, larkxrNumDoF *dof);

// 检擦设备是否连接成功
LARK_XR_API BOOL DC_CALL larkxr_DeviceIsConnected(larkxrDeviceType deviceType);

// 重定位设备，**待完善**
LARK_XR_API void DC_CALL larkxr_Rencenter(larkxrDeviceType deviceType);

// 触发震动, **待完善**
LARK_XR_API void DC_CALL larkxr_TriggerVibrator(larkxrControllerType controllerType, unsigned short duration,
                            larkxrIntensity intensity);

// 同步触发震动, **待完善**
LARK_XR_API void DC_CALL larkxr_SyncTriggerVibrator(larkxrControllerType controllerType, unsigned short duration,
                                larkxrIntensity intensity);

// 获取电池电量,状态信息。**待完善**
LARK_XR_API void DC_CALL larkxr_GetBatteryStatus(larkxrDeviceType deviceType, larkxrBatteryStatus *status);

// 获取电池电量百分比。
LARK_XR_API uint8_t DC_CALL larkxr_GetBatteryStatusNumber(larkxrDeviceType deviceType);

// 设置是否启用震动。**待完善**
LARK_XR_API void DC_CALL larkxr_SetEnableVibrator(BOOL enable);

// 异步循环.**待完善**
LARK_XR_API void DC_CALL larkxr_AsyncTaskLoop();
//--------------------------------------------------------------------------------------------------
// 在最终渲染时调用，用于收集延时信息。
// 在最终渲染时调用，用于收集延时信息。
LARK_XR_API void DC_CALL larkxr_LatencyCollectorrSubmit(uint64_t frameIndex, float blackDegree);
//--------------------------------------------------------------------------------------------------
// 用户设置
LARK_XR_API void DC_CALL larkxr_SetupUserSetting(larkxrUserSetting *userSeting);

// 读取所有用户设置
LARK_XR_API void DC_CALL larkxr_GetUserSetting(larkxrUserSetting *userSeting);

// 设置比特率
LARK_XR_API void DC_CALL larkxr_SetupBitrateKbps(int bitrate);

// 获取比特率
LARK_XR_API int DC_CALL larkxr_GetBitrateKbps();

// 获取默认比特率
LARK_XR_API int DC_CALL larkxr_GetDefaultBitrateKbps();

// 设置房间高度
LARK_XR_API void DC_CALL larkxr_SetupRoomHeight(float roomHeight);

// 获取房间高度
LARK_XR_API float DC_CALL larkxr_GetRoomHeight();

// 获取默认房间高度
LARK_XR_API float DC_CALL larkxr_GetDefaultRoomHeight();
//--------------------------------------------------------------------------------------------------
// system apis
// 检擦系统信息是否初始化完成
LARK_XR_API BOOL DC_CALL larkxr_SystemInited();

// 返回系统信息
LARK_XR_API void DC_CALL larkxr_GetSystemInfo(larkxrSystemInfo *info);

// 返回系统信息
// unity 调用
LARK_XR_API void DC_CALL larkxr_GetSystemInfo2(BOOL *isInited, larkxrSystemType *systemType,
                           larkxrPlatFromType *platFromType, larkxrHeadSetSDKVersion *headSetSdkVersion);

//--------------------------------------------------------------------------------------------------
// render info
// 获取默认配置
LARK_XR_API void DC_CALL larkxr_GetDefaultRenderInfo(larkxrRenderInfo *defaultInfo);

// 获取渲染信息。
LARK_XR_API void DC_CALL larkxr_RenderInfo(larkxrRenderInfo *info);

// 设置渲染信息。
// 请注意渲染的大小和视场角，如果不正确的话会导致画面的畸变。
LARK_XR_API void DC_CALL larkxr_InitRenderInfo(larkxrRenderInfo *renderInfo);

// 设置分辨率
LARK_XR_API void DC_CALL larkxr_SetRenderResolution(int renderWidth, int renderHeight);

// 获取分辨率
LARK_XR_API void DC_CALL larkxr_GetRenderResolution(int *renderWidth, int *renderHeight);

// 获取默认分辨率
LARK_XR_API void DC_CALL larkxr_GetDefaultResolution(int *renderWidth, int *renderHeight);

// 设置瞳距
LARK_XR_API void DC_CALL larkxr_SetIpd(float ipd);

// 获取瞳距
LARK_XR_API float DC_CALL larkxr_GetIpd();

// 获取默认瞳距
LARK_XR_API float DC_CALL larkxr_GetDefaultIpd();

// 设置渲染帧率
LARK_XR_API void DC_CALL larkxr_SetRenderFps(int fps);
// 获取当前渲染帧率
LARK_XR_API int DC_CALL larkxr_GetRenderFps();
// 默认 FPS
LARK_XR_API int DC_CALL larkxr_GetDefaultFps();

// 设置 fov
LARK_XR_API void DC_CALL larkxr_SetFov(larkxrRenderFov *fov, int cout);

// 设置 fov
// unity 用
LARK_XR_API void DC_CALL
 larkxr_SetFov2(float eyeLeft_Left, float eyeLeft_Right, float eyeLeft_Top, float eyeLeft_Bottom,
               float eyeRight_Left, float eyeRight_Right, float eyeRight_Top,
               float eyeRight_Bottom);

LARK_XR_API void DC_CALL
 larkxr_GetFov2(float *eyeLeft_Left, float *eyeLeft_Right, float *eyeLeft_Top, float *eyeLeft_Bottom,
               float *eyeRight_Left, float *eyeRight_Right, float *eyeRight_Top,
               float *eyeRight_Bottom);

LARK_XR_API void DC_CALL larkxr_GetDefaultFov2(float *eyeLeft_Left, float *eyeLeft_Right, float *eyeLeft_Top,
                           float *eyeLeft_Bottom,
                           float *eyeRight_Left, float *eyeRight_Right, float *eyeRight_Top,
                           float *eyeRight_Bottom);

// set larkHeadSetControllerDesc
LARK_XR_API void DC_CALL lakrxr_SetHeadSetControllerDesc(larkHeadSetControllerDesc* headset_desc);
// get larkHeadSetControllerDesc
LARK_XR_API void DC_CALL lakrxr_GetHeadSetControllerDesc(larkHeadSetControllerDesc* headset_desc);
// get default head set control
LARK_XR_API void DC_CALL lakrxr_GetDefaultHeadSetControllerDesc(larkHeadSetControllerDesc* headset_desc);

// set use mutiview. stereo mode only support in android.
LARK_XR_API void DC_CALL larkxr_SetUseMultiview(BOOL useMulti);
// set flip drawing. only suuport in android.
LARK_XR_API void DC_CALL larkxr_SetFlipDraw(BOOL flipDraw);

// setup fov rendeing
LARK_XR_API void DC_CALL larkxr_SetFoveatedRendering(larkFoveatedRendering* fovRending);
// get fov rendeing setup
LARK_XR_API void DC_CALL larkxr_GetFoveatedRendering(larkFoveatedRendering* fovRending);
LARK_XR_API void DC_CALL larkxr_GetDefaultFoveatedRendering(larkFoveatedRendering* fovRending);

// setup 
LARK_XR_API void DC_CALL larkxr_SetColorCorrention(larkColorCorrention* colorCorrection);
LARK_XR_API void DC_CALL larkxr_GetColorCorrention(larkColorCorrention* colorCorrection);
LARK_XR_API void DC_CALL larkxr_GetDefaultColorCorrention(larkColorCorrention* colorCorrection);

// quick config with level
LARK_XR_API void DC_CALL larkxr_QuickConfigWithDefaulSetup(int level);
LARK_XR_API int DC_CALL larkxr_GetDefaultQuickConfigLevel();

// ResolutionScale
LARK_XR_API void DC_CALL larkxr_SetResolutionScale(float scale);
LARK_XR_API float DC_CALL larkxr_GetResolutionScale();

// use h265
LARK_XR_API void DC_CALL larkxr_SetUseH265(BOOL use);
LARK_XR_API BOOL DC_CALL larkxr_GetUseH265();

// stream type
LARK_XR_API void DC_CALL larkxr_GetStreamType(larkStreamType* type);
LARK_XR_API void DC_CALL larkxr_GetDefaultStreamType(larkStreamType* type);
LARK_XR_API void DC_CALL larkxr_SetStreamType(larkStreamType type);


//数据通道相关接口
LARK_XR_API void DC_CALL larkxr_SendData(const char* buffer, int length);
LARK_XR_API void DC_CALL larkxr_SendString(const char* buffer);
LARK_XR_API void DC_CALL larkxr_SendAudioData(const char* buffer, int length);
//

// render config
LARK_XR_API void DC_CALL larkxr_SetUseRenderQueue(BOOL useRenderQueue);
// set render queue size
LARK_XR_API void DC_CALL larkxr_SetRenderQueueSize(int renderQueueSize);
//--------------------------------------------------------------------------------------------------
#if defined( __cplusplus )
}    // extern "C"
#endif

#endif //CLOUDLARKXR_XR_CLIENT_4C_H
