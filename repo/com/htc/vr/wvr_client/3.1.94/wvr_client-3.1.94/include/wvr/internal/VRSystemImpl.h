// ++ LICENSE-HIDDEN HEADER ++
/** @file
    @brief Header

    @date 2016

    @author
    hTC, Inc.
*/

#ifndef INCLUDED_VRSystemImpl_h_GUID_13D357E8_B0D9_42B4_AD2E_7A9D8BC3154A
#define INCLUDED_VRSystemImpl_h_GUID_13D357E8_B0D9_42B4_AD2E_7A9D8BC3154A

// Internal Includes
#include <vrcore/VRSystemStub.h>
#include <vrcore/proto/common.pb.h>

// OpenVR includes
#include <openvr/openvr.h>

#include <openvrinternal.h>

// Standard includes
#include <mutex>
#include <condition_variable>

// FIXME: using WVR_DeviceType
#define VRCORE_DeviceType_HMD              1
#define VRCORE_DeviceType_Controller_Right 2
#define VRCORE_DeviceType_Controller_Left  3
#define VRCORE_DeviceType_Tracker          4

namespace vr {

class VRSystemImpl : public IVRSystem {
private:
    VRSystemImpl();
    static VRSystemImpl * thiz;

    vrcore::VRSystemStub* mVRSystemStub;
    vrcore::DeviceInfo mVRDeviceTable[vr::k_unMaxTrackedDeviceCount];

    vrcore::ServerStatus mServerStatus;
    vrcore::DeviceInfo GetDeviceById(uint32_t unDeviceIndex);
    bool updateDeviceStatus();

public:
    static VRSystemImpl * getInstance(const char* pchVersion = IVRSystem_Version);
    static void clear();

    void Init();
    void Shutdown();

    vrcore::VRSystemStub* GetVRSystemStub();

    vrcore::DeviceInfo GetDeviceByType(uint32_t deviceType);
    std::vector<vrcore::DeviceInfo> GetDevices(uint32_t typeBitMask);
    inline vrcore::DeviceInfo *GetVRDeviceTable() { return this->mVRDeviceTable; }

    virtual ~VRSystemImpl();
};

};  // namespace vr

#endif  // INCLUDED_VRSystemImpl_h_GUID_13D357E8_B0D9_42B4_AD2E_7A9D8BC3154A
