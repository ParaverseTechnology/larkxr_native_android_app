# Lark VR Andorid Apps

本项目为使用云雀 VR Android SDK 接入 VR 云渲染的 Demo 项目。演示多种 VR 一体机头显设备接入的方式，包括 oculus quest 系列，htc focus 系列，pico neo 系列等。

[Lark VR SDK文档](https://pingxingyun.github.io/nativeclient_xr_sdk/)

[平行云](https://www.pingxingyun.com/)

[LarkVR 服务端部署手册](https://www.pingxingyun.com/online/vr3.html?id=447)

项目目录结构如下：

```path
cmake-----------------------------cmake 配置
doc-------------------------------文档
lib_pxygl-------------------------适用于 VR 双目渲染的 opengl 的封装
lib_xr_common_ui------------------APP 的通用原生 ui
xr_app_htc------------------------HTC focus 系列头显适配
xr_app_huawei---------------------Huawei VR 系列头显适配
xr_app_oculus---------------------Meta Oculus Quest 系列头显适配(系统版本46以下)
xr_app_openxr_oculus--------------Meta Oculus Quest 系列头显适配
xr_app_openxr_pico----------------Pico4/Pico Neo3 系列头显的适配
third_party-----------------------第三方库
```

## Lark VR SDK 兼容测试

- [:heavy_check_mark:] [Meta Qculus Quest(Include > 46.0)](https://www.oculus.com/quest-2/)
- [:heavy_check_mark:] [HTC Focus](https://www.vive.com/cn/product/vive-focus/)
- [:heavy_check_mark:] [Pico4/Pico Neo3](https://www.pico-interactive.com/)
- [:heavy_check_mark:] [Nolo Sonic](https://www.nolovr.com/index)
- [:heavy_check_mark:] [奇遇](https://dev-qiyu.iqiyi.com/)
- [:heavy_check_mark:] [Nreal](https://www.nreal.cn/nrealapp/)
- [:heavy_check_mark:] [Huawei](https://developer.huawei.com/consumer/cn/vr/)

## code style

[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
