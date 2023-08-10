// 
// Created by fcx@pingxingyun.com
// 2020-04-29 10:35
//
#pragma once
#ifndef XR_TRACKING_FRAME_INCLUDE
#define XR_TRACKING_FRAME_INCLUDE
#include "lark_xr/lark_xr.h"
#include "lark_xr/xr_video_frame.h"
#include "lark_xr/types.h"
#include <memory>

namespace lark {
/**
 * 视频和姿态数回调。
 * 一般只在使用软件界面器时会回调该数据。
 * @see xr_client.h
*/
class LARK_XR_API XRTrackingFrame {
public:
	static std::unique_ptr<XRTrackingFrame> Create(std::unique_ptr<larkxrTrackingFrame>&& tracking, std::unique_ptr<XRVideoFrame>&& videoFrame);

	XRTrackingFrame(const XRTrackingFrame& trackingFrame);
	XRTrackingFrame(XRTrackingFrame&& trackingFrame);

	const bool Avaliable() const;
	const uint64_t GetFrameIndex() const;
	const larkxrTrackingFrame* GetTracking() const;
	const XRVideoFrame* GetVideoFrame() const;
private:
	XRTrackingFrame();

	void SetTracking(std::unique_ptr <larkxrTrackingFrame>&& tracking);
	void SetVideoFrame(std::unique_ptr<XRVideoFrame>&& videoFrame);

	std::unique_ptr<larkxrTrackingFrame>  tracking_{};
	std::unique_ptr<XRVideoFrame> video_frame_{};
};
}
#endif // XR_TRACKING_FRAME_INCLUDE