// 
// Created by fcx@pingxingyun.com
// 2020-01-14 17:05
//
#pragma once
#ifndef GET_ENTER_APPLIINFO_INCLUDE
#define GET_ENTER_APPLIINFO_INCLUDE

#include <iostream>
#include "../common.h"

namespace lark
{
typedef enum ForceHMDType_ {
	ForceHMDType_Default = 0,
	ForceHMDType_Quest   = 1,
	ForceHMDType_HTC     = 2,
	ForceHMDType_Pico    = 3,
	ForceHMDType_Nolo    = 4,
} ForceHMDType;

struct EnterAppliInfo
{
	std::string appServer;
	int         appPort;
	std::string appliId;
	std::string taskId;
	int         appliType;
	int         initWinSize;
	std::string preferPubOutIp;
	int         noOperationTimeout;
	int         wsProxy;
	int         initCursorMode;
	std::string logLevel;
	int         rttLimit;
	int         rttLimitInterval;
	int         fullScreenMode;
	// useGamePad
	std::string touchOperateMode;
	std::string groupId;
	int         mobileForceLandscape;
	std::string language;
	std::string serverId;
	int         network;
	int         playerListToggle;
	std::string bgColor;
	int         mobileVirtualJoystick;
	int         taskStatus;
	int         mouseZoomDirection;
	int         useGamepad;
	ForceHMDType forceHmdType;
};

struct EnterAppliParams {
	std::string appliId;
	// 互动模式
	//启动模式：0：普通模式, 1：互动模式（一人操作多人观看），2: 多人协同（键盘鼠标放开，需要应用配合）
	std::string playerMode;
	//Task所有者:1 /  观察者:0
	std::string userType;
	//口令:8位唯一码,写入TaskInfo
	std::string roomCode;
	std::string taskId;
	// groups
	std::string groupId;
	std::string regionId;
	//指定节点分配
	std::string targetServerIp;
	// 覆盖全局 appkey 设置
	std::string appKey;
	// 覆盖全局 appkey 设置时手动设置 timestamp
	std::string timestamp;
	// 覆盖全局 appkey 设置时手动设置 signature
	std::string signature;
	// 附加参数,可能为空
	std::string backEndStartCmdSeqNo;
};

class GetEnterAppliInfoImp;
class LARK_COMMON_API GetEnterAppliInfo {
public:
	GetEnterAppliInfo();
	~GetEnterAppliInfo();
	// send json
	bool SendJson(const std::string& jsonStr, const std::string& macAddress, const std::string& sdkID = "");
	// send with enter appli params
	void Send(const EnterAppliParams& params, const std::string& macAddress, const std::string& sdkID = "");
	// send sync
	void Send(const std::string& appliId, const std::string& macAddress, const std::string& sdkID = "");
	// send async
	void SendAsync(const std::string& appliId, const std::string& macAddress, const std::string& sdkID = "");

	EnterAppliInfo enter_appli_info() const;

	int http_code() const;
	int api_result_code() const;
	std::string message() const;
	bool is_success() const;

private:
	GetEnterAppliInfoImp* get_enter_appli_info_imp_;
};
}
#endif // GET_ENTER_APPLIINFO_INCLUDE
