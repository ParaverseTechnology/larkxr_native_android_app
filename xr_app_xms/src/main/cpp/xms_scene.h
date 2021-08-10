//
// Created by fcx@pingxingyun.com on 2021/7/15.
//

#ifndef CLOUDLARKXR_XMS_SCENE_H
#define CLOUDLARKXR_XMS_SCENE_H

//#include <glm/glm.hpp>
//#include <glm/detail/type_quat.hpp>
#include <object.h>
#include "svrApplication.h"

class XmsScene {
public:
    XmsScene();
    virtual ~XmsScene();

    virtual void InitGl(Svr::SvrApplicationContext* context);
    virtual void UpdateHMDPose(const svrHeadPoseState& headPoseState);
    virtual void Draw(int eye);
    virtual void Submit();
protected:
    void AddObject(std::shared_ptr<lark::Object> object);
    void RemoveObject(std::shared_ptr<lark::Object> object);
    void ClearObject();

    float fov_0_ = 101.0f;
    float fov_1_ = 101.0f;
    float near_z_ = 0.01f;
    float far_z_ = 50.0f;
    float ipd_ = 0.062f;

    int eye_buffer_width_ = 1920;
    int eye_buffer_height_ = 1920;

    glm::mat4 view_[2];
    glm::mat4 projection_[2];

    // objects.
    std::vector<std::shared_ptr<lark::Object>> objects_{};
    Svr::SvrApplicationContext* context_;

    // CloudXR texture
    GLuint render_texture_[2];
    svrHeadPoseState pose_state_;
};


#endif //CLOUDLARKXR_XMS_SCENE_H
