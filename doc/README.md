# LarkXR Client SDK

## Intro

LarkXR Client SDK is responsible for connecting to LarkXR server and cloud rendering. Mainly supports VR all-in-one machines on the Android platform. Rendered using Android OpenGL ES. Provides native C++ interfaces and DEMO integrated with VR headset SDK.

Since different brands of XR headset all-in-one development platforms have different usage logic and external interfaces, only specific DEMOs integrated with the XR headset’s native SDK are provided to ensure better results.

LarkXR Client SDK includes:

1. Conenct to LarkXR Admin server with restful API. Include applist, enter app, online/offline ext.

2. Connect to LarkXR Rendering server. Decode video stream and upload headset's pose.

With LarkXR Client SDK:

1. Get LarkXR applist.
2. User chose one cloud app from applist. LarkXR Client SDK call enter app API. LarkXR Admin server allocates rendering nodes and returns relevant connection parameters.
3. LarkXR Client SDK login to LarkXR Rendering server and start streaming threads.
4. Connect to LarkXR Rendering server success. LarkXR Client SDK upload loacl headset pose and decode cloud stream.
5. User close and release resources.

![CloudAPP INPUT VR SDK OUPUT](./images/cloudapp_vrsdk.gif)

> Left: CloudAPP INPUT Right: SDK output

![vr headset](./images/vrheadset.gif)

> Simulation of VR headset output

## Folder:

```path
cmake-----------------------------cmake config
doc-------------------------------
lib_pxygl-------------------------Support VR render opengl api.
lib_xr_common_ui------------------APP common ui for all three demo.
xr_app_htc------------------------HTC focus headset.
xr_app_huawei---------------------Huawei VR headset
xr_app_oculus---------------------Meta Oculus Quest headset(Sys < 46.0, old meta native sdk)
xr_app_openxr_oculus--------------Meta Oculus Quest headset(new meta openxr sdk)
xr_app_openxr_pico----------------Pico4/Pico Neo3 headset
third_party-----------------------
```

### SDK Interface

## Java (Called in android activity for init andoird resource and folder)

### XrSystem

XrSystem.java is main Java class.

* init. Should be called during ativity initialization, as Activity onCreate.

```java
xrSystem = new XrSystem();
xrSystem.init(this);
```

* life cycle functions: onResume，onPause，onDestro

```java
@Override
protected void onResume() {
    super.onResume();
    Log.d(TAG, "java activity onResume");
    xrSystem.onResume();
}
@Override
protected void onPause() {
    super.onPause();
    Log.d(TAG, "java activity onPause");
    xrSystem.onPause();
}

@Override
protected void onDestroy() {
    super.onDestroy();
    Log.d(TAG, "java activity onDestroy");
    xrSystem.onDestroy();
}
```

## c++ interface (for cloud rendering)

### xr_client.h

#### main interfaces

XRClient is larkxr sdk main class，include init sdk, sdk life cycle, enter cloud app, send device pose, get cloud textures.

* Init


```c++
/**
* init LarkXR SDK
* @param vm java vm
* @param init_share_context init with opengl context。
* If opengl context not ready when call init should be false and call InitGLShareContext when opengl context ready
*/
void Init(JavaVM* vm, bool init_share_context = true);
/**
* Init openg gl conetxt for LarkXR SDK
* Must called in opengl thread.
*/
void InitGLShareContext();
/**
* 
*/
void ReleaseGLShareContext();
```

Setup LarkXR SDK ID

```c++
/**
 * Setup LarkXR SDK ID
 * @param id SDK ID
 * @return
 */
bool InitSdkAuthorization(const std::string& id);
```

> c++ Init shoud after Activity call  xrSystem.init.


* Android Life cycle call.

```c++
// When system created. If already have admin server address will online in LarkXR Admin server.
void OnCreated();
// When system resume. If already have admin server address will online in LarkXR Admin server.
void OnResume();
// When system pause. If already have admin server address will offline in LarkXR Admin server.
void OnPause();
// When system destory. If already have admin server address will offline in LarkXR Admin server.
void OnDestory();
```

* Setup LarkXR Admin server address

```c++
/**
 *  Setup LarkXR Admin server address
 * @param ip LarkXR Admind server ip
 * @param port LarkXR Admind server port
 */
void SetServerAddr(const std::string& ip, int port);
```

* Set certificate

```c++
/**
 *  Setup client certificate. Get certificate in LarkXR Admin server when LarkXR Admin request certificate to get applist.
 * @param appKey admin_key
 * @param appSecret admin_secret
 */
static void SetCertificate(const std::string& appKey, const std::string& appSecret);
```

* Setup headset battery level. 0-100.

```c++
/**
* Setup headset battery level. 0-100.
* @param level
*/
static void SetHmdBatteryLevel(int level);
/**
* Setup headset controller battery level. 0-100.
* @param left
* @param right
*/
static void SetControlerBatteryLevel(int left, int right);
```

* Regeister callback

```c++
/**
* Regeister callback
* @param observer
*/
void RegisterObserver(XRClientObserver* observer);
/**
* 
*/
void UnRegisterObserver();
```

* Enter/Close cloud app.

```c++
/**
 * Enter cloud app with  cloud app id from applist.
 * @param appliId cloud app id
 */
void EnterAppli(const std::string& appliId);
/**
 * Connect cloud server with specific params.
 * @param config
 */
void Connect(const CommonConfig& config);
/**
 * Close connection
 */
void Close();
```

* Upload device pose.

Device pose incloude hmd pose and controller pose in `larkxrTrackingDevicePairFrame` struct.

Could be call `RequestTrackingInfo` in callback.

```c++
/**
* Immediately send device pose to cloud.
* @param devicePair deivce pose info
*/
void SendDevicePair(const larkxrTrackingDevicePairFrame& devicePair);
/**
* Send last device pose.
*/
void SendDevicePair();
```

* Cloud Rendering

```c++
/**
 * Check new frame come.
 * @return
 */
bool HasNewFrame();
/**
 * Get cloud tracking frame. Return success when new frame come.
 * A tracking frame include texture for render and hmd pose for atw.
 * @param trackingFrame trakcing info
 * @return is success
 */
bool Render(larkxrTrackingFrame* trackingFrame);
```

* Error message:

```c++
/**
 *  error code in larkEventTypes enum
 * @return error code
 */
int last_error_code();
/**
 * last error message
 * @return error message
 */
std::string last_error_message();
/**
 * Clear error message.
 */
void ClearError();
```

* Data channel send data to cloud app. Cloud app should use LarkXR datachannel plugin.

```c++
/**
    * Send binary to cloud app.
    * @param buffer
    * @param length
    */
void SendData(const char* buffer, int length);
/**
    * Send text to cloud app.
    * @param buffer
    * @param length
    */
void SendData(const std::string& data);
```

* Send audio data to cloud.

```c++
/**
* Send audio data to cloud.
* @param buffer
* @param length
*/
void SendAudioData(const char* buffer, int length);
```

* Callbacks for connection:

```c++
/**
* Login to render server success.
*/
virtual void OnConnected() = 0;
/**
* Connection closeed
* @param code Reason @see larkEventTypes. 
*             LK_XR_UDP_CHANNEL_USER_CANCEL 
*             LK_PROXY_SERVER_CLOSE 
*             LK_RENDER_SERVER_CLOSE
*             LK_XR_UDP_CHANNEL_CLOSED
*/
virtual void OnClose(int code) = 0;
/**
* When streaming channel close. Streaming channel will be closed when headset paused and can be resume when headset light up.
*/
virtual void OnStreamingDisconnect() = 0;
```

* Callbacks for info:

```c++
/**
* Info like login success.
* @param infoCode  @see larkEventTypes
* @param msg message maybe empty.
*/
virtual void OnInfo(int infoCode, const std::string& msg) = 0;
/**
* Error message callback
* @param errCode @see larkEventTypes
* @param msg message maybe empty.
*/
virtual void OnError(int errCode, const std::string& msg) = 0;
```

* Decode and rendering callback:

```c++
/**
* When soft decode success.
*/
virtual void OnMediaReady() = 0;
/**
* HW decode success.
* Return opengl GL_TEXTURE_2D VR texture 
* --------------
* |  L   |  R  |
* --------------
* Texture update by larkxr sdk.
* Call once when first frame decoded.
* Render this texture when have tracking frame for atw.
* @param nativeTextrure opengl texture id
*/
virtual void OnMediaReady(int nativeTextrure) = 0;
/**
* When soft decode a new frame success.
* Include tracking pose and video frame.
* @param frame tracking
*/
virtual void OnTrackingFrame(std::unique_ptr<XRTrackingFrame>&& frame) = 0;
/**
* HW decode a new frame success.
* Tracking data include device pose for cloud rendering. Use this pose for local headset atw render.
* @param trackingFrame tracking
*/
virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) = 0;
/**
* LarkXR call RequestTrackingInfo with fixed frequency.
*/
virtual void RequestTrackingInfo() = 0;
```

* LarkXR Admin server client Id

```c++
/**
* Receive LarkXR Admin server client id.
* @param clientId
*/
virtual void OnClientId(const std::string& clientId) = 0;
```

* Other callbacks

```c++
/**
 * Receive Haptics Feedback from cloud app.
 * @param isLeft is left controller
 * @param startTime
 * @param amplitude
 * @param duration
 * @param frequency
 */
virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) = 0;
/**
* Cloud app request sync player space.
*/
virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) = 0;
/**
* Data channel open. Can send data to cloud app when datachannel opened.
*/
virtual void OnDataChannelOpen() = 0;
/**
* Data channel close.
*/
virtual void OnDataChannelClose() = 0;
/**
* Receive binary data from cloud app.
*/
virtual void OnDataChannelData(const char* buffer, int length) = 0;
/**
* Receive text data from cloud app.
*/
virtual void OnDataChannelData(const std::string& data) = 0;
```

### Other

#### Get applist and runmode.

c++ AppListTask class to get applist and runmode. Receive new data from callbacks.

* Callback

```c++
// Applist info
virtual void OnAppListInfo(const std::vector<AppliInfo>& appliInfo) = 0;
// Receive applist info with page info.
virtual void OnAppListPageInfo(const AppliPageInfo& appliPageInfo) = 0;
// When request failed.
virtual void OnFailed(const std::string& msg) = 0;
// Runmode
// When client run center controler mode, admin server want client without applist and wait for start app request.
virtual void OnRunMode(GetVrClientRunMode::ClientRunMode runMode) = 0;
```

#### SDK Config

XRConfig class include stream config.

```c++
// Original render width with both eye.
static int render_width;
// Original render height.
static int render_height;
// Scale resolution 0 - 2
// Final resolution align32(render_width * resolution_scale); align32(render_height * resolution_scale)
static float resolution_scale;
// Bitrate kbps.
static int bitrate;
// Frame rate.
static int fps;
// @Deprecated 
static float room_height;
// Interpupillary distance
static float ipd;
// For for both eyes in degree.
static larkxrRenderFov fov[2];
// @Deprecated 
static bool use_kcp;
// use h265 encode/decode.
static bool use_h265;
// use haptics feedback
static bool use_haptics_feedback;
// callback both eye to one texture
static bool use_multiview;
// Flip draw cloud texture upsided down.
static bool flip_draw;
// Default 0.005 (3.1.8.0 added)
static float seconds_from_vsync_to_photons;
// Endable 10bit encode (3.1.8.0 added)
static bool use_10bit_encoder;
// @Deprecated 
static bool report_fec_failed;
// Headset type (3.1.8.0 added)
static larkHeadSetControllerDesc headset_desc;
// Enable fov rendering  (3.1.8.0 add)
static larkFoveatedRendering foveated_rendering;
// Enable color correction (3.1.8.0 add)
static larkColorCorrention color_corrention;
// Stream type udp，tcp, kcp (3.1.8.0 add)
static larkStreamType stream_type;
// Use render queue (3.1.8.0 add)
static bool use_render_queue;
static QuickConfigLevel quick_config_level;
```

#### Latency data

```c++
// Render specific frameIndex frome in opengl。
void Rendered2(uint64_t frameIndex);
// Submit specific frameIndex to headset SDK. blackDegree is difference cloud pose and device local pose in yaw.
void Submit(uint64_t frameIndex, float blackDegree);
```

