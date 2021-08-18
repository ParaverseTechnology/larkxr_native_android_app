//
// Created by fcx@pingixngyun.com on 2019/11/18.
//

#ifndef CLOUDLARK_OCULUS_DEMO_CONTROLLER_H
#define CLOUDLARK_OCULUS_DEMO_CONTROLLER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <object.h>
#include <model.h>
#include "input.h"
#include "raycast.h"

// WARNING
// TODO ADD NAMESPACE TO ALL
// WARNING NO NAMESPACE CAUSE PICONEO3 CRASH
namespace lark {
struct ControllerConfig {
    // 模型路径
    std::string modelLeft;
    std::string modelRight;
    // 射线偏移
    glm::vec3 rayOffsetLeft;
    glm::vec3 rayOffsetRight;
    // 模型缩放旋转
    float modelScale;
    float modelRotate;
    glm::vec3 modelRotateAxis;
    // 模型本身的颜色
    glm::vec4 color;
    // 模型材质漫反射贴图
    lark::Model::MaterialTexture diffuse;
    // 模型材质高光
    lark::Model::MaterialTexture specular;
};

const ControllerConfig CONTROLLER_OCULUS_QUEST = {
        "model/oculus_quest_controller_left/oculus_quest_controller_left.obj",
        "model/oculus_quest_controller_right/oculus_quest_controller_right.obj",
        glm::vec3(-0.009, 0.006, -0.007),
        glm::vec3(0.009, 0.006, -0.007),
        1.0f,
        0,
        glm::vec3(0,0,0),
        glm::vec4(1, 1, 1, 1),
        {
                lark::Model::MaterialTextureType_DIFFUSE,
                "oculus_quest_controller.png"
        }
};

const ControllerConfig CONTROLLER_HTC_FOCUS_PLUS = {
        "model/controller_vive_6dof/chirp_0918.fbx",
        "model/controller_vive_6dof/chirp_0918.fbx",
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        0.01f,
        glm::pi<float>(),
        glm::vec3(0,1,0),
};

const ControllerConfig CONTROLLER_HTC_FOCULS = {
        "model/controller_htc_focus/overlay.obj",
        "model/controller_htc_focus/overlay.obj",
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        1.0f,
        0,
        glm::vec3(0,0,0),
};

const ControllerConfig CONTROLLER_PICO_NEO = {
        "model/controller_pico_neo/controller3.obj",
        "model/controller_pico_neo/controller3.obj",
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        0.01f,
        0,
        glm::vec3(0, 0, 0),
        glm::vec4(1, 1, 1, 0.6),
        {
                lark::Model::MaterialTextureType_DIFFUSE,
                "controller3_idle.png"
        }
};

const ControllerConfig CONTROLLER_PICO_NEO_2 = {
        "model/controller_pico_neo_2/controller4Left.obj",
        "model/controller_pico_neo_2/controller4Right.obj",
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        0.01f,
        0,
        glm::vec3(0,0,0),
        glm::vec4(1, 1, 1, 0.6),
        {
                lark::Model::MaterialTextureType_DIFFUSE,
                "controller4_idle.png"
        }
};

const ControllerConfig CONTROLLER_PICO_G2 = {
        "model/controller_pico_g2/controller2.obj",
        "model/controller_pico_g2/controller2.obj",
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        1.0f,
        0,
        glm::vec3(0,0,0),
};

const ControllerConfig CONTROLLER_PICO_GLOBLIN = {
        "model/controller_pico_goblin/controller1.obj",
        "model/controller_pico_goblin/controller1.obj",
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        1.0f,
        0,
        glm::vec3(0,0,0),
};

class Controller: public lark::Object {
public:
    explicit Controller(bool isLeft, const ControllerConfig& config);

    virtual void Update();
private:
    std::shared_ptr<lark::Model> model_;
    std::shared_ptr<Raycast> raycast_ = nullptr;
    bool is_left = false;
    bool is_main_ = false;
    Input::RayCastType ray_cast_type_;
};
}

#endif //CLOUDLARK_OCULUS_DEMO_CONTROLLER_H
