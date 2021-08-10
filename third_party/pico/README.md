# 需要注意以下两个接口

1.把渲染时用的 pose 传递给 atw

```java
/**
 * Pass render pose data to SDK for ATW.
 * @param nativePtr, NativeApp pointer. Store in "nativePtr" field when VRActivity be created.
 * @param pose, float array length >= 7, in order "x,y,z,w,px,py,pz".
 */
public native void nativeSetRenderPose(long nativePtr, float[] pose);
```

注意调用时机，此接口应该在回调接口onFrameEnd()里调用，

示例如下：
@Override
public void onFrameEnd() {  this.nativeSetRenderPose(this.nativePtr,sensor); }

2.获取头部姿态时的姿态预测接口

```java
float [] getHmdPose (float time)
```

功能：获取主Sensor的预测方向,位置数据。请在SDK正常运行以后再调用，如果SDK未正常初始化，会调用失败，返回默认值。

参数：time: 预测的时间，单位ms

返回值：float数组，包含{x, y, z, w, px, py, pz}

调用方式：例如 预测50ms以后的数据，`PicovrSDK.getHmdPose( 50 )`
