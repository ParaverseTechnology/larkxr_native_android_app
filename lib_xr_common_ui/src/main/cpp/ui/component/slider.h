//
// Created by fcx@pingixngyun.com on 2019/11/13.
//

#ifndef CLOUDLARK_OCULUS_DEMO_SLIDER_H
#define CLOUDLARK_OCULUS_DEMO_SLIDER_H

#include <object.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "color_box.h"
#include "image.h"
#include "border.h"
#include "text.h"
#include "ui/aa_bb.h"

class Slider: public lark::Object, public component::Base, public AABB {
public:
    class Callback {
    public:
        virtual void onSliderValueChanged(float value) = 0;
    };

    Slider(Callback * callback);
    ~Slider() override;

    void SetBgColor(uint64_t rgba);
    void SetActiveColor(uint64_t rgba);
    void set_position(const glm::vec3& position) override;
    void set_position(float x, float y, float z) override;
    void SetPositionX(float x) override;
    void SetPositionY(float y) override;
    void SetPositionZ(float z) override;
    void set_size(const glm::vec2& size) override;
    void SetBgHeight(float height);
    void SetActiveBgHeight(float height);
    /**
     * slider value 0 - 1;
     */
    void setValue(float value);
    void addValue(float vlaue);
    void minusValue(float vlaue);
    float value() { return value_; };
    bool IsMax() { return value_ == 1; }
    bool IsMin() { return value_ == 0; }

    // aabb check
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;
private:
    void Init();
    void Resize();

    uint64_t bg_color_;
    uint64_t active_color_;

    float bg_height_;
    float active_bg_height_;
    float value_;

    std::shared_ptr<ColorBox> slider_bg_;
    std::shared_ptr<Image> slider_lint_;
    std::shared_ptr<ColorBox> slider_bg_active_;

    Callback* callback_;
};


#endif //CLOUDLARK_OCULUS_DEMO_SLIDER_H
