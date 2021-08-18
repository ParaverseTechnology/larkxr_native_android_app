//
// Created by fcx on 2020/5/17.
//

#ifndef CLOUDLARKXR_XR_CLIENT_4C_H
#define CLOUDLARKXR_XR_CLIENT_4C_H

#include "lark_xr/types.h"

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
// main apis
// 初始化系统上下文
// 需要先调用 larkxr_InitContext

#ifdef __ANDROID__
void larkxr_InitContext(JavaVM *vm);
#else
void larkxr_InitContext();
#endif
// 清理系统上下文
void larkxr_ReleaseContext();

// 注册 sdk
bool larkxr_InitSdkAuthorization(const char* id);

// 查看错误码
int larkxr_GetLastError();

// 检查是否连接成功
bool larkxr_IsConnected();

// 检查是否暂停
bool larkxr_IsPause();

// 检查是否有帧初始化成功
bool larkxr_IsFrameInited();

// 获取渲染纹理。安卓中返回的是 oes texture。
int larkxr_GetRenderTexurte();

// 清理渲染纹理
void larkxr_ClearTexture();

// 检查是否有收到新的帧。也可以调用 larkxr_Render，省略该检查。
bool larkxr_HasNewFrame();

// 渲染云端返回的画面。通过参数 trakcingFrame 带回渲染所需的姿态信息。
// 该姿态信息是云端渲染该帧所用的姿态信息。一般用在最终的渲染提交中。
// 如果返回失败，请跳过该次渲染。
bool larkxr_Render(larkxrTrackingFrame *trackingFrame);

// unity 调用
bool larkxr_Render2(uint64_t *frameIndex, uint64_t *fetchTime, double *displayTime,
                    float *px, float *py, float *pz,
                    float *rx, float *ry, float *rz, float *rw);

// 设置服务器地址
void larkxr_SetServerAddr(const char* ip, int port);

// 连接云端应用
void larkxr_EnterAppli(const char* appliId);

// 关闭云端应用
void larkxr_Close();

// 系统生命周期
// android 创建成功
void larkxr_OnCreated();
// android 恢复
void larkxr_OnResume();
// android 暂停
void larkxr_OnPause();
// android 销毁
void larkxr_OnDestory();
// deivces
// 更新设置姿态数据
void larkxr_UpdateDevicePose(larkxrDeviceType deviceType, larkxrTrackedPose *pose);

// 更新设置姿态数据
// unity 调用
void larkxr_UpdateDevicePose2(larkxrDeviceType deviceType, float px, float py, float pz,
                              float rx, float ry, float rz, float rw);

// 更新手柄状态。包括按钮状态等
void larkxr_UpdateControllerInputState(larkxrControllerType controllerType,
                                       larkxrControllerInputState *controllerInputState);

// 更新手柄状态。包括按钮状态等
// unity 调用
void larkxr_UpdateControllerInput2(larkxrControllerType controllerType, bool isConnected,
                                   bool touchPadPressed, bool triggerPressed,
                                   bool digitTriggerPressed, bool appMenuPressed,
                                   bool homePressed, bool gripPressed, bool volumUpPressed,
                                   bool volumDownPressed, bool touchPadTouched);

// 更新手柄整体状态，包括姿态和按钮状态。
void larkxr_UpdateControllerState(larkxrControllerType controllerType,
                                  larkxrControllerDeviceState *controllerDeviceState);

// 更新设备整体设备状态。
void larkxr_UpdateDevicePair(larkxrDevicePair *devicePair);

// 获取当前设备姿态。一般供云雀 sdk 内部调用.
bool larkxr_GetDevicePose(larkxrDeviceType deviceType,
                          larkxrTrackedPose *pose, uint32_t ipredictMillsec);

// 获取手柄状态。一般供云雀 sdk 内部调用.
bool larkxr_GetControllerInputState(larkxrControllerType deviceType,
                                    larkxrControllerInputState *inputState);

// 发送姿态数据.
void larkxr_SendDevicePair(uint64_t frameIndex, uint64_t fetchTime, double displayTime);

// 获取是否为 3dof 或 6dof。**待完善**
bool larkxr_DeviceDegreeOfDof(larkxrDeviceType deviceType, larkxrNumDoF *dof);

// 检擦设备是否连接成功
bool larkxr_DeviceIsConnected(larkxrDeviceType deviceType);

// 重定位设备，**待完善**
void larkxr_Rencenter(larkxrDeviceType deviceType);

// 触发震动, **待完善**
void larkxr_TriggerVibrator(larkxrControllerType controllerType, unsigned short duration,
                            larkxrIntensity intensity);

// 同步触发震动, **待完善**
void larkxr_SyncTriggerVibrator(larkxrControllerType controllerType, unsigned short duration,
                                larkxrIntensity intensity); \

// 获取电池电量,状态信息。**待完善**
void larkxr_GetBatteryStatus(larkxrDeviceType deviceType, larkxrBatteryStatus *status);

// 获取电池电量百分比。
uint8_t larkxr_GetBatteryStatusNumber(larkxrDeviceType deviceType);

// 设置是否启用震动。**待完善**
void larkxr_SetEnableVibrator(bool enable);

// 异步循环.**待完善**
void larkxr_AsyncTaskLoop();
//--------------------------------------------------------------------------------------------------
// 在最终渲染时调用，用于收集延时信息。
// 在最终渲染时调用，用于收集延时信息。
void larkxr_LatencyCollectorrSubmit(uint64_t frameIndex, float blackDegree);
//--------------------------------------------------------------------------------------------------
// 用户设置
void larkxr_SetupUserSetting(larkxrUserSetting *userSeting);

// 读取所有用户设置
void larkxr_GetUserSetting(larkxrUserSetting *userSeting);

// 设置比特率
void larkxr_SetupBitrateKbps(int bitrate);

// 获取比特率
int larkxr_GetBitrateKbps();

// 获取默认比特率
int larkxr_GetDefaultBitrateKbps();

// 设置房间高度
void larkxr_SetupRoomHeight(float roomHeight);

// 获取房间高度
float larkxr_GetRoomHeight();

// 获取默认房间高度
float larkxr_GetDefaultRoomHeight();
//--------------------------------------------------------------------------------------------------
// system apis
// 检擦系统信息是否初始化完成
bool larkxr_SystemInited();

// 返回系统信息
void larkxr_GetSystemInfo(larkxrSystemInfo *info);

// 返回系统信息
// unity 调用
void larkxr_GetSystemInfo2(bool *isInited, larkxrSystemType *systemType,
                           larkxrPlatFromType *platFromType, larkxrHeadSetSDKVersion *headSetSdkVersion);

//--------------------------------------------------------------------------------------------------
// render info
// 获取默认配置
void larkxr_GetDefaultRenderInfo(larkxrRenderInfo *defaultInfo);

// 获取渲染信息。
void larkxr_RenderInfo(larkxrRenderInfo *info);

// 设置渲染信息。
// 请注意渲染的大小和视场角，如果不正确的话会导致画面的畸变。
void larkxr_InitRenderInfo(larkxrRenderInfo *renderInfo);

// 设置分辨率
void larkxr_SetRenderResolution(int renderWidth, int renderHeight);

// 获取分辨率
void larkxr_GetRenderResolution(int *renderWidth, int *renderHeight);

// 获取默认分辨率
void larkxr_GetDefaultResolution(int *renderWidth, int *renderHeight);

// 设置瞳距
void larkxr_SetIpd(float ipd);

// 获取瞳距
float larkxr_GetIpd();

// 获取默认瞳距
float larkxr_GetDefaultIpd();

// 设置渲染帧率
void larkxr_SetRenderFps(int fps);
// 获取当前渲染帧率
int larkxr_GetRenderFps();
// 默认 FPS
int larkxr_GetDefaultFps();

// 设置 fov
void larkxr_SetFov(larkxrRenderFov *fov, int cout);

// 设置 fov
// unity 用
void
larkxr_SetFov2(float eyeLeft_Left, float eyeLeft_Right, float eyeLeft_Top, float eyeLeft_Bottom,
               float eyeRight_Left, float eyeRight_Right, float eyeRight_Top,
               float eyeRight_Bottom);

void
larkxr_GetFov2(float *eyeLeft_Left, float *eyeLeft_Right, float *eyeLeft_Top, float *eyeLeft_Bottom,
               float *eyeRight_Left, float *eyeRight_Right, float *eyeRight_Top,
               float *eyeRight_Bottom);

void larkxr_GetDefaultFov2(float *eyeLeft_Left, float *eyeLeft_Right, float *eyeLeft_Top,
                           float *eyeLeft_Bottom,
                           float *eyeRight_Left, float *eyeRight_Right, float *eyeRight_Top,
                           float *eyeRight_Bottom);

//--------------------------------------------------------------------------------------------------
#if defined( __cplusplus )
}    // extern "C"
#endif

#endif //CLOUDLARKXR_XR_CLIENT_4C_H
