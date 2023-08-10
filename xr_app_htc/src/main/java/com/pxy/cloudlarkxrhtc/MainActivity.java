package com.pxy.cloudlarkxrhtc;

import com.htc.vr.sdk.VRActivity;
import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.libxrcommonui.PermissionHelper;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.File;

public class MainActivity extends VRActivity {
    private static String TAG = "HTC_MainActivity";
    private static String MODEL_VIVE_FLOW = "Vive Flow";
    static {
//        // cloud xr sdk use JNI_OnLoad to get java vm;
//        // muse load lib to trigger JNI_Onload
        if (BuildConfig.ENABLE_CLOUDXR) {
            System.loadLibrary("CloudXRClient");
        }
        System.loadLibrary("lark_xr_htc");
    }
    //
    private XrSystem xrSystem = null;

    public MainActivity() {
        super.setUsingRenderBaseActivity(true);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Log.e(TAG,"inmain");
        String s1 = getFilesDir().getAbsolutePath();

        String s2 = "";
        File externalFile = getExternalFilesDir("/");
        if (externalFile != null) {
            s2 = externalFile.getAbsolutePath();
        }

        init(getResources().getAssets(), s1, s2);

        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().init(this);
        // shoud init vr system before vr started.
        xrSystem = new XrSystem();
        xrSystem.init(this);

        Log.d(TAG, "onCreate");

        nativeCreated(MODEL_VIVE_FLOW.equals(Build.MODEL));
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        nativeResume();
        xrSystem.onResume();

        // check permission
        if (!PermissionHelper.hasRequiredPermissions(this)) {
            PermissionHelper.requestPermissions(this);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause");
        nativePause();
        xrSystem.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");
        nativeDestory();
        xrSystem.onDestroy();
    }

    // Pass this acitivty instance to native
    public native void init(AssetManager am, String internalDataPath, String externalDataPath);
    private native void nativeCreated(boolean isViveFlow);
    private native void nativeResume();
    private native void nativePause();
    private native void nativeDestory();
}
