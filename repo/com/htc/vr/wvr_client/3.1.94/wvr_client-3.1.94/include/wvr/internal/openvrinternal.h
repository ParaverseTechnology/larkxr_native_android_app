// ++ LICENSE-HIDDEN HEADER ++
/** @file
    @brief Header

    @date 2016

    @author
    hTC, Inc.
*/

#ifndef SRC_OPENVR_OPENVRINTERNAL_H_
#define SRC_OPENVR_OPENVRINTERNAL_H_

// Internal Includes

// OpenVR includes

// Standard includes
#include <stdint.h>
#include <openvr/openvr.h>
extern "C" {

namespace vr
{
    enum DistortionColor{
		Distort_Red = 0,
		Distort_Green =1,
		Distort_Blue =2
    };

    enum CalibrateType{
		Calibrate_IPD = 0,
		Calibrate_Distortion = 1,
    };


    struct DistortCalibrate_t{
		//value k1,k2,k3
		float red[3];
		float green[3];
		float blue[3];
    };

    struct DistortionFactor_t {
        float red[7];
        float green[7];
        float blue[7];
    };

    struct StencilMesh_t {
        uint32_t vertCount;
        float* vertData;
        uint32_t triCount;
        uint16_t* indexData;
    };

    enum EVREventFlag {
        VREventFlag_OnPause           = 0x00000001,
        VREventFlag_OnResume          = 0x00000002,
        VREventFlag_SurfaceChanged    = 0x00000004,
        VREventFlag_SurfaceDestroyed  = 0x00000008,
        VREventFlag_OnBackKey         = 0x00000010
    };

    enum EVRArenaShape
    {
        VRXArenaShape_None = 0,
        VRXArenaShape_Rectangle = 1,
        VRXArenaShape_Round = 2,
        VRXArenaShape_Hexagon = 6
    };

    enum EVRSubmitFlags_Internal {
        Submit_FreeRunMode = 1 << 30    // For free-run mode enable/disable
    };


    typedef void (*ScreenshotCallback)(int32_t errorCode, const unsigned char *pixels, int width, int height);

    // new API based on flags value because some event arch. changed.
    void setEventFlag(uint32_t flagBit);
    void clearEventFlag(uint32_t flagBit);
    uint32_t getEventFlags(void);

    int OpenVR_getCustomContextSurfaceType();
    bool OpenVR_IsATWActive();
    void OpenVR_SetATWActive(bool isActive, void *anativeWindow = nullptr);
    void OpenVR_ShutdownRender();
    void setClientActive(bool isActive);
    void setDynamicDistortion(DistortCalibrate_t &distortionPolynomial);
    void setDistortionSetEnable(bool isEnable);
    void setDistortionFactor(DistortionFactor_t &dFactor);
    void setDynamicDistortionEnable(bool isEnable);
    void setIPDChangeEnable(bool isEnable);

    void updateIpdChanged(float pfIpd);
    bool checkHMD6DoF();
    const char * getHmdTrackingSystemName();
    void takeScreenshot(int width, int height, int mode, ScreenshotCallback callback);
    void setVsyncThreadId(int tid);

    void setViewportOffset(const float x, const float y);
    void OpenVR_GetStencilMesh(bool eEye, vr::StencilMesh_t* outputMesh);
    void OpenVR_SetStencilMeshConfig(bool realtime, float centerX, float centerY, float radiusX, float radiusY);
    void OpenVR_SetBoundaryBlurConfig(bool enable);
}
    bool OpenVR_IsPresentedOnExternal();
    int OpenVR_GetNumberOfTextures();
    void* OpenVR_StoreRenderTextures(uint32_t texturesIDs[], int size, bool eEye, vr::TextureTarget target);
    uint32_t OpenVR_GetAvailableTextureID(void* queue);
    void* OpenVR_ReplaceCurrentTextureID(void* queue, void* imageHandle);
    void releaseTextureQueue(void* queue);
    bool releaseTexture(int32_t texture_id);

    // Apply WVR_EventType_Quit
    void WVR_onQuit();
}

#endif /* SRC_OPENVR_OPENVRINTERNAL_H_ */
