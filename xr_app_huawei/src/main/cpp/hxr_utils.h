//
// Created by fcx@pingxingyung.com on 2022/11/8.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_HXR_UTILS_H
#define LARKXR_HXR_UTILS_H

#include "jni.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "log.h"
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <openxr/openxr_platform.h>
#include <openxr/openxr_platform_defines.h>
#include "matrix_functions.h"

namespace hxr {
    inline glm::mat4 toGlm(const XrMatrix4x4f& m) {
        return glm::mat4(
/*                m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
                m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
                m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
                m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]   */

                m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3],
                m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3],
                m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3],
                m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]
        );
    }
    inline glm::vec3 toGlm(const XrVector3f& v) {
        return glm::vec3(v.x, v.y, v.z);
    }
    inline glm::quat toGlm(const XrQuaternionf& q) {
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    inline XrVector3f fromGlm(const glm::vec3& v) {
        return  { v.x, v.y, v.z };
    }
    inline XrQuaternionf fromGlm(const glm::quat& q) {
        return { q.x, q.y, q.z, q.w };
    }
}

#endif //LARKXR_HXR_UTILS_H
