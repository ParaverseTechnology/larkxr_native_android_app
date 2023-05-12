package com.pxy.cloudlarkxrpico.Activity;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.SwitchCompat;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.pxy.cloudlarkxrkit.Config;
import com.pxy.cloudlarkxrpico.Adapter.AppListAdapter;
import com.pxy.cloudlarkxrpico.Adapter.ServerListApinnerAdapter;
import com.pxy.cloudlarkxrpico.Bean.ServerBean;
import com.pxy.cloudlarkxrpico.MainActivity;
import com.pxy.cloudlarkxrpico.R;
import com.pxy.cloudlarkxrpico.UI.AnimatBanner;
import com.pxy.larkcore.ClientLifeManager;
import com.pxy.larkcore.CloudlarkManager;
import com.pxy.larkcore.ImSocketChannel;
import com.pxy.larkcore.SocketChannelObserver;
import com.pxy.larkcore.Util;
import com.pxy.larkcore.request.AppListItem;
import com.pxy.larkcore.request.Base;
import com.pxy.larkcore.request.Bean.GetRunModeBean;
import com.pxy.larkcore.request.GetAppliList;
import com.pxy.larkcore.request.GetRunMode;
import com.pxy.larkcore.request.PageInfo;
import com.pxy.larkcore.request.ScheduleTaskManager;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class ListActivity extends Activity {
    private static final String TAG = "ovr_activity_list";
    private static final String SETTING = "pxy_setting";
    private static final String SETTING_LIST_3D = "list3D";
    //网络设置
    private static final String SETTING_SERVER = "serverAddress";
    private static final String SETTING_SERVER_USE_HTTPS = "useHttps";
    private List<ServerBean> serverBeanList = new ArrayList<>();
    private Spinner serverListspinner;
    private ServerListApinnerAdapter serverListApinnerAdapter = null;
    //生命周期管理
    private ClientLifeManager clientLifeManager;
    private Config config = null;
    //IP
    private String mServerIp = "";
    private boolean useHttps = false;

    //设置按钮和列表
    private Button confirmip, closeip, closeSetTab, clearServer;
    private RecyclerView rec;
    //设置面板
    private LinearLayout setIp, advancedList, setTab, selfOnline;
    private View advanceListDivi;
    private EditText inputIp, inputPort;
    //列表adapter
    private AppListAdapter appListAdapter;
    //列表组件
    private SwitchCompat advancedSetting, list3D, vibrator, UseH265, reportFecFailed, UseFovRending, Use10Bit;
    private RadioGroup QuickConfigLevel;
    private RadioButton QuickConfigLevel_Manual, QuickConfigLevel_Fast, QuickConfigLevel_Normal, QuickConfigLevel_Extreme;
    private SeekBar resolutionScaleBar, coderateBar;
    private TextView resolutionScale, coderate, SelfOnlineText;
    private RadioGroup StreamType;
    private RadioButton larkStreamType_UDP, larkStreamType_TCP, larkStreamType_KCP;
    //socket链接
    private ImSocketChannel imSocketChannel;
    //翻页请求
    private GetAppliList getAppliList;
    List<AppListItem> applist;
    private int mPage = 1;
    private ImageView lastPage, nextPage;
    //关闭app按钮
    private ImageView closeApp;
    //打开菜单
    private ImageView openMenu, opennet;
    //getrunmode
    private GetRunMode getRunMode;
    //初始引导步骤
    private TextView text1, text2, text3, text4, text5;
    private ConstraintLayout firstrun;
    private int stap = 1;
    //列表样式
    private RadioGroup list_show_type;
    private RadioButton type_list, type_scroll;
    private int list_show_type_num = 0;
    //animateBanner
    private AnimatBanner animateBanner;
    private LinearLayout list_background;
    private ScheduleTaskManager s1, s2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_list);
        FindViewById();

        SharedPreferences sp = getSharedPreferences("firststap", Context.MODE_PRIVATE);
        if (sp.getBoolean("first", true)) {
            firstrun.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (stap >= 5) {
                        stap = 0;
                        SharedPreferences sp = getSharedPreferences("firststap", Context.MODE_PRIVATE);
                        SharedPreferences.Editor editor = sp.edit();
                        editor.putBoolean("first", false);
                        editor.apply();
                        firstrun.setVisibility(View.GONE);

                        Init();
                        initview();
                    }
                    StapText(stap++);
                }
            });
            firstrun.setVisibility(View.VISIBLE);
        } else {
            Init();
            initview();
        }
    }

    public Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            getMessage(msg);
        }
    };

    private void Init() {
        config = Config.readFromCache(this);
        BaseApplication.getInstance().setmHandler(handler);
        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        boolean list3Dbool = sp.getBoolean(SETTING_LIST_3D, false);
        list3D.setChecked(list3Dbool);
        if (list3Dbool) {
            GoMainActivity(ListActivity.this, null);
        }
        if (!sp.getString(SETTING_SERVER, "").isEmpty()) {
            serverBeanList = JSON.parseArray(sp.getString(SETTING_SERVER, ""), ServerBean.class);
           /* mServerIp = "http://" + serverBeanList.get(0).getIp() + ":" + serverBeanList.get(0).getPort();
            useHttps = serverBeanList.get(0).getUse_https();*/
            Log.e("serverBeanList", serverBeanList.toString());
            setIp();
        } else {
            Log.e("serverBeanList", "null");
            showSetupIP();
        }

        CloudlarkManager.init(this, CloudlarkManager.APP_TYPE_VR);

        ListActivity.this.runOnUiThread(() -> {
            switch (config.quickConfigLevel) {
                case Config.QuickConfigLevel_Manual:
                    QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
                    break;
                case Config.QuickConfigLevel_Normal:
                    QuickConfigLevel.check(R.id.QuickConfigLevel_Normal);
                    break;
                case Config.QuickConfigLevel_Fast:
                    QuickConfigLevel.check(R.id.QuickConfigLevel_Fast);
                    break;
                case Config.QuickConfigLevel_Extreme:
                    QuickConfigLevel.check(R.id.QuickConfigLevel_Extreme);
                    break;
            }
            syncConfig(config);
        });
    }

    private void FindViewById() {
        rec = findViewById(R.id.rec);
/*        int screenWidth = getWindowManager().getDefaultDisplay().getWidth(); // 屏幕宽（像素，如：480px）
        int screenHeight = getWindowManager().getDefaultDisplay().getHeight(); // 屏幕高（像素，如：800p）
        int spancount=screenWidth>screenHeight?4:2;*/
        rec.setLayoutManager(new GridLayoutManager(this, 4));
        selfOnline = findViewById(R.id.SelfOnline);

        setTab = findViewById(R.id.setTab);
        setIp = findViewById(R.id.setIP);
        inputIp = findViewById(R.id.inputIp);
        inputPort = findViewById(R.id.inputPort);
        confirmip = findViewById(R.id.confirmip);
        closeip = findViewById(R.id.closeip);
        closeSetTab = findViewById(R.id.closeSetTab);

        advancedSetting = findViewById(R.id.advancedSetting);
        list3D = findViewById(R.id.list3D);
        advancedList = findViewById(R.id.advancedList);
        advanceListDivi = findViewById(R.id.advanceListDivi);
        vibrator = findViewById(R.id.vibrator);

        QuickConfigLevel = findViewById(R.id.QuickConfigLevel);
        QuickConfigLevel_Manual = findViewById(R.id.QuickConfigLevel_Manual);
        QuickConfigLevel_Fast = findViewById(R.id.QuickConfigLevel_Fast);
        QuickConfigLevel_Normal = findViewById(R.id.QuickConfigLevel_Normal);
        QuickConfigLevel_Extreme = findViewById(R.id.QuickConfigLevel_Extreme);

        resolutionScaleBar = findViewById(R.id.resolutionScaleBar);
        resolutionScale = findViewById(R.id.resolutionScale);
        coderateBar = findViewById(R.id.coderateBar);
        coderate = findViewById(R.id.coderate);

        SelfOnlineText = findViewById(R.id.SelfOnlineText);

        StreamType = findViewById(R.id.StreamType);
        larkStreamType_UDP = findViewById(R.id.larkStreamType_UDP);
        larkStreamType_TCP = findViewById(R.id.larkStreamType_TCP);
        larkStreamType_KCP = findViewById(R.id.larkStreamType_KCP);

        UseH265 = findViewById(R.id.UseH265);
        reportFecFailed = findViewById(R.id.reportFecFailed);
        UseFovRending = findViewById(R.id.UseFovRending);
        Use10Bit = findViewById(R.id.Use10Bit);

        lastPage = findViewById(R.id.lastPage);
        nextPage = findViewById(R.id.nextPage);

        closeApp = findViewById(R.id.closeApp);
        openMenu = findViewById(R.id.openMenu);
        opennet = findViewById(R.id.opennet);

        serverListspinner = findViewById(R.id.serverList);
        clearServer = findViewById(R.id.clearServer);

        text1 = findViewById(R.id.text1);
        text2 = findViewById(R.id.text2);
        text3 = findViewById(R.id.text3);
        text4 = findViewById(R.id.text4);
        text5 = findViewById(R.id.text5);
        firstrun = findViewById(R.id.firstRun);

        list_show_type = findViewById(R.id.list_show_type);
        animateBanner = findViewById(R.id.animateBanner);
        list_background = findViewById(R.id.list_background);
    }

    private void initview() {
        Log.d(TAG, "cached server address use https " + useHttps + " serverIp " + mServerIp);

        opennet.setOnClickListener(v -> showSetupIP());

        closeip.setOnClickListener(v -> setIp.setVisibility(View.GONE));

        openMenu.setOnClickListener(v -> {
            Init();
            setTab.setVisibility(View.VISIBLE);
        });

        confirmip.setOnClickListener(v -> {
            if (inputIp.getText().toString().isEmpty()) {
                toastInner("IP不能为空");
                return;
            }
            hideSoftInputFromWindow(this);
            setIp.setVisibility(View.GONE);
            setServerSpinner(inputIp.getText().toString(), inputPort.getText().toString());
            setIp();
        });

        advancedSetting.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                advancedList.setVisibility(View.VISIBLE);
                advanceListDivi.setVisibility(View.VISIBLE);
            } else {
                advancedList.setVisibility(View.GONE);
                advanceListDivi.setVisibility(View.GONE);
            }
        });

        list3D.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                SharedPreferences sp = ListActivity.this.getSharedPreferences(SETTING, Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = sp.edit();
                editor.putBoolean(SETTING_LIST_3D, isChecked);
                editor.apply();
            }
        });

        closeSetTab.setOnClickListener(v -> {
            setTab.setVisibility(View.GONE);
            Config.saveToCache(ListActivity.this, config);
            if (list3D.isChecked()) {
                GoMainActivity(ListActivity.this, null);
                list3D.setChecked(false);
            }
        });

        RadioButton buttonManual = findViewById(R.id.QuickConfigLevel_Manual);
        RadioButton buttonFast = findViewById(R.id.QuickConfigLevel_Fast);
        RadioButton buttonNormal = findViewById(R.id.QuickConfigLevel_Normal);
        RadioButton buttonExtreme = findViewById(R.id.QuickConfigLevel_Extreme);

        buttonManual.setOnClickListener((view) -> {
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            Log.d(TAG, "QuickConfigLevel changed " + config.quickConfigLevel);

            config.QuickConfigWithDefaulSetup(config.quickConfigLevel);
            syncConfig(config);
        });

        buttonFast.setOnClickListener((view) -> {
            config.quickConfigLevel = Config.QuickConfigLevel_Fast;
            Log.d(TAG, "QuickConfigLevel changed " + config.quickConfigLevel);

            config.QuickConfigWithDefaulSetup(config.quickConfigLevel);
            syncConfig(config);
        });

        buttonNormal.setOnClickListener(view -> {
            config.quickConfigLevel = Config.QuickConfigLevel_Normal;
            Log.d(TAG, "QuickConfigLevel changed " + config.quickConfigLevel);

            config.QuickConfigWithDefaulSetup(config.quickConfigLevel);
            syncConfig(config);
        });

        buttonExtreme.setOnClickListener(view -> {
            config.quickConfigLevel = Config.QuickConfigLevel_Extreme;
            Log.d(TAG, "QuickConfigLevel changed " + config.quickConfigLevel);

            config.QuickConfigWithDefaulSetup(config.quickConfigLevel);
            syncConfig(config);
        });

        resolutionScaleBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // skip setup with other
                if (!fromUser) {
                    return;
                }
                ListActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        /*DecimalFormat df=new DecimalFormat("0.00");
                        resolutionScale.setText(df.format((float)progress/100));*/
                        resolutionScale.setText(progress + "");
                        config.resulutionScale = (float) progress / 100;
                        // 调整高级设置 调整为手动挡
                        config.quickConfigLevel = Config.QuickConfigLevel_Manual;
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
                    }
                });
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        coderateBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // skip setup with other
                if (!fromUser) {
                    return;
                }
                ListActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        coderate.setText(progress * 10000 + "");
                        config.biteRate = progress * 10000;

                        // 调整高级设置 调整为手动挡
                        config.quickConfigLevel = Config.QuickConfigLevel_Manual;
                        QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
                    }
                });
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });


        vibrator.setOnCheckedChangeListener((buttonView, isChecked) -> {
            config.vibrator = isChecked;
        });

        larkStreamType_UDP.setOnClickListener((buttonView) -> {
            config.streamType = Config.larkStreamType_UDP;

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });

        larkStreamType_TCP.setOnClickListener((buttonView) -> {
            config.streamType = Config.larkStreamType_TCP;

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });

        larkStreamType_KCP.setOnClickListener((buttonView) -> {
            config.streamType = Config.larkStreamType_KCP;

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });


        UseH265.setOnClickListener((buttonView) -> {
            config.useH265 = ((SwitchCompat)buttonView).isChecked();

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });

        reportFecFailed.setOnClickListener((buttonView) -> {
            config.reportFecFailed = ((SwitchCompat)buttonView).isChecked();

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            Log.d(TAG, "fecfailed checked " + config.reportFecFailed + " " + config.quickConfigLevel);
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });

        UseFovRending.setOnClickListener((buttonView) -> {
            config.useFovRendering = ((SwitchCompat)buttonView).isChecked();

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });

        Use10Bit.setOnClickListener((buttonView) -> {
            config.use10BitEncoder = ((SwitchCompat)buttonView).isChecked();

            // 调整高级设置 调整为手动挡
            config.quickConfigLevel = Config.QuickConfigLevel_Manual;
            QuickConfigLevel.check(R.id.QuickConfigLevel_Manual);
        });

        lastPage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (getAppliList != null) {
                    getAppliList.setPage(--mPage);
                    getAppliList.getAppliList();
                }
            }
        });

        nextPage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (getAppliList != null) {
                    getAppliList.setPage(++mPage);
                    getAppliList.getAppliList();
                }
            }
        });

        closeApp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                StopApp(true);
            }
        });

        serverListspinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                //处理事件的代码
                Log.d("position", position + "");
                if (position==0)return;
                setServerSpinner(serverBeanList.get(position).getIp(), serverBeanList.get(position).getPort());
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) { }
        });
        serverListApinnerAdapter = new ServerListApinnerAdapter(ListActivity.this, serverBeanList);
        serverListspinner.setAdapter(serverListApinnerAdapter);

        clearServer.setOnClickListener(v -> {
            serverBeanList.clear();
            SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
            SharedPreferences.Editor editor = sp.edit();
            editor.putString(SETTING_SERVER, "");
            editor.apply();

            serverListspinner.setAdapter(null);

            toastInner("记录已清除");
        });

        list_show_type.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                switch (checkedId) {
                    case R.id.type_list:
                        list_show_type_num = 0;
                        break;
                    case R.id.type_scroll:
                        list_show_type_num = 1;
                        break;
                }
            }
        });

    }

    private void syncConfig(Config config) {
        Log.d(TAG, "syncConfig " + config.reportFecFailed);
        switch (config.streamType) {
            case Config.larkStreamType_UDP:
                larkStreamType_UDP.setChecked(true);
                break;
            case Config.larkStreamType_TCP:
                larkStreamType_TCP.setChecked(true);
                break;
            case Config.larkStreamType_KCP:
                larkStreamType_KCP.setChecked(true);
                break;
        }

        vibrator.setChecked(config.vibrator);

        int resulutionScaleint = (int) (config.resulutionScale * 100);
        resolutionScale.setText(resulutionScaleint + "");
        resolutionScaleBar.setProgress(resulutionScaleint);

        UseH265.setChecked(config.useH265);
        reportFecFailed.setChecked(config.reportFecFailed);
        UseFovRending.setChecked(config.useFovRendering);
        Use10Bit.setChecked(config.use10BitEncoder);

        coderate.setText(config.biteRate + "");
        coderateBar.setProgress(config.biteRate / 10000);
    }

    private void StapText(int s) {
        List<TextView> textViews = new ArrayList<>();
        textViews.add(text1);
        textViews.add(text2);
        textViews.add(text3);
        textViews.add(text4);
        textViews.add(text5);
        for (int i = 0; i < textViews.size(); i++) {
            textViews.get(i).setVisibility(View.GONE);
        }
        textViews.get(s).setVisibility(View.VISIBLE);
    }

    private void showSetupIP() {
        ListActivity.this.runOnUiThread(() -> {
            setIp.setVisibility(View.VISIBLE);
            appListAdapter = null;
            rec.setAdapter(null);
            StopApp(false);

            if (serverBeanList.isEmpty()) return;
            setServerSpinner(serverBeanList.get(0).getIp(), serverBeanList.get(0).getPort());
        });
    }

    private void setServerSpinner(String ip, String port) {
        for (int i = 0; i < serverBeanList.size(); i++) {
            if (serverBeanList.get(i).getIp().equals(ip)
                    && serverBeanList.get(i).getPort().equals(port)
            ) {
                serverBeanList.remove(i);
            }
        }

        inputIp.setText(ip);
        inputPort.setText(port);

        ServerBean serverBean = new ServerBean();
        serverBean.setIp(ip);
        serverBean.setPort(port);
        serverBean.setUse_https(false);

        serverBeanList.add(0, serverBean);

        SharedPreferences sp = getSharedPreferences(SETTING, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putString(SETTING_SERVER, String.valueOf(JSON.toJSON(serverBeanList)));
        editor.apply();

        serverListApinnerAdapter = new ServerListApinnerAdapter(ListActivity.this, serverBeanList);
        serverListspinner.setAdapter(serverListApinnerAdapter);
    }

    private void setIp() {
        if (clientLifeManager != null) {
            clientLifeManager.ClientOffline();
        }
        if (serverBeanList.isEmpty()) return;


        ServerBean serverBean = serverBeanList.get(0);
        Log.e("serverBean", serverBean.toString());

        mServerIp = "http://" + serverBean.getIp() + ":" + serverBean.getPort();

        Base.releaseUrl();
        Base.setServerAddr(useHttps, mServerIp);

        inputIp.setText(serverBean.getIp());
        inputPort.setText(serverBean.getPort());

        config.serverIp = serverBean.getIp();
        config.serverPort = Integer.parseInt(serverBean.getPort());

        if (imSocketChannel == null) {
            Log.e("imSocketChannel", "isnull");
            imSocketChannel = new ImSocketChannel(socketChannelObserver, ListActivity.this);
            imSocketChannel.connect();
        }


        if (s1 == null) {
            s1 = new ScheduleTaskManager(3000);
            if (getRunMode == null) {
                getRunMode = new GetRunMode(new GetRunMode.Callback() {
                    @Override
                    public void onSuccess(GetRunModeBean getRunModeBean) {
                        setMessage(2, getRunModeBean);
                    }

                    @Override
                    public void onFail(String s) {
                        Log.e("getRunModeFaile", s);
                    }
                });
                s1.addTask(() -> {
                    getRunMode.dorequest(Util.getLocalMacAddress(ListActivity.this));
                });
            }
            if (getAppliList == null) {
                getAppliList = new GetAppliList(new GetAppliList.Callback() {
                    @Override
                    public void onSuccess(List<AppListItem> list) {
                        setMessage(1, list);
                    }

                    @Override
                    public void onPageInfoChange(PageInfo pageInfo) {
                        mPage = pageInfo.getPageNum();
                        ListActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (pageInfo.hasPreviousPage()) {
                                    lastPage.setImageResource(R.mipmap.lastpage);
                                    lastPage.setEnabled(true);
                                } else {
                                    lastPage.setImageResource(R.mipmap.lastpagefalse);
                                    lastPage.setEnabled(false);
                                }

                                if (pageInfo.hasNextPage()) {
                                    nextPage.setImageResource(R.mipmap.nextpage);
                                    nextPage.setEnabled(true);
                                } else {
                                    nextPage.setImageResource(R.mipmap.nextpagefalse);
                                    nextPage.setEnabled(false);
                                }
                            }
                        });
                    }

                    @Override
                    public void onFail(String s) {
                        Log.e("GetapplistFail", s);
                    }
                });
                s1.addTask(() -> {
                    getAppliList.getAppliList();
                });
            }

        }
        if (s2 == null) {
            s2 = new ScheduleTaskManager(10 * 1000);
            if (clientLifeManager == null) {
                clientLifeManager = new ClientLifeManager(this);
                s2.addTask(() -> clientLifeManager.GetHertBeat());
            }
        }

        clientLifeManager.ClientOnline();
        s1.startTask();
        s2.startTask();
    }

    private void toastInner(final String msg) {
        runOnUiThread(() -> Toast.makeText(ListActivity.this, msg, Toast.LENGTH_SHORT).show());
    }

    private void setMessage(int what, Object obj) {
        Message message = Message.obtain();
        message.what = what;
        message.obj = obj;
        //message.obj = ToJavaBean.toJavaBean(value,obj);
        handler.sendMessage(message);
    }

    private void StopApp(Boolean close) {
        Log.e("StopApp", String.valueOf(close));
        if (config != null) {
            Config.saveToCache(this, config);
        }
        if (clientLifeManager != null) {
            clientLifeManager.ClientOffline();
        }
        if (imSocketChannel != null && imSocketChannel.isConnected()) {
            imSocketChannel.close();
            Log.e("imSocketChannel", "close");
        }
        if (rec.getAdapter() != null) {
            rec.setAdapter(null);
        }
        if (s1 != null) {
            s1.stopTask();
        }
        if (s2 != null) {
            s2.stopTask();
        }
        if (close) {
            if (s1 != null) {
                s1.release();
            }
            if (s2 != null) {
                s2.release();
            }
            finish();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d(TAG, "onPause");
        StopApp(false);
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop");
        //StopApp(false);
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart");
        if (clientLifeManager != null) {
            clientLifeManager.ClientOnline();
        }
        if (imSocketChannel != null) {
            if (imSocketChannel.isConnected()) {
                imSocketChannel.close();
            }
            imSocketChannel.connect();
        }
        if (s1 != null) {
            s1.startTask();
        }
        if (s2 != null) {
            s2.startTask();
        }
        //Init();
        config = Config.readFromCache(ListActivity.this);
        setServerSpinner(config.serverIp, String.valueOf(config.serverPort));
        setIp();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");
        StopApp(true);
        super.onDestroy();
    }

    public static void hideSoftInputFromWindow(Activity activity) {
        if (activity != null) {
            InputMethodManager imm = (InputMethodManager) activity.getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) {
                imm.hideSoftInputFromWindow(activity.getWindow().getDecorView().getWindowToken(), 0);
            }
        }
    }

    private final SocketChannelObserver socketChannelObserver = new SocketChannelObserver() {
        @Override
        public void onOpen() {
            Log.e("socketChannelObserver", "onOpen");
        }

        @Override
        public void onClose() {
            Log.e("socketChannelObserver", "onClose");
        }

        @Override
        public void onError(String s) {
            Log.e("socketChannelObserver", "onError:" + s);
            if (imSocketChannel != null) {
                if (imSocketChannel.isConnected()) {
                    imSocketChannel.close();
                }
                imSocketChannel.connect();
            }
        }

        @Override
        public void onMessage(byte[] bytes) {
            Log.e("socketChannelObserver", "onMessagebyt:" + bytes.toString());
        }

        @Override
        public void onMessage(String s) {
            Log.d(TAG, "onMessagestr:" + s);
            try {
                JSONObject jsonObject = new JSONObject(s);
                switch (jsonObject.optString("type")) {
                    case ImSocketChannel.IM_MESSAGE_TYPE_KEEPALIVE: {
                      /*  if (clientLifeManager != null) {
                            clientLifeManager.ClientOnline();
                        }*/
                        break;
                    }
                    case ImSocketChannel.IM_MESSAGE_TYPE_START: {
                        if (!getTopActivity(ListActivity.this).equals(MainActivity.class.getName()))
                            GoMainActivity(ListActivity.this, jsonObject.optString("appliId"));
                        break;
                    }
                    case ImSocketChannel.IM_MESSAGE_TYPE_STOP: {
                        Message message = new Message();
                        message.what = 4;
                        BaseApplication.getInstance().getmHandler().sendMessage(message);
                        break;
                    }
                    case ImSocketChannel.IM_MESSAGE_TYPE_CONNECT_SUCCESS: {
                        toastInner("连接成功");
                        break;
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    };

    public void GoMainActivity(Context context, String appid) {
        Intent intent = new Intent(context, MainActivity.class);
        if (appid != null) {
            Log.e("GoMainActivity", appid);
            intent.putExtra("appid", appid);
        } else {
            intent.putExtra("appid", "");
            Log.e("GoMainActivity", "justGo");
        }
        startActivity(intent);
    }

    private void getMessage(Message msg) {
        if (msg.what == 1) {
            Log.v("getapplist", msg.what + "");
            List<AppListItem> locallist = (List<AppListItem>) msg.obj;
            switch (list_show_type_num) {
                case 0: {
                    animateBanner.setVisibility(View.GONE);
                    rec.setVisibility(View.VISIBLE);
                    list_background.setBackground(getResources().getDrawable(R.mipmap.border));
                    if (rec.getAdapter() == null) {
                        appListAdapter = new AppListAdapter(ListActivity.this, locallist);
                        rec.setAdapter(appListAdapter);
                    } else {
                        if (!locallist.equals(applist)) {
                            applist = locallist;
                            appListAdapter = new AppListAdapter(ListActivity.this, applist);
                            rec.setAdapter(appListAdapter);
                        }
                    }
                }
                break;
                case 1: {
                    animateBanner.setVisibility(View.VISIBLE);
                    rec.setVisibility(View.GONE);
                    list_background.setBackground(getResources().getDrawable(R.mipmap.homebackground));
                    animateBanner.setAutoPlay(false);
                    animateBanner.setClickSwitchable(true);
                    if (animateBanner.getItems() == null || animateBanner.getItems().isEmpty()) {
                        Log.e("ani", "animateBanner.getItems()==null");
                        animateBanner.setImagesToBanner(locallist);
                    } else {
                        Log.e("ani", "animateBannerNotnnull");
                        if (!locallist.equals(applist)) {
                            applist = locallist;
                            animateBanner.setImagesToBanner(applist);
                        }
                    }
                }
                break;
            }
        } else if (msg.what == 2) {
            GetRunModeBean getRunModeBean = (GetRunModeBean) msg.obj;
            Log.e("gerrunmode", getRunModeBean.toString());
            if (getRunModeBean.getCode() == 1000) {
                if (getRunModeBean.getResult().getRunMode().equals("1")) {
                    //集中管控模式下
                    selfOnline.setVisibility(View.GONE);
                    SelfOnlineText.setVisibility(View.VISIBLE);
                    /* GoMainActivity(ListActivity.this, getRunModeBean.getResult().getPrimaryClientId());
                     getRunMode = null;*/
                    if (clientLifeManager != null) {
                        clientLifeManager.ClientOnline();
                    }
                } else {
                    selfOnline.setVisibility(View.VISIBLE);
                    SelfOnlineText.setVisibility(View.GONE);
                }
            } else {
                toastInner(getRunModeBean.getMessage());
            }
        } else if (msg.what == 3) {
            ListActivity.this.onPause();
            ListActivity.this.onStop();
        }

    }

    public String getTopActivity(Context context) {
        ActivityManager manager = (ActivityManager) context.getSystemService(ACTIVITY_SERVICE);
        List<ActivityManager.RunningTaskInfo> runningTaskInfos = manager.getRunningTasks(1);

        if (runningTaskInfos != null) {
            return (runningTaskInfos.get(0).topActivity.getClassName());
        } else
            return null;
    }
}