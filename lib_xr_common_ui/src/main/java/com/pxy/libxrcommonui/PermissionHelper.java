package com.pxy.libxrcommonui;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.provider.Settings;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

public class PermissionHelper {
    private static final String INTERNET_PERMISSION = Manifest.permission.INTERNET;
    private static final String WRITE_PERMISSION = Manifest.permission.READ_EXTERNAL_STORAGE;
    private static final String READ_PERMISSION = Manifest.permission.WRITE_EXTERNAL_STORAGE;
    private static final String RECORD_AUDIO = Manifest.permission.RECORD_AUDIO;
    private static final int PERMISSION_CODE = 0;

    /** Check to see we have all permissions for this app. */
    public static boolean hasPermissions(Activity activity) {
        return ContextCompat.checkSelfPermission(activity, INTERNET_PERMISSION)
                        == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(activity, WRITE_PERMISSION)
                        == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(activity, READ_PERMISSION)
                        == PackageManager.PERMISSION_GRANTED;
    }

    /** Check to see we have the required permissions for this app. */
    public static boolean hasRequiredPermissions(Activity activity) {
        return ContextCompat.checkSelfPermission(activity, INTERNET_PERMISSION)
                        == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(activity, WRITE_PERMISSION)
                        == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(activity, READ_PERMISSION)
                        == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(activity, RECORD_AUDIO)
                        == PackageManager.PERMISSION_GRANTED;
    }

    /** Check to see we have the necessary permissions for this app, and ask for them if we don't. */
    public static void requestPermissions(Activity activity) {
        ActivityCompat.requestPermissions( activity,
                new String[] { INTERNET_PERMISSION, WRITE_PERMISSION, READ_PERMISSION, RECORD_AUDIO },
                PERMISSION_CODE
        );
    }

    /** Check to see if we need to show the rationale for this permission. */
    public static boolean shouldShowRequestPermissionRationale(Activity activity) {
        return ActivityCompat.shouldShowRequestPermissionRationale(activity, INTERNET_PERMISSION) ||
                ActivityCompat.shouldShowRequestPermissionRationale(activity, WRITE_PERMISSION) ||
                ActivityCompat.shouldShowRequestPermissionRationale(activity, READ_PERMISSION);
    }

    /** Launch Application Setting to grant permission. */
    public static void launchPermissionSettings(Activity activity) {
        Intent intent = new Intent();
        intent.setAction(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
        intent.setData(Uri.fromParts("package", activity.getPackageName(), null));
        activity.startActivity(intent);
    }
}
