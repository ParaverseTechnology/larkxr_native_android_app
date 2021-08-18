//
// Created by Administrator on 2021/7/6.
//

#ifndef CLOUDLARKXR_PVR_XR_UTILS_H
#define CLOUDLARKXR_PVR_XR_UTILS_H

#include "pch.h"
#include "common.h"
#include <common/xr_linear.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Math {
    namespace Pose {
        inline XrPosef Identity() {
            XrPosef t{};
            t.orientation.w = 1;
            return t;
        }

        inline XrPosef Translation(const XrVector3f& translation) {
            XrPosef t = Identity();
            t.position = translation;
            return t;
        }

        inline XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation) {
            XrPosef t = Identity();
            t.orientation.x = 0.f;
            t.orientation.y = std::sin(radians * 0.5f);
            t.orientation.z = 0.f;
            t.orientation.w = std::cos(radians * 0.5f);
            t.position = translation;
            return t;
        }
    }  // namespace Pose
}  // namespace Math

namespace pxrutils {
    inline std::string GetXrVersionString(XrVersion ver) {
        return Fmt("%d.%d.%d", XR_VERSION_MAJOR(ver), XR_VERSION_MINOR(ver), XR_VERSION_PATCH(ver));
    }

    inline XrFormFactor GetXrFormFactor(const std::string& formFactorStr) {
        if (EqualsIgnoreCase(formFactorStr, "Hmd")) {
            return XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        }
        if (EqualsIgnoreCase(formFactorStr, "Handheld")) {
            return XR_FORM_FACTOR_HANDHELD_DISPLAY;
        }
        throw std::invalid_argument(Fmt("Unknown form factor '%s'", formFactorStr.c_str()));
    }

    inline XrViewConfigurationType GetXrViewConfigurationType(const std::string& viewConfigurationStr) {
        if (EqualsIgnoreCase(viewConfigurationStr, "Mono")) {
            return XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO;
        }
        if (EqualsIgnoreCase(viewConfigurationStr, "Stereo")) {
            return XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        }
        throw std::invalid_argument(Fmt("Unknown view configuration '%s'", viewConfigurationStr.c_str()));
    }

    inline XrEnvironmentBlendMode GetXrEnvironmentBlendMode(const std::string& environmentBlendModeStr) {
        if (EqualsIgnoreCase(environmentBlendModeStr, "Opaque")) {
            return XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
        }
        if (EqualsIgnoreCase(environmentBlendModeStr, "Additive")) {
            return XR_ENVIRONMENT_BLEND_MODE_ADDITIVE;
        }
        if (EqualsIgnoreCase(environmentBlendModeStr, "AlphaBlend")) {
            return XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND;
        }
        throw std::invalid_argument(Fmt("Unknown environment blend mode '%s'", environmentBlendModeStr.c_str()));
    }
    inline XrReferenceSpaceCreateInfo GetXrReferenceSpaceCreateInfo(const std::string& referenceSpaceTypeStr) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Identity();
        if (EqualsIgnoreCase(referenceSpaceTypeStr, "View")) {
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "ViewFront")) {
            // Render head-locked 2m in front of device.
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Translation({0.f, 0.f, -2.f}),
                    referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Local")) {
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Stage")) {
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeft")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, {-2.f, 0.f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRight")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, {2.f, 0.f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeftRotated")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(3.14f / 3.f, {-2.f, 0.5f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRightRotated")) {
            referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(-3.14f / 3.f, {2.f, 0.5f, -2.f});
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        } else {
            throw std::invalid_argument(Fmt("Unknown reference space type '%s'", referenceSpaceTypeStr.c_str()));
        }
        return referenceSpaceCreateInfo;
    }
}

namespace pvr {
    inline glm::mat4 toGlm(const XrMatrix4x4f& m) {
        return glm::mat4(
                m.m[0], m.m[1], m.m[2], m.m[3],
                m.m[4], m.m[5], m.m[6], m.m[7],
                m.m[8], m.m[9], m.m[10], m.m[11],
                m.m[12], m.m[13], m.m[14], m.m[15]
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
} // namespace pvr



#endif //CLOUDLARKXR_PVR_XR_UTILS_H
