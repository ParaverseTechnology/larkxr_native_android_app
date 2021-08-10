package com.pxy.xr_app_xms;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static android.os.Environment.*;

/**
 * Created by Administrator on 2019/7/27.
 */

public class FileUtil {
    public static final int BUFFER_SIZE = 2048;
    public static  void copyAssetsToExternal(Context c, String subFolder) {
        AssetManager assetManager = c.getAssets();
        String[] files = null;
        try {
            InputStream in = null;
            OutputStream out = null;

            files = assetManager.list(subFolder);
            for (int i = 0; i < files.length; i++) {
                Log.e("FileUtil","file="+files[i].toString());
                in = assetManager.open(subFolder + "/" + files[i]);
                String outDir = c.getExternalFilesDir(null).toString() + "/";

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
            Log.e("FileUtil", "Failed to get asset file list.", e);
        }

        File file = c.getExternalFilesDir(null);
        Log.d("FileUtil", "file:" + file.toString());
    }
    /*
     * read file from InputStream and write to OutputStream.
     */
    private static  void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[BUFFER_SIZE];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    public static  boolean copySDCardToExternal(Context c, String name) {
        File file = new File(getExternalStorageDirectory(), name);

        if(!file.exists()){
            return false;
        }
        Log.d("FileUtil", "file:" + file.toString());
        try {
                InputStream in = null;
                OutputStream out = null;
                in = new FileInputStream(file);
                String outDir = c.getExternalFilesDir(null).toString() + "/"+name;
                File outFile = new File(outDir);
                out = new FileOutputStream(outFile);
                copyFile(in, out);
                in.close();
                in = null;
                out.flush();
                out.close();
                out = null;
            } catch (FileNotFoundException e1) {
            e1.printStackTrace();
            return false;
        } catch (IOException e1) {
            e1.printStackTrace();
            return false;
        }

        return  true;

    }
}
