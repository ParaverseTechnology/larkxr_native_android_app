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
struct EnterAppliInfo
{
	std::string appServer;
	int         appPort;
	std::string appliId;
	std::string taskId;
	std::string initWinSize;
	std::string preferPubOutIp;
	int         noOperationTimeout;
	int         wsProxy;
	std::string initCursorMode;
	std::string logLevel;
	std::string rttLimit;
	std::string rttLimitInterval;
	// useGamePad
};

class GetEnterAppliInfoImp;
class LARK_COMMON_API GetEnterAppliInfo {
public:
	GetEnterAppliInfo();
	~GetEnterAppliInfo();
	// send sync
	void Send(const std::string& appliId, const std::string& macAddress);
	// send async
	void SendAsync(const std::string& appliId, const std::string& macAddress);

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
