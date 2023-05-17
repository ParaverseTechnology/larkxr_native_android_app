//
// Created by Administrator on 2021/7/6.
//

#ifndef CLOUDLARKXR_PVR_XR_SCENE_H
#define CLOUDLARKXR_PVR_XR_SCENE_H

#include "openxr/openxr.h"
#include <vector>
#include <skybox.h>
#include "graphics_device_android.h"
#include "test_obj.h"
#include "xr_input_state.h"
#include "frame_buffer.h"

class PvrXRScene {
public:
    PvrXRScene();
    virtual ~PvrXRScene() = default;

    virtual void InitGL(GraphicsDeviceAndroid* device);
    virtual void HandleInput(const InputState& input_state, const XrSession& session, const XrSpace& space);
    virtual void RenderView(lark::Object::Eye eye, const XrCompositionLayerProjectionView& layerView, picoxr::FrameBuffer& frameBuffer);
    virtual void ReleaseGL();

protected:
    void AddObject(std::shared_ptr<lark::Object> object);
    void RemoveObject(std::shared_ptr<lark::Object> object);
    void ClearObject();
    // objects.
    std::vector<std::shared_ptr<lark::Object>> objects_{};
    GraphicsDeviceAndroid* device_ = nullptr;
    // test obj
//    std::shared_ptr<TestObj> test_obj_;
//    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
};

#endif //CLOUDLARKXR_PVR_XR_SCENE_H
