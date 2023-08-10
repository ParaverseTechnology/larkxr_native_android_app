//
// Created by fcx on 2020/5/12.
//

#ifndef CLOUDLARKXR_REPORT_RESOURCE_H
#define CLOUDLARKXR_REPORT_RESOURCE_H

#include <iostream>
#include "../common.h"

namespace lark {
class ReportResourceImp;
class LARK_COMMON_API ReportResource {
public:
    struct Resource {
        uint64_t memTotal;
        uint64_t memUsed;
        uint64_t diskTotal;
        uint64_t diskUsed;
        uint64_t sdTotal;
        uint64_t sdUsed;
        // 百分比电量
        int batteryHead;
        int batteryHandLeft;
        int batteryHandRight;
    };

    ReportResource();
    ~ReportResource();


    void Report(const std::string& clientMac, const std::string& clientBrand, const Resource& resource);

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    ReportResourceImp* report_resource_imp_;
};
}

#endif //CLOUDLARKXR_REPORT_RESOURCE_H
