//
// Created by fcx@pingxingyung.com on 2022/8/2.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_REGION_RTT_TEST_H
#define CLOUDLARKXR_REGION_RTT_TEST_H

#include <iostream>
#include "../common.h"

namespace lark {

class RegionRttTestListener {
public:
    virtual void OnRegionRttTestResult(uint64_t rtt, const std::string& regionId) = 0;
    virtual void OnRegionRttTestError(const std::string& err, const std::string& regionId) = 0;
    virtual void OnRegionRttTestClose(const std::string& err, const std::string& regionId) = 0;
};

class RegionRttTestImp;
class LARK_COMMON_API RegionRttTest {
public:
    RegionRttTest();
    ~RegionRttTest();

    bool Connect(const std::string& ip, int port, const std::string& path, const std::string& regionId);
    void Close();

    void set_listener(RegionRttTestListener* listener);
private:
    RegionRttTestImp* region_rtt_test_;
};
}

#endif //CLOUDLARKXR_REGION_RTT_TEST_H
