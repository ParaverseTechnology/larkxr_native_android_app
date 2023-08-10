//
// Created by fcx@pingxingyun.com on 2019/10/23.
//

#include <VrApi_Ext.h>
#include <sys/prctl.h>
#include <VrApi.h>
#include <VrApi_Helpers.h>
#include <EGL/egl.h>
#include <unistd.h>
#include <asset_files.h>
#include "ovr_application.h"
#include "log.h"
#include "egl_utils.h"
#include "utils.h"
#include "env_context.h"
#include "utils.h"
#include "ovr_utils.h"
#include <lark_xr/xr_config.h>

#define LOG_TAG "ovr_native_application"

#define USE_RENDER_QUEUE = 1;

OvrApplication::OvrApplication():
    ovr_egl_(nullptr),
    scene_local_(nullptr),
    scene_cloud_(nullptr)
{
    RegiseredInstance(this);
    LOGENTRY();
}

OvrApplication::~OvrApplication() {
    UnRegiseredInstance();
    LOGENTRY();
};

bool OvrApplication::InitVR(android_app *app) {
    LOGENTRY();
    java_.Vm = app->activity->vm;
    java_.Env = app->activity->env;
    // TODO
    // WARING jin verions dif AttachCurrentThread param different.
    java_.Vm->AttachCurrentThread(&java_.Env, nullptr);
    java_.ActivityObject = app->activity->clazz;
    // Note that AttachCurrentThread will reset the thread name.
    prctl( PR_SET_NAME, (long)"OVR::Main", 0, 0, 0 );
    LOGE("ovrInitParms");
    const ovrInitParms initParms = vrapi_DefaultInitParms( &java_ );
    int32_t initResult = vrapi_Initialize( &initParms );

    if ( initResult != VRAPI_INITIALIZE_SUCCESS )
    {
        // If intialization failed, vrapi_* function calls will not be available.
        return false;
    }
    LOGE("初始化环境");
    // 初始化环境。
    Context::Init(app->activity);
    // 初始化客户端接入凭证
    LOGE("初始化客户端接入凭证");
    InitCertificate();
    return true;
}

bool OvrApplication::InitGL() {
    LOGENTRY();

    ovr_egl_ = std::make_shared<OvrEgl>();
    bool res = ovr_egl_->createContext();

    // 开启透明同道混合
    GL( glEnable(GL_BLEND) );
    GL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
//
    GL( glEnable(GL_DEPTH_TEST) );
    GL( glDepthFunc(GL_LEQUAL) );
    GL( glDepthMask(true) );

    // load assets.
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::AssetLoader::instance()->Load(&context, Assetlist);

    scene_local_ = std::make_shared<OvrSceneLocal>();
    scene_cloud_ = std::make_shared<OvrSceneCloud>();

    GL( glCullFace(GL_BACK) );
    GL( glEnable(GL_CULL_FACE) );
    GL( glFrontFace(GL_CCW) );

    return res;
}

void OvrApplication::InitJava() {
    LOGENTRY();
    use_multiview_ &= (ovr_egl_->getMultiView() &&
                       vrapi_GetSystemPropertyInt(&java_, VRAPI_SYS_PROP_MULTIVIEW_AVAILABLE));

    LOGV( "AppState UseMultiview : %d", use_multiview_);

    cpu_level = CPU_LEVEL;
    gpu_level_ = GPU_LEVEL;
    main_thread_tid_ = gettid();
    CreateFrameBuffer(&java_, use_multiview_);
//    scene_local_->InitJava(&java_, use_multiview_);
//    scene_cloud_->InitJava(&java_, use_multiview_);

#ifdef ENABLE_CLOUDXR
    // create cloudxr before scene cloud inited
    cloudxr_client_ = std::make_shared<CloudXRClient>(this);
    scene_cloud_->SetCloudXRClient(cloudxr_client_);
#endif

    scene_local_->InitGL(frame_buffer_, num_buffers_);

    scene_cloud_->InitGL(frame_buffer_, num_buffers_);
}

void OvrApplication::ShutdownVR() {
    LOGENTRY();
    // 清理环境
    Context::Reset();

    vrapi_Shutdown();
    if (java_.Vm) {
        java_.Vm->DetachCurrentThread();
    }
}

void OvrApplication::ShutdownGL() {
    LOGENTRY();

    connected_ = false;

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
    }
#endif

    if (recording_stream_) {
        recording_stream_->close();
        recording_stream_.reset();
    }

    // release cloudlark
    if (xr_client_) {
        xr_client_->UnRegisterObserver();
        xr_client_->Release();
        xr_client_.reset();
    }

    // reset all state.
    Input::ResetInput();
   // Navigation::ClearToastEnterAppli();

    scene_local_->ShutdownGL();
    scene_local_.reset();
    scene_cloud_->ShutdownGL();
    scene_cloud_.reset();
    lark::AssetLoader::Release();

    DestoryFrameBuffer();
    ovr_egl_->destoryContext();
}

void OvrApplication::HandleVrModeChange() {
    LOGENTRY();
//    utils::MutexLock mutexLock(mLock);
    if (resumed_ && native_window_ != nullptr) {
        if (ovr_ == nullptr) {
            ovrModeParms parms = vrapi_DefaultModeParms( &java_ );
            // No need to reset the FLAG_FULLSCREEN window flag when using a View
            parms.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;

            parms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
            parms.Display = (size_t)ovr_egl_->getDisplay();
            parms.WindowSurface = (size_t)native_window_;
            parms.ShareContext = (size_t)ovr_egl_->getContext();
            LOGV( "eglGetCurrentSurface( EGL_DRAW ) = %p", eglGetCurrentSurface( EGL_DRAW ) );
            LOGV( "vrapi_EnterVrMode()" );
            ovr_ = vrapi_EnterVrMode(&parms);
            LOGV( "eglGetCurrentSurface( EGL_DRAW ) = %p", eglGetCurrentSurface( EGL_DRAW ) );

            // If entering VR mode failed then the ANativeWindow was not valid.
            if (ovr_ == nullptr)
            {
                LOGE( "Invalid ANativeWindow!" );
                native_window_ = nullptr;
            }

            // Set performance parameters once we have entered VR mode and have a valid ovrMobile.
            if ( ovr_ != nullptr )
            {
                // setup Guardian
                //                vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_STAGE);

                vrapi_SetClockLevels( ovr_, cpu_level, gpu_level_ );

                LOGV( "		vrapi_SetClockLevels( %d, %d )", cpu_level, gpu_level_ );

                vrapi_SetPerfThread( ovr_, VRAPI_PERF_THREAD_TYPE_MAIN, main_thread_tid_ );

                LOGV( "		vrapi_SetPerfThread( MAIN, %d )", main_thread_tid_ );

                vrapi_SetPerfThread( ovr_, VRAPI_PERF_THREAD_TYPE_RENDERER, render_thread_tid_ );

                LOGV( "		vrapi_SetPerfThread( RENDERER, %d )", render_thread_tid_ );

                if (!xr_client_) {
                    // oculus info.
                    /*    int renderWidth = vrapi_GetSystemPropertyInt(&java_,
                                                                 VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);*/
                    //    renderWidth *= 2;

                    int renderWidth = vrapi_GetSystemPropertyInt(&java_,
                                                                 VRAPI_SYS_PROP_DISPLAY_PIXELS_WIDE);
                    /*    int renderHeight = vrapi_GetSystemPropertyInt(&java_,
                                                                      VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);*/
                    int renderHeight = vrapi_GetSystemPropertyInt(&java_,
                                                                  VRAPI_SYS_PROP_DISPLAY_PIXELS_HIGH);
                    //
                    // config
                    //
                    // setup render size.
                    lark::XRConfig::render_width = renderWidth;
                    lark::XRConfig::render_height = renderHeight;

                    //        lark::XRConfig::render_width = 2432;
                    //        lark::XRConfig::render_height = 1344;
                    //
                    //        lark::XRConfig::render_width = 2880;
                    //        lark::XRConfig::render_height = 1600;

                    //        lark::XRConfig::render_width = 2880;
                    //        lark::XRConfig::render_height = 1440;

                    //        lark::XRConfig::render_width = 1920;
                    //        lark::XRConfig::render_height = 1080;

                    //        lark::XRConfig::render_width = 2560;
                    //        lark::XRConfig::render_height = 1440;

                    //    lark::XRConfig::render_width = 2048;
                    //    lark::XRConfig::render_height = 1024;

                    // frame rate
                    //        lark::XRConfig::fps = 75;
                    lark::XRConfig::fps = 72;
                    //    lark::XRConfig::fps = 60;

                    // test disable multivew;
                    lark::XRConfig::use_multiview = true;
                    //    lark::XRConfig::foveated_rendering.enableFoveateRendering = true;

//                    lark::XRConfig::headset_desc.type = larkHeadSetType_HTC;
                    lark::XRConfig::headset_desc.type = larkHeadSetType_OCULUS;

//                    lark::XRConfig::use_multiview = false;
//                    lark::XRConfig::flip_draw = true;
#ifdef USE_RENDER_QUEUE
                    lark::XRConfig::use_render_queue = true;
#else
                    lark::XRConfig::use_render_queue = false;
#endif
                    int ovrDeviceType = vrapi_GetSystemPropertyInt(&java_, VRAPI_SYS_PROP_DEVICE_TYPE);
                    LOGV("system info. width: %d; height: %d; deviceType: %d", renderWidth, renderHeight, ovrDeviceType);

                    JavaVM* vm = nullptr;
                    java_.Env->GetJavaVM(&vm);

                    xr_client_ = std::make_shared<lark::XRClient>();
                    xr_client_->Init(java_.Vm);
                    xr_client_->RegisterObserver(this);
                    xr_client_->EnableDebugMode(true);

                    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
                        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
                        Navigation::ShowToast(xr_client_->last_error_message());
                    }

                    LOGV("pre appid ========== %s", appli_id_from_2d_ui_.c_str());
                    if (!appli_id_from_2d_ui_.empty()) {
                        EnterAppli(appli_id_from_2d_ui_);
                    }
                }
                // setup fov
                {
                    double displayTime = vrapi_GetPredictedDisplayTime(ovr_, 0);
                    ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovr_, displayTime);

                    for (int eye = 0; eye < 2; eye++) {
                        auto projection = tracking.Eye[eye].ProjectionMatrix;
                        double a = projection.M[0][0];
                        double b = projection.M[1][1];
                        double c = projection.M[0][2];
                        double d = projection.M[1][2];
                        double n = -projection.M[2][3];
                        double w1 = 2.0F * n / a;
                        double h1 = 2.0F * n / b;
                        double w2 = c * w1;
                        double h2 = d * h1;

                        double maxX = (w1 + w2) / 2.0F;
                        double minX = w2 - maxX;
                        double maxY = (h1 + h2) / 2.0F;
                        double minY = h2 - maxY;

                        double rr = 180 / M_PI;

                        LOGI("getFov maxX=%f minX=%f maxY=%f minY=%f a=%f b=%f c=%f d=%f n=%f", maxX, minX, maxY,
                             minY, a, b, c, d, n);
                        float left = atan(minX / -n) * rr; // left (minX)
                        float right = -atan(maxX / -n) * rr; // right (maxX)
                        float top = atan(minY / -n) * rr; // top (minY)
                        float bottom = -atan(maxY / -n) * rr; // bottom (maxY)

                        // config.
                        lark::XRConfig::fov[eye] = {
                                left, right, top, bottom
                        };
                        LOGI("getFov[%d](D) l=%f r=%f t=%f b=%f", eye, left, right, top, bottom);
                    }
                }
                // latency mode
                vrapi_SetExtraLatencyMode(ovr_, ovrExtraLatencyMode::VRAPI_EXTRA_LATENCY_MODE_ON);
                // frame rate.
//                ovrResult result = vrapi_SetDisplayRefreshRate(ovr_, 90.0f);
//                LOGI("set fps result %d %d %d", (int)result, result == ovrError_InvalidOperation,
//                     result == ovrError_InvalidParameter);
//                // setup frame rate.
//                lark::XRConfig::fps = 90;
                int frameRate = vrapi_GetSystemPropertyInt(&java_,
                                                             VRAPI_SYS_PROP_DISPLAY_REFRESH_RATE);
                int supportFrameRateNum = vrapi_GetSystemPropertyInt(&java_,
                                                                  VRAPI_SYS_PROP_NUM_SUPPORTED_DISPLAY_REFRESH_RATES);
                float rates[5];
                int size = vrapi_GetSystemPropertyFloatArray(&java_, VRAPI_SYS_PROP_SUPPORTED_DISPLAY_REFRESH_RATES,
                                                             rates, 5);
                LOGI("frame rate %d %d %d", frameRate, supportFrameRateNum, size);
                for (int i = 0; i < size; i ++) {
                    LOGI("support frame rate %f", rates[i]);
                }
                // tracking space
//                tracking_space_ = vrapi_GetTrackingSpace(ovr_);
//                tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL_FLOOR;
                tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL;
//                tracking_space_ = VRAPI_TRACKING_SPACE_STAGE;
                vrapi_SetTrackingSpace(ovr_, tracking_space_);
                LOGI("tracking space %d", tracking_space_);
                // setup ipd
                ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovr_, 0.0);
                float ipd = vrapi_GetInterpupillaryDistance(&tracking);
                lark::XRConfig::set_ipd(ipd);
                LOGI("current ipd %f", ipd);
//                lark::XRConfig::foveated_rendering.enableFoveateRendering = true;
            }

#ifdef ENABLE_CLOUDXR
            if (cloudxr_client_ && !cloudxr_client_->inited()) {
                cloudxr_client_->InitRenderParamsWithLarkXRConfig();
                cloudxr_client_->Init();
            }
#endif
        } else {
            LOGV("VR mode change");
        }
    } else {
        if (ovr_ != nullptr) {
            LOGV( "eglGetCurrentSurface( EGL_DRAW ) = %p", eglGetCurrentSurface( EGL_DRAW ) );
            LOGV( "vrapi_LeaveVrMode()" );
            vrapi_LeaveVrMode(ovr_);
            ovr_ = nullptr;
            LOGV( "eglGetCurrentSurface( EGL_DRAW ) = %p", eglGetCurrentSurface( EGL_DRAW ) );
        }
    }
}

void OvrApplication::OnResume() {
    LOGV( "onResume()" );
    LOGV( "    APP_CMD_RESUME" );
    resumed_ = true;
    if (xr_client_) {
        sleep(1);
        xr_client_->OnResume();
    }
}

void OvrApplication::OnPause() {
    LOGV( "onPause()" );
    LOGV( "    APP_CMD_PAUSE" );
    resumed_ = false;
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

void OvrApplication::OnInitWindow(ANativeWindow * window) {
    LOGV( "surfaceCreated()" );
    LOGV( "    APP_CMD_INIT_WINDOW" );
    native_window_ = window;
    if (xr_client_) {
        xr_client_->OnCreated();
    }
}

void OvrApplication::OnDestory() {
    LOGV( "onDestroy()" );
    LOGV( "    APP_CMD_DESTROY" );
    native_window_ = nullptr;
    if (xr_client_) {
        xr_client_->OnDestory();
    }
}

bool OvrApplication::OnUpdate() {
    if (ovr_ == nullptr) {
        return false;
    }
#ifdef ENABLE_CLOUDXR
    if (need_recreat_cloudxr_client_) {
        cloudxr_client_->Init();
        need_recreat_cloudxr_client_ = false;
    }

    if (need_reconnect_public_ip_ && cloudxr_client_) {
        // recreate recevier
        cloudxr_client_->Init();
        // try to connect to public ip.
        cloudxr_client_->Connect(prepare_public_ip_);

        need_reconnect_public_ip_ = false;
        prepare_public_ip_ = "";
    }
    // cloudxr progess
    if (cloudxr_client_ && cloudxr_client_->IsConnect()) {
        // update input.
        scene_cloud_->HandleInput();

        cxrFramesLatched latched;
        cxrError error = cloudxr_client_->Latch(latched);
        if (error != cxrError_Success)
        {
            LOGV("Latching frame failed.");
            if (error == cxrError_Frame_Not_Ready)
            {
                LOGW("LatchFrame failed, frame not ready for %d ms", 150);
            }
            else
            {
                LOGE("Error in LatchFrame [%0d] = %s", error, cxrErrorString(error));
            }
            return false;
        }

        scene_cloud_->Render(ovr_, latched);

        cloudxr_client_->Release();
        cloudxr_client_->Stats();

        egl_utils::GLCheckErrors(0);
        return true;
    }
#endif

    if (connected_) {
#ifdef USE_RENDER_QUEUE
        larkxrTrackingFrame trackingFrame;
        lark::XRVideoFrame xrVideoFrame(0);
        if (xr_client_->Render(&trackingFrame, &xrVideoFrame)) {
            scene_cloud_->HandleInput();
            scene_cloud_->Render(ovr_, trackingFrame, xrVideoFrame);
            xr_client_->ReleaseRenderTexture();
        } else {
            if (!xr_client_->media_ready()) {
                scene_cloud_->Update(ovr_);
            } else {
                scene_cloud_->HandleInput();
                usleep(1000);
            }
        }
#else
        // clear old buffer.
        if (xr_client_->media_ready()) {
            scene_cloud_->HandleInput();
            scene_cloud_->UpdateAsync(ovr_);
            larkxrTrackingFrame trackingFrame;
            if (xr_client_->Render(&trackingFrame)) {
                scene_cloud_->Render(ovr_, trackingFrame);
            } else {
                usleep(1000);
            }
        } else {
            scene_cloud_->Update(ovr_);
        }
#endif
    } else {
        scene_local_->Update(ovr_);
    }
    return true;
}

void OvrApplication::OnConnected() {
    LOGENTRY();
    Application::OnConnected();
    connected_ = true;
    scene_cloud_->OnConnect();
}

void OvrApplication::OnClose(int code) {
    LOGV("=========on close %d", code);
    Application::OnClose(code);

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
        need_recreat_cloudxr_client_ = true;
    }
#endif

    connected_ = false;
    scene_cloud_->OnClose();

    std::string msg = "";
    switch(code) {
        case LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED:
            msg = "与服务器媒体传输通道连接关闭";
            break;
        case LK_RENDER_SERVER_CLOSE:
            msg = "与渲染服务器 WebSocket 连接关闭";
            break;
        case LK_PROXY_SERVER_CLOSE:
            msg = "与渲染服务器代理连接关闭";
            break;
    }
    Navigation::ShowToast(msg);

    if (ui_mode() == ApplicationUIMode_Opengles_3D) {
        scene_local_->HomePage();
    } else {
        scene_local_->LoadingPage();
        JniCallbackOnError(code, msg);
    }

    if (ovr_ != nullptr && tracking_space_ != VRAPI_TRACKING_SPACE_LOCAL) {
        vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL);
        tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL;
    }
}

void OvrApplication::OnError(int errCode, const char* msg) {
    Application::OnError(errCode, msg);
    LOGE("on xr client error %d; msg %s;", errCode, msg);

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_ && cloudxr_client_->IsConnectStarted()) {
        cloudxr_client_->Teardown();
        need_recreat_cloudxr_client_ = true;
    }
#endif

    Navigation::ShowToast(msg);
    if (errCode == LK_API_ENTERAPPLI_FAILED) {
        // enter applifailed.
        if (ui_mode() == ApplicationUIMode_Opengles_3D) {
            scene_local_->HomePage();
        } else {
            JniCallbackOnError(errCode, msg);
            scene_local_->LoadingPage();
        }
    } else {
        connected_ = false;
        scene_cloud_->OnClose();
        if (ui_mode() == ApplicationUIMode_Opengles_3D) {
            scene_local_->HomePage();
        } else{
            JniCallbackOnError(errCode, msg);
            scene_local_->LoadingPage();
        }

    }
    if (ovr_ != nullptr && tracking_space_ != VRAPI_TRACKING_SPACE_LOCAL) {
        vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL);
        tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL;
    }
}

void OvrApplication::OnMediaReady(int nativeTextrure) {
    LOGENTRY();
    if (ovr_ != nullptr && tracking_space_ != VRAPI_TRACKING_SPACE_LOCAL_FLOOR) {
        vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);
        tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL_FLOOR;
    }
    scene_cloud_->OnMediaReady(nativeTextrure);
}

void OvrApplication::OnMediaReady(int nativeTextureLeft, int nativeTextureRight) {
    LOGENTRY();
    if (ovr_ != nullptr && tracking_space_ != VRAPI_TRACKING_SPACE_LOCAL_FLOOR) {
        vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);
        tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL_FLOOR;
    }
    LOGV("on stereo media ready %d %d", nativeTextureLeft, nativeTextureRight);
    scene_cloud_->OnMediaReady(nativeTextureLeft, nativeTextureRight);
}

void OvrApplication::OnMediaReady() {
    if (ovr_ != nullptr && tracking_space_ != VRAPI_TRACKING_SPACE_LOCAL_FLOOR) {
        vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);
        tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL_FLOOR;
    }
    scene_cloud_->OnMediaReady();
}

void OvrApplication::RequestTrackingInfo() {
    // skip when ovr not ready.
    if (ovr_ == nullptr) {
        return;
    }

//    LOGV("RequestTrackingInfo");
    // send device pair
//    if (!xr_client_->media_ready()) {
//         use async tracking info.
        scene_cloud_->UpdateAsync(ovr_);
//    }

    xr_client_->SendDevicePair(scene_cloud_->device_pair_frame());
}

void
OvrApplication::OnHapticsFeedback(bool isLeft, uint64_t startTime, float amplitude, float duration,
                                  float frequency) {

    LOGENTRY();
    Application::OnHapticsFeedback(isLeft, startTime, amplitude, duration, frequency);

    ovr::TriggerHaptic(ovr_, isLeft, startTime, amplitude, duration, frequency);
}

void OvrApplication::OnNetworkAvailable() {
    Application::OnNetworkAvailable();
    if (xr_client_) {
        xr_client_->OnResume();
    }
}

void OvrApplication::OnNetworkLost() {
    Application::OnNetworkLost();
    Navigation::ShowToast("检测到系统网络断开，请检查网络");
    if (xr_client_) {
        xr_client_->OnPause();
    }
}

void OvrApplication::OnSyncPlayerSpace(larkxrPlaySpace *playSpace) {
    if (ovr_ == nullptr) {
        return;
    }

    playSpace->recenterCount = vrapi_GetSystemStatusInt(&java_, VRAPI_SYS_STATUS_RECENTER_COUNT);

    ovrPosef spacePose = vrapi_LocateTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);
    memcpy(&playSpace->position, &spacePose.Position, 3 * sizeof(float));
    memcpy(&playSpace->rotation, &spacePose.Orientation, 4 * sizeof(float));

    ovrVector3f bboxScale;
    // Theoretically pose (the 2nd parameter) could be nullptr, since we already have that, but
    // then this function gives us 0-size bounding box, so it has to be provided.
    vrapi_GetBoundaryOrientedBoundingBox(ovr_, &spacePose, &bboxScale);
    playSpace->area_width = 2.0f * bboxScale.x;
    playSpace->area_height = 2.0f * bboxScale.z;

    uint32_t pointsCount = 0;
    vrapi_GetBoundaryGeometry(ovr_, 0, &pointsCount,nullptr);
    if (pointsCount != 0) {
        if (pointsCount != guardian_points.size()) {
            guardian_points.clear();
            guardian_points.resize(pointsCount);
        }
        vrapi_GetBoundaryGeometry(ovr_, pointsCount, &playSpace->perimeterPointsCount,&guardian_points[0]);
        playSpace->perimeterPoints = reinterpret_cast<float (*)[3]>(&guardian_points[0]);
    }
}

void OvrApplication::CreateFrameBuffer(const ovrJava *java, const bool useMultiview) {
    num_buffers_ = useMultiview ? 1 : VRAPI_FRAME_LAYER_EYE_MAX;

    // Create the frame buffers.
    for ( int eye = 0; eye < num_buffers_; eye++ )
    {
        frame_buffer_[eye].create( useMultiview,
                                   GL_RGBA8,
//                                  vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH ),
                                   vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_DISPLAY_PIXELS_WIDE ) / 2,
//                                  vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT ),
                                   vrapi_GetSystemPropertyInt( java, VRAPI_SYS_PROP_DISPLAY_PIXELS_WIDE ),
                                   NUM_MULTI_SAMPLES);
    }
}

void OvrApplication::DestoryFrameBuffer() {
    for ( int eye = 0; eye < num_buffers_; eye++ )
    {
        frame_buffer_[eye].destory();
    }
    ClearFrameBuffer();
}

void OvrApplication::ClearFrameBuffer() {
    for(auto & frameBuffer : frame_buffer_)
    {
        frameBuffer.clear();
    }
    num_buffers_ = VRAPI_FRAME_LAYER_EYE_MAX;
}

void OvrApplication::Quit3DUI() {
    Application::Quit3DUI();
    LOGV("OnQuit3DUI");
    auto env_wraper = Context::instance()->GetEnv();
    auto env = env_wraper.get();
    if (env == nullptr) {
        LOGV("OnQuit3DUI failed env empty");
        return;
    }
    jclass clazz = env->GetObjectClass(java_.ActivityObject);
//    clazz = env->FindClass("com/pxy/cloudlarkxroculus/MainActivity");
    jmethodID mid = env->GetMethodID(clazz, "switchTo2DAppList", "()V");
    env->CallVoidMethod(java_.ActivityObject, mid);
    env->DeleteLocalRef(clazz);
}

void OvrApplication::JniCallbackOnError(int code, const std::string& msg) {
    LOGV("JniCallbackOnError");
    auto env_wraper = Context::instance()->GetEnv();
    auto env = env_wraper.get();
    if (env == nullptr) {
        LOGV("OnQuit3DUI failed env empty");
        return;
    }
    jclass clazz = env->GetObjectClass(java_.ActivityObject);
//    clazz = env->FindClass("com/pxy/cloudlarkxroculus/MainActivity");
    jmethodID mid = env->GetMethodID(clazz, "onError", "(ILjava/lang/String;)V");

    jstring jstr = env->NewStringUTF(msg.c_str());
    env->CallVoidMethod(java_.ActivityObject, mid, code, jstr);

    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(jstr);
}

#ifdef ENABLE_CLOUDXR
// cloudxr callback
void OvrApplication::UpdateClientState(cxrClientState state, cxrStateReason reason) {
    LOGI("UpdateClientState state %d reason %d", state, reason);
    switch (state) {
        case cxrClientState_ReadyToConnect:
            Navigation::ShowToast("创建CloudXR客户端成功");
            break;
        case cxrClientState_ConnectionAttemptInProgress:
            Navigation::ShowToast("开始连接服务器");
            break;
        case cxrClientState_StreamingSessionInProgress:
            Navigation::ShowToast("连接服务器成功");
            if (scene_cloud_) {
                scene_cloud_->OnCloudXRConnected();
            }
            if (ovr_ != nullptr && tracking_space_ != VRAPI_TRACKING_SPACE_LOCAL_FLOOR) {
                vrapi_SetTrackingSpace(ovr_, VRAPI_TRACKING_SPACE_LOCAL_FLOOR);
                tracking_space_ = VRAPI_TRACKING_SPACE_LOCAL_FLOOR;
            }
            break;
        case cxrClientState_ConnectionAttemptFailed:
        {
            if (!prepare_public_ip_.empty()) {
                need_reconnect_public_ip_ = true;
            } else {
                char buff[200];
                sprintf(buff, "连接CloudXR服务器失败 reason %d", reason);
                Navigation::ShowToast(buff);
                // release resource when cloudxr connected failed.
                xr_client_->Close();
            }
        }
            break;
        case cxrClientState_Disconnected:
        {
            char buff[200];
            sprintf(buff, "与CloudXR服务器连接断开 reason %d", reason);
            Navigation::ShowToast(buff);
            // release resource when cloudxr close.
            xr_client_->Close();
        }
            break;
        default:
            break;
    }
}

void OvrApplication::ReceiveUserData(const void *data, uint32_t size) {

}

void OvrApplication::GetTrackingState(cxrVRTrackingState *state) {
    // skip when ovr not ready.
    if (ovr_ == nullptr) {
        return;
    }

    scene_cloud_->UpdateAsync(ovr_);

    *state = CloudXRClient::VRTrackingStateFrom(scene_cloud_->device_pair_frame());
}

void
OvrApplication::OnCloudXRReady(const char* appServerIp, const char* preferOutIp) {
    Application::OnCloudXRReady(appServerIp, preferOutIp);
    prepare_public_ip_ = preferOutIp;
    cxrError error = cloudxr_client_->Connect(appServerIp);
    if (error != cxrError_Success) {
        const char* errorString = cxrErrorString(error);
        LOGE("Error in LatchFrame [%0d] = %s", error, errorString);
        Navigation::ShowToast(errorString);
        need_recreat_cloudxr_client_ = true;
    }
}

#endif
