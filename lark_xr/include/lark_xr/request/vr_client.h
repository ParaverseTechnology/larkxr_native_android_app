//
// Created by fcx on 2020/5/12.
//

#ifndef CLOUDLARKXR_VR_CLIENT_H
#define CLOUDLARKXR_VR_CLIENT_H

#include <iostream>
#include "../common.h"

namespace lark {
class LARK_COMMON_API VrClientObserver {
public:
    virtual void OnClientId(const std::string& clientId) = 0;
};
class VrClientImp;
class LARK_COMMON_API VrClient {
public:
    explicit VrClient();
    ~VrClient();

    void Online(const std::string& macAddress);
    void Offline(const std::string& macAddress);

    void SetObserver(VrClientObserver* observer);
    std::string GetClientId() const;
private:
    VrClientImp* vr_client_imp_;
};
}
#endif //CLOUDLARKXR_VR_CLIENT_H
