// ++ LICENSE-HIDDEN HEADER ++

#ifndef INCLUDED_OPENVR_OS_IMPL_H_
#define INCLUDED_OPENVR_OS_IMPL_H_

#include <openvr/openvr.h>
#include <vector>
#include <string>

namespace vr {

typedef void (*frameCBptr)(uint64_t handle, unsigned char* dataBuffer, int width, int height, int size, int format);

struct VRSystemDisplayFuncTable_t {
    bool (*GetDispRefreshRate)(float *pfrate);
    bool (*GetTimeSinceLastVsync)(float *pfSecondsSinceLastVsync, uint64_t *pulFrameCounter);
    bool (*IsReportsTimeSinceVSync)();
};

// declare register callback function
VR_INTERFACE bool registerDisplayFunc(VRSystemDisplayFuncTable_t *dptab);
}

#endif
