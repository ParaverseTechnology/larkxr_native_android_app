//
// Created by fcx@pingxingyung.com on 2022/8/2.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_REGION_LIST_H
#define CLOUDLARKXR_REGION_LIST_H

#include <iostream>
#include <vector>
#include "../common.h"

namespace lark {

struct RegionInfo {
    std::string certDomainUrl;
    std::string detectRttFlag;
    std::string preferPublicIp;
    std::string publicIp;
    std::string regionId;
    std::string regionName;
    std::string serverIp;
    std::string serverPort;
    std::string sslPort;
};

class GetRegionListListener {
public:
    virtual void OnSuccess(bool detectRttFlag, const std::vector<RegionInfo>& appliPageInfo) = 0;
    virtual void OnFailed(const std::string& msg) = 0;
};

class GetRegionListImp;
class LARK_COMMON_API GetRegionList {
public:
    GetRegionList();
    ~GetRegionList();

    // send sync
    void Send();
    // send async
    void SendAsync();

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
    const std::vector<RegionInfo>& region_list() const;

    void set_listener(GetRegionListListener* callback);
private:
    GetRegionListImp* get_region_list_imp_;
};
}

#endif //CLOUDLARKXR_REGION_LIST_H
