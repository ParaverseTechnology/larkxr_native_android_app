//
// Created by fcx on 2020/5/14.
//

#ifndef CLOUDLARKXR_VRCLIENT_HEARTBEAT_H
#define CLOUDLARKXR_VRCLIENT_HEARTBEAT_H

#include "../common.h"

namespace lark {
class VrClientHeartbeatImp;
class LARK_COMMON_API VrClientHeartbeat {
public:
    VrClientHeartbeat();
    ~VrClientHeartbeat();
    void HeartBeat(const std::string& macAddress);

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    VrClientHeartbeatImp* vr_client_heart_beat_imp_;
};
}

#endif //CLOUDLARKXR_VRCLIENT_HEARTBEAT_H
