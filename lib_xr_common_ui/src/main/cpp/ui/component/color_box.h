//
// Created by fcx@pingixngyun.com on 2019/11/13.
//

#ifndef CLOUDLARK_OCULUS_DEMO_COLOR_BOX_H
#define CLOUDLARK_OCULUS_DEMO_COLOR_BOX_H

#include <object.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "base.h"

class ColorBox: public lark::Object, public component::Base {
public:
    ColorBox();
    ColorBox(glm::vec4 color, glm::vec2 size, glm::vec3 position);
    ~ColorBox() override = default;

    void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) override;

    void set_size(const component::size& size) override;
    void set_position(const component::position& position) override;
    void set_position(float x, float y, float z) override;
    void SetPositionX(float x) override;
    void SetPositionY(float y) override;
    void SetPositionZ(float z) override;
    void set_scale(float scale) override;
private:
    void Init();
    void SetVrtxCoord();

    int model_location_ = 0;
    int view_location_ = 0;
    int projection_location_ = 0;

    int color_loaction_ = 0;
};


#endif //CLOUDLARK_OCULUS_DEMO_COLOR_BOX_H
