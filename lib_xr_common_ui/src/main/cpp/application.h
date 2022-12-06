//
// Created by fcx on 2020/5/6.
//

#ifndef CLOUDLARKXR_APPLICATION_H
#define CLOUDLARKXR_APPLICATION_H

#include <android_native_app_glue.h>
#include "oboe/Oboe.h"
#include "lark_xr/xr_client.h"

//#define LARK_SDK_ID "请配置SDK ID. 如果没有请联系商务获取. https://www.pingxingyun.com/index.html"

#ifndef LARK_SDK_ID
#error "请配置SDK ID. 如果没有请联系商务获取. https://www.pingxingyun.com/index.html"
#endif

//
// application interface.
//
class Application: public lark::XRClientObserverWrap, public oboe::AudioStreamDataCallback {
public:
    enum ApplicationUIMode {
        ApplicationUIMode_Android_2D  = 0,
        ApplicationUIMode_Opengles_3D = 1,
    };
    // 云渲染使用的空间
    enum Space {
        // local space -> 使用固定的地面高度和初始位置原点
        Space_Local = 0,
        // app space -> 使用app设置的地面高度和位置原点
        Space_App   = 1,
    };

    // warning should init in child class
    static Application* instance();
    virtual ~Application();

    //
    virtual bool InitVR(android_app* app) { return false; };
    virtual bool InitVR() { return false; };
    virtual bool InitGL() { return false; };
    virtual void InitJava() {};
    virtual void ShutdownVR() {};
    virtual void ShutdownGL() {};

    //  callback from android native activity.
    virtual void HandleVrModeChange() {};
    virtual bool OnUpdate() { return false; };

    // call by ui
    // 进入应用
    virtual void EnterAppli(const std::string& appId);
    // 可选区域id等参数进入应用
    virtual void EnterAppliParams(const lark::EnterAppliParams& params);
    // 关闭应用
    virtual void CloseAppli();
    // ui 设置帧率
    virtual void SetupFPS(int fps) {};
    // ui 设置云渲染使用的空间
    // app space -> 使用app设置的地面高度和位置原点
    // local space -> 使用固定的地面高度和初始位置原点
    virtual void SetupSapce(Space space) {};
    // ui 设置天空盒, 更新场景中的天空盒
    virtual void SetupSkyBox(int index) {};

    // xr client callback
    virtual void OnSDKAuthorizationFailed(int code, const std::string& msg) override {};
    virtual void OnCloudXRReady(const std::string& appServerIp, const std::string& perferOutIp) override {};
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void OnStreamingDisconnect() override {};
    virtual void OnInfo(int infoCode, const std::string& msg) override;
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnDataChannelOpen() override {};
    virtual void OnDataChannelClose() override {};
    virtual void OnDataChannelData(const char* buffer, int length) override {};
    virtual void OnDataChannelData(const std::string& data) override {};
    // soft decoder
    virtual void OnMediaReady() override  {};
    // hw decoder callback textrue.
    virtual void OnMediaReady(int nativeTextrure) override {};
    virtual void OnMediaReady(int nativeTextrureLeft, int nativeTextureRight) override {};
    virtual void OnTrackingFrame(std::unique_ptr<lark::XRTrackingFrame>&& frame) override {};
    // hw decoder callback without textrue.
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override {};
    virtual void RequestTrackingInfo() override {};
    virtual void OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration, float frequency) override {};
    // request player space
    virtual void OnSyncPlayerSpace(larkxrPlaySpace* playSpace) override {};
    virtual void OnClientId(const std::string& clientId) override;
    // update server 3.2.5.0
    virtual void RequestAudioInput() override;
    // xr client callback end

    // AudioStreamDataCallback interface
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames) override;

    //
    // setup server addr
    virtual void SetServerAddr(const std::string& ip, uint16_t port);
    // handle network change
    virtual void OnNetworkAvailable() {};
    virtual void OnNetworkLost() {};

    virtual void Quit3DUI() {}

    inline void set_ui_mode(ApplicationUIMode ui_mode) { ui_mode_ = ui_mode; }
    inline ApplicationUIMode ui_mode() { return ui_mode_; }

    inline void set_appli_id_from_2d_ui(const std::string& appid) { appli_id_from_2d_ui_ = appid; }
    inline std::string  appli_id_from_2d_ui() { return appli_id_from_2d_ui_; }

    inline void Set2DUIEnterAppliMode(const std::string& appid) {
        appli_id_from_2d_ui_ = appid;
        ui_mode_ = ApplicationUIMode_Android_2D;
    }
protected:
    static void RegiseredInstance(Application* instance);
    static void UnRegiseredInstance();

    Application();
    virtual void HandleInput() {};
    // 初始化客户端接入凭证
    // android Context 创建成功之后调用
    // 从文件中读取 appkey 和 secret
    // 优先从 sd 卡读取
    // 【外部路径】or【内部路径】/data/data/APPID/larkxr/certificate_appkey.txt
    // 【外部路径】or【内部路径】/data/data/APPID/larkxr/certificate_appsecret.txt
    void InitCertificate();

    // xrclient
    std::shared_ptr<lark::XRClient> xr_client_ = nullptr;

    //
    bool connected_ = false;

    ApplicationUIMode ui_mode_ = ApplicationUIMode_Opengles_3D;
    std::string appli_id_from_2d_ui_ = "";

    std::shared_ptr<oboe::AudioStream> recording_stream_{};
private:
    // static instance
    // WARNING should init in child class
    static Application* s_instance_;
};


#endif //CLOUDLARKXR_APPLICATION_H
