//
// Created by fcx@pingxingyun.com on 2019/10/25.
//

#ifndef MY_APPLICATION_SCENE_H
#define MY_APPLICATION_SCENE_H

#include <VrApi_Types.h>
#include "ui/component/text.h"
#include "ui/component/image.h"
#include "ovr_frame_buffer.h"
#include "skybox.h"
#include "model.h"

class OvrScene {
public:
    // current raycast type
    OvrScene();
    virtual ~OvrScene();

    virtual bool InitGL(OvrFrameBuffer *frame_buffer, int num_buffers);
//    virtual bool InitJava(OvrFrameBuffer *frame_buffer, int num_buffers);
    virtual bool ShutdownGL();
    // update on every frame;
    virtual bool Update(ovrMobile * ovr);
    virtual void UpdateHMDMatrixPose();

    inline uint64_t frame_index() { return frame_index_; }
    inline double display_time() { return display_time_; }
    inline glm::quat light_dir() { return light_dir_; }
//    inline bool inited() { return inited_; }
//    inline bool use_multiview() { return use_multiview_; };
//    inline ovrJava * java() { return java_; }
    inline int num_buffers() { return num_buffers_; }
    inline OvrFrameBuffer * frame_buffer() { return frame_buffer_; }
    inline int swap_interval() { return swap_interval_; }

    virtual bool HandleInput(ovrMobile * ovr);
    virtual bool Render(ovrMobile * ovr);
protected:
    //
    virtual ovrLayerProjection2 RenderFrame(const ovrTracking2 * tracking, ovrMobile * ovr);
    virtual void UpdateEyeToHeadMatrix(bool is6DoF);
    virtual void SetupCameras();
    virtual void UpdateTime();
    virtual void UpdateControllerDegreeOfFreedom();

    void AddObject(std::shared_ptr<lark::Object> object);
    void RemoveObject(std::shared_ptr<lark::Object> object);
    void ClearObject();

    inline const uint64_t frame_index() const { return frame_index_; }

    //
    void ClearFrameBuffer();
    void CreateFrameBuffer(const ovrJava * java, const bool useMultiview);
    void DestoryFrameBuffer();
    void OnTestRenderLoading(ovrMobile * ovrMobile1);
    void OnTestRender(ovrMobile * ovrMobile1);

    uint64_t       frame_index_;
    double 		   display_time_;
    // light dir
    glm::quat      light_dir_;

//    bool           inited_;
//    bool           use_multiview_;
//    ovrJava *      java_;

    OvrFrameBuffer* frame_buffer_;
    int			   num_buffers_;
    int            swap_interval_;

    // objects.
    std::vector<std::shared_ptr<lark::Object>> objects_{};
};


#endif //MY_APPLICATION_SCENE_H
