//=============================================================================
// FILE: SvrNativeActivity.java
//
//                  Copyright (c) 2017 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
package com.pxy.xr_app_xms;

import android.Manifest;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.os.Bundle;
import 	android.content.res.AssetManager;
import android.view.WindowManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.ximmerse.sdk.XDeviceApi2;
import com.pxy.cloudlarkxrkit.XrSystem;

public class SvrNativeActivity extends android.app.NativeActivity
{
	private XrSystem xrSystem = null;

	// native application
	private long nativeApplication = 0;

	static
	{
		System.loadLibrary( "svrapi" );
		System.loadLibrary( "lark_xr_xms" );
	}

	private final String TAG = "SvrNativeActivity";

	public static final String FIRST_TIME_TAG = "first_time";
	public static final  String ASSETS_SUB_FOLDER_NAME = "raw";
	public static final int BUFFER_SIZE = 1024;

	private static String sCalibFilePath_Android = "/sdcard/wonderland/calib/";
	private String mPath = "";

	private String getMarkerSettingPath(String str) {
		File d = new File(sCalibFilePath_Android);
		for (File f : d.listFiles()) {
			if (f.isDirectory() && f.getName().endsWith(str)) {
				String pcba_sn = f.getName().split("-")[0];

//				Log.d("dig", "getMarkerSettingPath: " + pcba_sn);
				return f.getPath() + "/" + pcba_sn + "/" + pcba_sn + "-" + str + ".json";
			}
		}

		return "";
	}

	@Override
	public void onRequestPermissionsResult(int requestCode,  String[] permissions,  int[] grantResults) {
		if (requestCode == 1 && grantResults != null && grantResults.length > 0) {
			if (grantResults[0] != PackageManager.PERMISSION_GRANTED) {
				Log.e(TAG, "Fatal error: read external storage permission has not been granted!");
				finish();
			}

			if (grantResults[1] != PackageManager.PERMISSION_GRANTED) {
				Log.e(TAG, "Error: write external storage permission has not been granted!");
				//finish();
				// Make this non-fatal for the moment.  Only affects logging outside of app directory.
			}

			if (grantResults[2] != PackageManager.PERMISSION_GRANTED) {
				Log.e(TAG, "Fatal error: internet permission has not been granted!");
				finish();
			}

			// if we get here, we're good to go, continue 'resuming'.
			FileUtil.copyAssetsToExternal(this, "raw");
			doResume();
		}
		else
		{
			Log.e(TAG, "Bad return for RequestPermissions: ["+requestCode+"] {"+permissions+"} {"+grantResults+"}");
		}
	}

	private void doResume() {
		Log.d(TAG, "doResume: ");
		//ConfigureController();
	}

	private void ConfigureController() {
		// Get SN
		XDeviceApi2.init(this);
		final XDeviceApi2.XDeviceHandle context_hdl_ = XDeviceApi2.newContext(XDeviceApi2.XDeviceContext_AGIA);
		final XDeviceApi2.XDeviceHandle hmd_hdl_ = XDeviceApi2.getDeviceHandle(context_hdl_,"XHawk-0");

		mPath =  this.getExternalFilesDir(null).toString();

		XDeviceApi2.doAction(hmd_hdl_, XDeviceApi2.XActions.kXAct_ResetMarkerSettings);

		XDeviceApi2.doAction(hmd_hdl_ , XDeviceApi2.XActions.kXAct_LoadMarkerSettingFile,
				mPath + "/"+ "/BEACON-500.json");

		new Thread(new Runnable() {
			@Override
			public void run() {
				while (true) {
					if (LoadMarkerSettingFile(context_hdl_, hmd_hdl_, "XCobra-0"))
						break;
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		}).start();

		new Thread(new Runnable() {
			@Override
			public void run() {
				while (true) {
					if (LoadMarkerSettingFile(context_hdl_, hmd_hdl_, "XCobra-1"))
						break;
					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		}).start();
	}

	private boolean LoadMarkerSettingFile(XDeviceApi2.XDeviceHandle context_hdl_, XDeviceApi2.XDeviceHandle hmd_hdl_, String ctrl_name) {
		XDeviceApi2.XDeviceHandle ctrl_hdl = XDeviceApi2.getDeviceHandle(context_hdl_, ctrl_name);

		String sn = XDeviceApi2.getString(ctrl_hdl, XDeviceApi2.XControllerAttributes.kXCAttr_Str_SerialNumber,
				"");

		if ("".equals(sn)) {
			return false;
		}

		String mPath = getMarkerSettingPath(sn);

		if ("".equals(mPath)) {
			return false;
		}
		XDeviceApi2.doAction(hmd_hdl_, XDeviceApi2.XActions.kXAct_LoadMarkerSettingFile,
				mPath);
		return true;
	}

	@Override 
	public void onWindowFocusChanged (boolean hasFocus)
	{
		if(android.os.Build.VERSION.SDK_INT >= 19) 
		{
			if(hasFocus) 
			{
				getWindow().getDecorView().setSystemUiVisibility(
						View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_FULLSCREEN
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
			}
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		String s1 = getFilesDir().getAbsolutePath();

		String s2 = "";
		File externalFile = getExternalFilesDir("/");
		if (externalFile != null) {
			s2 = externalFile.getAbsolutePath();
		}

		init(getResources().getAssets(), s1, s2);

		super.onCreate(savedInstanceState);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(this);
		if (!prefs.getBoolean(FIRST_TIME_TAG, false)) {
			SharedPreferences.Editor editor = prefs.edit();
			editor.putBoolean(FIRST_TIME_TAG, true);
			editor.commit();
			copyAssetsToExternal();
		}

		xrSystem = new XrSystem();
		xrSystem.init(this);


		Log.d(TAG, "onCreate");

	}

	/*
	 * copy the Assets from assets/raw to app's external file dir
	 */
	public void copyAssetsToExternal() {
		AssetManager assetManager = getAssets();
		String[] files = null;
		try {
			InputStream in = null;
			OutputStream out = null;

			files = assetManager.list(ASSETS_SUB_FOLDER_NAME);
			for (int i = 0; i < files.length; i++) {
				in = assetManager.open(ASSETS_SUB_FOLDER_NAME + "/" + files[i]);
				String outDir = getExternalFilesDir(null).toString() + "/";

				File outFile = new File(outDir, files[i]);
				out = new FileOutputStream(outFile);
				copyFile(in, out);
				in.close();
				in = null;
				out.flush();
				out.close();
				out = null;
			}
		} catch (IOException e) {
			Log.e("tag", "Failed to get asset file list.", e);
		}
		File file = getExternalFilesDir(null);
		Log.d("tag", "file:" + file.toString());
	}

    /*
     * read file from InputStream and write to OutputStream.
     */
	private void copyFile(InputStream in, OutputStream out) throws IOException {
		byte[] buffer = new byte[BUFFER_SIZE];
		int read;
		while ((read = in.read(buffer)) != -1) {
			out.write(buffer, 0, read);
		}
	}

	@Override
	protected void onDestroy() {
		Log.d(TAG, "onDestroy: ");
		XDeviceApi2.exit();
		super.onDestroy();
	}

	@Override
	protected void onPause() {
		XDeviceApi2.exit();
		super.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();

		if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
				checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
				checkSelfPermission(Manifest.permission.INTERNET) != PackageManager.PERMISSION_GRANTED) {
			requestPermissions(new String[] {
					Manifest.permission.READ_EXTERNAL_STORAGE,
					Manifest.permission.WRITE_EXTERNAL_STORAGE,
					Manifest.permission.INTERNET
			}, 1);
			Log.w(TAG, "Waiting for permissons from user...");
			return;  // we await the callback...
		}

		doResume();
	}
	public native void init(AssetManager am, String internalDataPath, String externalDataPath);

}
