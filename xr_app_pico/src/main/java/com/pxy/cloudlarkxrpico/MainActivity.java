package com.pxy.cloudlarkxrpico;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import com.picovr.cvclient.CVController;
import com.picovr.cvclient.CVControllerListener;
import com.picovr.cvclient.CVControllerManager;
import com.picovr.picovrlib.cvcontrollerclient.ControllerClient;
import com.picovr.vractivity.Eye;
import com.picovr.vractivity.HmdState;
import com.picovr.vractivity.RenderInterface;
import com.picovr.vractivity.VRActivity;
import com.psmart.vrlib.PicovrSDK;
import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.Utils;
import com.pxy.cloudlarkxrkit.XrSystem;

import java.io.File;


public class MainActivity extends VRActivity implements RenderInterface {

    private static final String TAG = "pvr_mainactivity";

    private static int HEAD_SETY_TYPE_OTHER = 0;
    private static int HEAD_SETY_TYPE_NEO_2 = 1;
    private static int HEAD_SETY_TYPE_NEO_3 = 2;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("lark_xr_pico");
    }

    // native application
    private long nativeApplication = 0;
    //
    private XrSystem xrSystem = null;
    //
    private long checkBatteryTimestamp = 0;
    // init cv service
    private CVController leftController;
    private CVController rightController;
    private CVControllerManager cvManager;
    private CVControllerListener cvListener = new CVControllerListener() {
        @Override
        public void onBindSuccess() {        }

        @Override
        public void onBindFail() {        }

        @Override
        public void onThreadStart() {
            leftController = cvManager.getMainController();
            rightController = cvManager.getSubController();
//            updateConnectState(leftController.getConnectState() << 1 | leftController.getSerialNum());
//            updateConnectState(rightController.getConnectState() << 1 | rightController.getSerialNum());
        }

        @Override
        public void onConnectStateChanged(int i, int i1) {
//            updateConnectState(leftController.getConnectState() << 1 | leftController.getSerialNum());
//            updateConnectState(rightController.getConnectState() << 1 | rightController.getSerialNum());
        }

        @Override
        public void onMainControllerChanged(int i) {        }

        @Override
        public void onChannelChanged(int i, int i1) {        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_FULLSCREEN
                        | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
                WindowManager.LayoutParams.FLAG_FULLSCREEN
                        | WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        String s1 = getFilesDir().getAbsolutePath();

        String s2 = "";
        File externalFile = getExternalFilesDir("/");
        if (externalFile != null) {
            s2 = externalFile.getAbsolutePath();
        }

        nativeInit(getResources().getAssets(), s1, s2);
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().init(this);

        int eyeBufferWidth =  PicovrSDK.GetEyeBufferWidth();
        int eyeBufferHeight = PicovrSDK.GetEyeBufferHeight();

        Log.d(TAG, "on create eye buffer " + eyeBufferWidth + " " + eyeBufferHeight);

        cvManager = new CVControllerManager(this.getApplicationContext());
        cvManager.setListener(cvListener);
        leftController = cvManager.getMainController();
        rightController = cvManager.getSubController();

        // shoud init vr system before vr started.
        xrSystem = new XrSystem();
        xrSystem.init(this);

        Log.d(TAG, "on create nativeApplication " + nativeApplication + " native instance " + nativeApplicationInstance());

        int type = HEAD_SETY_TYPE_OTHER;
        if (XrSystem.PLATFORM_DEVICE_NAME_PICO_NEO_2.equals(Utils.getDeviceName())) {
            type = HEAD_SETY_TYPE_NEO_2;
        } else if (XrSystem.PLATFORM_DEVICE_NAME_PICO_NEO_3.equals(Utils.getDeviceName())) {
            type = HEAD_SETY_TYPE_NEO_3;
        } else {
            Log.e(TAG, "Other Pico controller not test now.");
        }

        // init native application
        if (nativeApplication == 0) {
            if (nativeApplicationInstance() == 0) {
                nativeApplication = nativeInitApplication();
                nativeInitJava(nativeApplication, new PicovrSDK(), this, type);
            } else {
                nativeApplication = nativeApplicationInstance();
            }
        } else {
            Log.w(TAG, "on create but old application found");
        }
        Log.d(TAG, "on create nativeApplication " + nativeApplication);
//        boolean res = PicovrSDK.setTrackingOriginType(1);
//        Log.d(TAG, "set tracking origin " + res);
    }

    @Override
    protected void onResume() {
        super.onResume();
        cvManager.bindService();
        xrSystem.onResume();
        Log.d(TAG, "onResume");
    }

    @Override
    protected void onPause() {
        cvManager.unbindService();
        super.onPause();
        xrSystem.onPause();
        Log.d(TAG, "onPause");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");
        xrSystem.onDestroy();
        if (nativeApplication != 0) {
            nativeReleaseApplication(nativeApplication);
            nativeApplication = 0;
        }
    }

    @Override
    public void onFrameBegin(HmdState hmdState) {
        float[] hmdOrientation = hmdState.getOrientation();
        float[] hmdPosition = hmdState.getPos();
        float[] hmdData = new float[7];
        hmdData[0] = hmdOrientation[0];
        hmdData[1] = hmdOrientation[1];
        hmdData[2] = hmdOrientation[2];
        hmdData[3] = hmdOrientation[3];

        hmdData[4] = hmdPosition[0];
        hmdData[5] = hmdPosition[1];
        hmdData[6] = hmdPosition[2];
        // update for pico neo3
        cvManager.updateControllerData(hmdData);

        long now = System.currentTimeMillis();
        // update battery info every 5s;
        if(now - checkBatteryTimestamp > 5 * 1000) {
            int left = leftController.getBatteryLevel();
            int right = rightController.getBatteryLevel();
            nativeSetControlerBatteryLevel(left, right);
            checkBatteryTimestamp = now;
        }

//        Log.d(TAG, "controller ori " + Arrays.toString(leftController.getOrientation())  + " " + Arrays.toString(leftController.getPosition()));

        nativeUpdateController(nativeApplication, 0, leftController);
        nativeUpdateController(nativeApplication, 1, rightController);
        nativeFrameBegin(nativeApplication, hmdState.getOrientation(),hmdState.getPos());
    }

    @Override
    public void onDrawEye(Eye eye) {
        nativeDraw(nativeApplication, eye.getType());
    }

    @Override
    public void onFrameEnd() {
        // Pass render pose data to SDK for ATW.
        // pose, float array length >= 7, in order "x,y,z,w,px,py,pz".
//        this.nativeSetRenderPose(this.nativePtr, sensor);
        float[] pose = new float[7];
        long[] timeStamp = new long[3];
        boolean res = naitveGetRenderPose(nativeApplication, pose, timeStamp);
        if (res) {
            this.nativeSetRenderPose(this.nativePtr, pose, timeStamp);
        }
        nativeFrameEnd(nativeApplication);
    }

    @Override
    public void onTouchEvent() {
    }

    @Override
    public void onRenderPause() {
        Log.d(TAG, "onRenderPause");
        nativeOnRenderPause(nativeApplication);
    }

    @Override
    public void onRenderResume() {
        Log.d(TAG, "onRenderResume");
        naitveOnRenderResume(nativeApplication);
    }

    @Override
    public void onRendererShutdown() {
        Log.d(TAG, "onRendererShutdown " + nativeApplication);
        xrSystem.onDestroy();
        if (nativeApplication != 0) {
            nativeOnRenderDestory(nativeApplication);
            nativeReleaseApplication(nativeApplication);
            nativeApplication = 0;
        }
    }

    @Override
    public void initGL(int width, int height) {
        Log.d(TAG, "init gl " + width + " " + height);
        nativeInitGl(nativeApplication, width, height);
    }

    @Override
    public void deInitGL() {
        Log.d(TAG, "deInitGL");
        if (nativeApplication != 0) {
            nativeDeInitGl(nativeApplication);
        }
    }

    @Override
    public void renderEventCallBack(int i) {
        Log.d(TAG, "renderEventCallBack " + i);
    }

    @Override
    public void surfaceChangedCallBack(int i, int i1) {
        Log.d(TAG, "surfaceChangedCallBack " + i + " " + i1);
    }

    //
    public void onHapticsFeedback(boolean isLeft, float amplitude, float duration, float frequency) {
        // 设置手柄震动，参数：strength，震动强度，范围0~1 Time，震动时间，范围0~65535ms ControllerSerialNum，0 左手柄，1 右手柄
        ControllerClient.vibrateCV2ControllerStrength(amplitude, (int)(duration * 1000), isLeft ? 0 : 1);
    }

    // native application
    private native void nativeInit(AssetManager am, String internalDataPath, String externalDataPath);
    private native long nativeApplicationInstance();
    private native long nativeInitApplication();
    private native void nativeInitJava(long ptr, PicovrSDK picovrSDK, MainActivity mainActivity, int type);
    private native void nativeReleaseApplication(long ptr);
    // render
    private native void nativeInitGl(long ptr, int eyeWidth, int eyeHeight);
    private native void nativeDeInitGl(long ptr);
    private native void nativeFrameBegin(long ptr, float[] orientation, float[] position);
    private native void nativeDraw(long ptr, int eye);
    private native void nativeFrameEnd(long ptr);
    // get render pose
    private native boolean naitveGetRenderPose(long ptr, float[] pose, long[] timeStamp);

    // controller
    private native void nativeUpdateController(long ptr, int type, CVController controller);
    // lifecycle
    private native void naitveOnRenderResume(long ptr);
    private native void nativeOnRenderPause(long ptr);
    private native void nativeOnRenderDestory(long ptr);
    //
    private native void nativeSetControlerBatteryLevel(int left, int right);
}
