//
// Created by fcx@pingixngyun.com on 2019/11/18.
//

#include <input.h>
#include "log.h"
#include "controller.h"
#include "vertex_array_object.h"
#include "env_context.h"
#define TAG "Controller"

namespace lark {
Controller::Controller(bool isLeft, const ControllerConfig& config):
    model_(nullptr),
    raycast_(nullptr),
    is_left(isLeft),
    ray_cast_type_(isLeft ? Input::RayCast_left : Input::RayCast_Right)
{
    auto env = Context::instance()->GetEnv();
    lark::AndroidAssetContext context = {
            Context::instance()->native_activity(),
            env.get(),
            Context::instance()->bitmap_factory(),
    };
    lark::ModelAsset modelAsset = {
            isLeft ? config.modelLeft : config.modelRight,
    };
    LOGV("model load start");
//    model_ = lark::AssetLoader::instance()->LoadModel(&context, modelAsset);
    model_ = std::make_shared<lark::Model>(isLeft ? config.modelLeft : config.modelRight);
    model_->Init(&context);
    model_->SetColor(config.color);
    if (config.diffuse.path != "") {
        model_->AddMeterailTexture(&context, config.diffuse);
    }
    if (config.specular.path != "") {
        model_->AddMeterailTexture(&context, config.specular);
    }
    model_->Scale(config.modelScale);
    if (config.modelRotate != 0) {
        model_->Rotate(config.modelRotate, config.modelRotateAxis);
    }
    AddChild(model_);
    LOGV("model load finish %d", model_ == nullptr);

    raycast_ = std::make_shared<Raycast>();
    raycast_->Rotate(glm::half_pi<float>(), glm::vec3(-1, 0, 0));
    raycast_->Move(isLeft ? config.rayOffsetLeft : config.rayOffsetRight);
    AddChild(raycast_);
}

void Controller::Update() {
    Object::Update();
    if (ray_cast_type_ == Input::GetCurrentRayCastType() && !is_main_) {
        // active controller.
        raycast_->set_color(glm::vec4(0, 0, 1, 1));
        is_main_ = true;
    }
    if (ray_cast_type_ != Input::GetCurrentRayCastType() && is_main_) {
        raycast_->set_color(glm::vec4(1, 1, 1, 1));
        is_main_ = false;
    }
}

}