package com.pxy.larkxr_openxr_pico;

import android.content.res.AssetManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.libxrcommonui.PermissionHelper;

import java.io.File;

public class MainActivity extends android.app.NativeActivity {
    private static final String TAG = MainActivity.class.getSimpleName();

    static {
        System.loadLibrary( "lark_pxygl");
        System.loadLibrary( "lark_xr_common_ui");
        // Note: Must handle loading of dependent shared libraries manually before
        // the shared library that depends on them is loaded, since there is not
        // currently a way to specify a shared library dependency for NativeActivity
        // via the manifest meta-data.
        System.loadLibrary("openxr_loader");
        // cloud xr sdk use JNI_OnLoad to get java vm;
        // muse load lib to trigger JNI_Onload
        if (BuildConfig.ENABLE_CLOUDXR) {
            System.loadLibrary("CloudXRClient");
        }
        System.loadLibrary("lark_xr_openxr_pico");
    }
    //
    private XrSystem xrSystem = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        String s1 = getFilesDir().getAbsolutePath();

        String s2 = "";
        File externalFile = getExternalFilesDir("/");
        if (externalFile != null) {
            s2 = externalFile.getAbsolutePath();
        }

        nativeInit(getResources().getAssets(), s1, s2);

        // shoud init vr system before vr started.
        xrSystem = new XrSystem();
        xrSystem.init(this);

        // 系统对在android里输出verbose级别日志做了限制，可以使用命令来打开，你试试
        // adb root
        // adb shell setprop persist.log.tag V

        Log.v("TestBuildInfo",  " MANUFACTURER " + Build.MANUFACTURER + "\r\n"
                + " MODEL " + Build.MODEL + "\r\n"
                + " BRAND " + Build.BRAND + "\r\n"
                + " BOARD " + Build.BOARD + "\r\n"
                + " DEVICE " + Build.DEVICE + "\r\n"
                + " USER " + Build.USER + "\r\n"
                + " ID " + Build.ID + "\r\n"
                + " DISPLAY " + Build.DISPLAY + "\r\n"
                + " BOOTLOADER " + Build.BOOTLOADER + "\r\n"
                + " PRODUCT " + Build.PRODUCT + "\r\n"
                + " HOST " + Build.HOST + "\r\n"
                + " TAGS " + Build.TAGS + "\r\n"
                + " FINGERPRINT " + Build.FINGERPRINT + "\r\n"
                + " HARDWARE " + Build.HARDWARE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        xrSystem.onResume();

        // check permission
        if (!PermissionHelper.hasRequiredPermissions(this)) {
            PermissionHelper.requestPermissions(this);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        xrSystem.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        xrSystem.onDestroy();
        nativeDestory();
    }

    private native void nativeInit(AssetManager assetManager, String internalPath, String externalPath);
    private native void nativeDestory();
}
