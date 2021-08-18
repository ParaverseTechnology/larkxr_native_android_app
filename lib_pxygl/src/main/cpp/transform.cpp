//
// Created by fcx@pingxingyun.com on 2019/11/29.
//

#include "transform.h"

namespace lark {
Transform::Transform(): mat_(1.0F) {
}

Transform::Transform(const glm::quat & rotation): mat_(glm::mat4_cast(rotation)) {
}

Transform::Transform(const glm::quat & rotation, const glm::vec3 & position): mat_(1.0F) {
    mat_ = glm::translate(mat_, position);
    mat_ *= glm::mat4_cast(rotation);
}

Transform::Transform(const glm::mat4 & trans): mat_(trans) {
}
}