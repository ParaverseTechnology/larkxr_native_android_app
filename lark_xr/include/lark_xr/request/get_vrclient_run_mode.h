//
// Created by fcx on 2020/5/12.
//

#ifndef CLOUDLARKXR_GET_VRCLIENT_RUN_MODE_H
#define CLOUDLARKXR_GET_VRCLIENT_RUN_MODE_H

#include <iostream>
#include "../common.h"

namespace lark {
class GetVrClientRunModeImp;
class LARK_COMMON_API GetVrClientRunMode {
public:
    enum ClientRunMode {
        CLIENT_RUNMODE_SELF = 0,
        CLIENT_RUNMODE_TEACHER = 1,
    };

    GetVrClientRunMode();
    ~GetVrClientRunMode();

    void RunMode(const std::string& macAddress);
    void RunModeAsync(const std::string& macAddress);
    ClientRunMode run_mode() const;

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    GetVrClientRunModeImp* get_vr_client_run_mode_imp_;
};
}

#endif //CLOUDLARKXR_GET_VRCLIENT_RUN_MODE_H
