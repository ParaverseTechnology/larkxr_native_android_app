package com.pxy.larkxr_openxr_pico;

import android.content.res.AssetManager;
import android.os.Bundle;

import com.pxy.cloudlarkxrkit.XrSystem;

import java.io.File;

public class MainActivity extends android.app.NativeActivity {
    static {
        // Note: Must handle loading of dependent shared libraries manually before
        // the shared library that depends on them is loaded, since there is not
        // currently a way to specify a shared library dependency for NativeActivity
        // via the manifest meta-data.
        System.loadLibrary("openxr_loader");
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
    }

    @Override
    protected void onResume() {
        super.onResume();
        xrSystem.onResume();
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
