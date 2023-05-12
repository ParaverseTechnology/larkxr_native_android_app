package com.pxy.cloudlarkxrpico.Activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.bumptech.glide.load.engine.DiskCacheStrategy;
import com.pxy.cloudlarkxrkit.Config;
import com.pxy.cloudlarkxrpico.BuildConfig;
import com.pxy.cloudlarkxrpico.MainActivity;
import com.pxy.cloudlarkxrpico.R;
import com.pxy.cloudlarkxrpico.UI.Fastclick;

public class StartActivity extends Activity {
    private final String TAG="StartActivity";

    private View animationView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (BuildConfig.ENABLE_2D_LIST) {

            setContentView(R.layout.activity_start);

            Config confg = Config.readFromCache(this);

            ImageView logo = findViewById(R.id.logo);
            TextView loadingText = findViewById(R.id.text_loading);

            if (!confg.serverIp.isEmpty()) {
                logo.setVisibility(View.VISIBLE);
                loadingText.setVisibility(View.GONE);

                animationView = logo;
                String path = "http://" + confg.serverIp + ":" + confg.serverPort + "/images/start_logo.png";
                Glide.with(this)
                        .load(path)
                        .diskCacheStrategy(DiskCacheStrategy.NONE)
                        .skipMemoryCache(true)
                        .into(logo);
            } else {
                logo.setVisibility(View.GONE);
                loadingText.setVisibility(View.VISIBLE);

                animationView = loadingText;
            }

            animationView.setOnClickListener(v -> {
                if (!Fastclick.isFastClick()) {
                    startListActivity();
                }
            });

            Animation animation1 = new AlphaAnimation(0.1f, 1.0f);
            animation1.setDuration(1000);

            Animation animation2 = new AlphaAnimation(1.0f, 0f);
            animation2.setDuration(1000);

            animation1.setAnimationListener(new Animation.AnimationListener() {
                @Override
                public void onAnimationStart(Animation animation) { }

                @Override
                public void onAnimationEnd(Animation animation) {
                    animationView.startAnimation(animation2);
                }

                @Override
                public void onAnimationRepeat(Animation animation) { }
            });

            animation2.setAnimationListener(new Animation.AnimationListener() {
                @Override
                public void onAnimationStart(Animation animation) { }

                @Override
                public void onAnimationEnd(Animation animation) {
                    startListActivity();
                }

                @Override
                public void onAnimationRepeat(Animation animation) { }
            });

            animationView.startAnimation(animation1);
        } else {
            startActivity(new Intent(StartActivity.this, MainActivity.class));
            finish();
        }
 /*       if (XrSystem.PLATFORM_DEVICE_NAME_PICO_NEO_2.equals(Utils.getDeviceName())) {

        } else if (XrSystem.PLATFORM_DEVICE_NAME_PICO_NEO_3.equals(Utils.getDeviceName())) {

        } else {
            Log.e(TAG, "Other Pico controller not test now.");
        }*/
    }

    private void startListActivity(){
        animationView.setVisibility(View.GONE);
        startActivity(new Intent(StartActivity.this, ListActivity.class));
        finish();
    }
}