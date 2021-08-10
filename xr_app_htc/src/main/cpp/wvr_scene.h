//
// Created by fcx on 2020/5/7.
//

#ifndef CLOUDLARKXR_WVR_SCENE_H
#define CLOUDLARKXR_WVR_SCENE_H

#include "wvr/wvr_device.h"
#include <wvr/wvr_types.h>
#include <wvr/wvr_events.h>
#include <vector>
#include "wvr_frame_buffer.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <object.h>

#define VR_MAX_CLOCKS 200
#define OBJ_ERROR_CHECK(obj) if ((obj)->hasError() || (obj)->hasGLError()) return false

class WvrScene {
public:
    WvrScene();
    virtual ~WvrScene();

    virtual bool InitGL(void* left_eye_queue, void* right_eye_qeue,
                        std::vector<WvrFrameBuffer*>* left_eye_fbo, std::vector<WvrFrameBuffer*>* right_eye_fbo);
    virtual bool ShutdownGL();
    virtual void Update();

    // handle input
    virtual bool HandleInput();
    // render.
    virtual bool Render();
protected:
    // update pose
    virtual void UpdateHMDMatrixPose();

    virtual void UpdateEyeToHeadMatrix(bool is6DoF);
    virtual void SetupCameras();
    virtual void UpdateTime();
    virtual void ProcessVREvent(const WVR_Event_t & event);

    void RenderStereoTargets();
    void RenderScene(WVR_Eye nEye);

    void AddObject(std::shared_ptr<lark::Object> object);
    void RemoveObject(std::shared_ptr<lark::Object> object);
    void ClearObject();

    float near_clip_ = 0;
    float far_clip_ = 0;

    glm::mat4 hmd_pose_{};
    glm::mat4 eye_pos_left_{};
    glm::mat4 eye_pos_right_{};

    glm::mat4 projection_left_{};
    glm::mat4 projection_right_{};

    uint32_t render_width_ = 0;
    uint32_t render_height_ = 0;

    int32_t index_left_ = 0;
    int32_t index_right_ = 0;

    void* left_eye_q_ = nullptr;
    void* right_eye_q_ = nullptr;
    std::vector<WvrFrameBuffer*>* left_eye_fbo_{};
    std::vector<WvrFrameBuffer*>* right_eye_fbo_{};

    glm::mat4 world_translation_{};  // a little backward and upper to avoid been in a cube.

    bool is_6dof_pose_ = true;

    WVR_DevicePosePair_t vr_device_pairs_[WVR_DEVICE_COUNT_LEVEL_1]{};

    glm::mat4 device_pose_array_[WVR_DEVICE_COUNT_LEVEL_1]{};

    int valid_pose_count_ = 0;

    std::string pose_classes_{};                            // what classes we saw poses for this frame
    char dev_class_char_[WVR_DEVICE_COUNT_LEVEL_1]{};        // for each device, a character representing its class

    uint32_t time_accumulator_2s_ = 0;  // add in micro second.
    struct timeval rtc_time_{};
    int frame_count_ = 0;
    float fps_ = 0;

    uint32_t clock_count_ = 0;

    // objects.
    std::vector<std::shared_ptr<lark::Object>> objects_{};
};
#endif //CLOUDLARKXR_WVR_SCENE_H
