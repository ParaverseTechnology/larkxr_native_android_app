//
// Created by fcx@pingixngyun.com on 2019/11/13.
//

#ifndef CLOUDLARK_OCULUS_DEMO_BORDER_H
#define CLOUDLARK_OCULUS_DEMO_BORDER_H

#include <memory>
#include <object.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "color_box.h"
#include "base.h"

/**
 *  border by 4 color box
 *   colorboxs
 *        a
 *      -----
 *      !   !
 *    b !    ! d
 *      !    !
 *      ------
 *        c
 */
class Border: public lark::Object, public component::Base {
public:
    Border();
    ~Border() override;

    void set_size(const component::size& size) override;
    void set_position(const component::position& position) override;
    void set_color(uint64_t rgb, uint8_t alpha) override;
    void set_color(uint64_t rgba) override;
    void set_color(component::color c) override;
    void setLineWidth(float width);
    inline float line_width() { return  line_width_; }
private:
    void Resize();

    float line_width_;
    std::shared_ptr<ColorBox> border_up_;
    std::shared_ptr<ColorBox> border_left_;
    std::shared_ptr<ColorBox> border_right_;
    std::shared_ptr<ColorBox> border_down_;
};


#endif //CLOUDLARK_OCULUS_DEMO_BORDER_H
