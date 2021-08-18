//
// Created by fcx@pingxingyun.com on 2019/11/29.
//

#ifndef CLOUDLARKVRDEMO_TRANSFORM_H
#define CLOUDLARKVRDEMO_TRANSFORM_H

#include "pxygl.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace lark {
typedef struct Ray_ {
    glm::vec3 ori;
    glm::vec3 dir;
} Ray;

typedef struct Plane_ {
    glm::vec3 normal;
    glm::vec3 dot;
} Plane;

class CLOUDLARK_PXYGL_API Transform {
public:
    Transform();
    Transform(const glm::quat & rotation);
    Transform(const glm::quat & rotation, const glm::vec3 & position);
    Transform(const glm::mat4 & trans);

    //
    inline glm::mat4 GetTrans() const { return mat_; }
    //
    inline glm::vec3 GetPosition() {
        return glm::vec3(mat_[3][0], mat_[3][1], mat_[3][2]);
    }
    //
    inline void SetPositon(const glm::vec3 & p) {
        mat_[3][0] = p.x;
        mat_[3][1] = p.y;
        mat_[3][2] = p.z;
    }
    //
    inline glm::quat GetRotation() {
        return glm::quat(mat_);
    }
    //
    inline void Translate(float x, float y, float z) {
        mat_ = glm::translate(mat_, glm::vec3(x, y, z));
    }
    //
    inline void Translate(const glm::vec3 & p) {
        mat_ = glm::translate(mat_, p);
    }
    //
    inline void Sacle(float scale) {
        mat_ = glm::scale(mat_, glm::vec3(scale, scale, scale));
    }
    //
    inline void Rotate(const glm::quat & rotation) {
        mat_ *= glm::mat4_cast(rotation);
    }
    //
    inline void Rotate(float ang, const glm::vec3 & point) {
        mat_ = glm::rotate(mat_, ang, point);
    }
    //
    inline glm::vec3 EulerAngles() { return glm::eulerAngles(glm::quat(mat_)); }
    //
    inline void LookAt(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up) {
        mat_ = glm::lookAt(eye, center, up);
    }
    inline glm::vec3 Forward() {
        return glm::vec3(mat_[2][0], mat_[2][1], mat_[2][2]);
    }
    inline glm::vec3 Up() {
        return glm::vec3(mat_[1][0], mat_[1][1], mat_[1][2]);
    }
private:
    glm::mat4 mat_;
};
}

#endif //CLOUDLARKVRDEMO_TRANSFORM_H
