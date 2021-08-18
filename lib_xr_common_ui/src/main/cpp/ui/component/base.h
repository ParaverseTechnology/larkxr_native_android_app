//
// Created by fcx@pingxingyun.com on 2019/11/11.
//

#ifndef MY_APPLICATION_BASE_H
#define MY_APPLICATION_BASE_H

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace component {
    const static float UNIT_PIXEL_SCALE = 0.005;
    /**
     *  center of the screen.
     */
//    const static glm::vec3 CAMERA_ORI_POSITION(1.0f, 1.5f, 2.0f);
    const static glm::vec3 CAMERA_ORI_POSITION(0, 0, 0);
    // rgba range(0-1)
    typedef glm::vec4 color;
    // xyz
    typedef glm::vec3 position;
    // wh
    typedef glm::vec2 size;
    class Base {
    public:
        Base();
        virtual ~Base() = default;
        // photoshop color rgb. range(0-255). exp : 0xe1e4e5. alpha (0-255)
        virtual void set_color(uint64_t rgb, uint8_t alpha);
        virtual void set_color(uint64_t rgba);
        virtual void set_color(color c);
        virtual color GetColor();
        virtual void set_position(float x, float y, float z);
        virtual void set_position(const position& p);
        virtual void SetPositionX(float x);
        virtual void SetPositionY(float y);
        virtual void SetPositionZ(float z);
        virtual position GetPosition();
        virtual void set_container_size(const size& s);
        virtual size container_size();
        virtual void set_size(const size& s);
        virtual size GetSize();
        virtual void set_scale(float scale);
        virtual float GetScale();
    protected:
        color color_;
        position position_;
        size size_;
        size container_size_;
        float scale_;
    };
}

#endif //MY_APPLICATION_BASE_H
