package com.pxy.xr_app_huawei;

import androidx.annotation.NonNull;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.huawei.hvr.LibUpdateClient;
import com.pxy.cloudlarkxrkit.CrashHandler;
import com.pxy.cloudlarkxrkit.XrSystem;
import com.pxy.larkcore.Util;
import com.pxy.libxrcommonui.PermissionHelper;

import java.io.File;

public class MainActivity extends Activity implements SurfaceHolder.Callback {
    private static final String TAG = "HuaweiMainAcitvity";
    static {
        // cloud xr sdk use JNI_OnLoad to get java vm;
        // muse load lib to trigger JNI_Onload
        if (BuildConfig.ENABLE_CLOUDXR) {
            System.loadLibrary("CloudXRClient");
        }
        // System.loadLibrary("lark_xr_huawei");
        System.loadLibrary("lark_pxygl");
    }

    private long exitTime = 0;
    private Context mContext = null;
    // lark xr system
    private XrSystem xrSystem = null;
    // render surface
    private SurfaceView mView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        CrashHandler.getInstance().init(this);
        mContext = this;
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);// 设置全屏
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        ConnectivityManager connectivityManager =
                (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            connectivityManager.registerDefaultNetworkCallback(mNetworkCallback);
        } else {
            NetworkRequest request = new NetworkRequest.Builder()
                    .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET).build();
            connectivityManager.registerNetworkCallback(request, mNetworkCallback);
        }

        xrSystem = new XrSystem();
        xrSystem.init(this, Util.getLocalMacAddress(this));

        mView = new SurfaceView(this);
        setContentView(mView);

        mView.getHolder().addCallback(this);
        //getDir();
        new LibUpdateClient(this).runUpdate();

        System.loadLibrary("lark_xr_huawei");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "java activity onResume");
        xrSystem.onResume();

        // check permission
/*        if (!PermissionHelper.hasRequiredPermissions(this)) {
            PermissionHelper.requestPermissions(this);
        }*/
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "java activity onPause");
        xrSystem.onPause();
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");
        super.onDestroy();
        xrSystem.onDestroy();

        nativeDestroy();
    }

    private ConnectivityManager.NetworkCallback mNetworkCallback = new ConnectivityManager.NetworkCallback() {
        @Override
        public void onAvailable(@NonNull Network network) {
            super.onAvailable(network);
            Log.d(TAG, "on net work avaliable");
            //nativeNetworkAvaliable();
        }

        @Override
        public void onLost(@NonNull Network network) {
            super.onLost(network);
            Log.d(TAG, "on net work lost");
            //nativeNetworkLost();
        }
    };


    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.i(TAG, "onKeyDown");
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getAction() == KeyEvent.ACTION_DOWN) {
            if ((System.currentTimeMillis() - exitTime) > 2000) {
                Toast.makeText(getApplicationContext(), "press again to exit", Toast.LENGTH_SHORT).show();
                exitTime = System.currentTimeMillis();
            } else {
                finish();
                System.exit(0);
            }
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.i(TAG, "surfaceChanged");

        String s1 = getFilesDir().getAbsolutePath();

        String s2 = "";
        File externalFile = getExternalFilesDir("/");
        if (externalFile != null) {
            s2 = externalFile.getAbsolutePath();
        }

        nativeInit(width, height, 0, mContext, holder.getSurface(), getResources().getAssets(), s1, s2);
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed");
        uninit();
    }

    public static native void nativeInit(int width, int height, int tex, Context act, Surface sf, AssetManager am, String internalDataPath, String externalDataPath);
    public static native void uninit();
    public static native void nativeDestroy();
}