//
// Created by fcx@pingxingyung.com on 2021/6/22.
// Copyright (c) 2021 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_GET_CORE_SERVER_VERSION_INFO_H
#define CLOUDLARKXR_GET_CORE_SERVER_VERSION_INFO_H

#include <iostream>
#include "../common.h"

namespace lark {
struct CoreServerVersionInfo {
    int marjor;
    int minor;
    int revise;
    int build;
};

class GetCoreServerVersionInfoImp;
class LARK_COMMON_API GetCoreServerVersionInfo {
public:
    GetCoreServerVersionInfo();
    ~GetCoreServerVersionInfo();

    // send sync
    void Send();
    // send async
    void SendAsync();

    CoreServerVersionInfo core_server_version_info();

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    GetCoreServerVersionInfoImp* imp_ = nullptr;
};
}
#endif //CLOUDLARKXR_GET_CORE_SERVER_VERSION_INFO_H
