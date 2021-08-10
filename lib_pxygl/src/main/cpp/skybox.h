//
// Created by fcx on 2020/4/13.
//

#ifndef CLOUDLARKXR_SKYBOX_H
#define CLOUDLARKXR_SKYBOX_H

#include "object.h"
#include "bitmap_factory.h"
#include "pxygl.h"

namespace lark {
class Texture;
class VertexArrayObject;
class Shader;

class CLOUDLARK_PXYGL_API SkyBox : public Object {
public:
    SkyBox(const char* path);
    ~SkyBox();

    inline const glm::vec4& light_dir() { return light_dir_; }

    virtual void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) override;
private:
    void InitVertices();

    std::string path_ = "";
    int view_location_ = 0;
    int projection_location_ = 0;
    int texture_location_ = 0;
    int multiview_matrix_location_ = 0;
    int multiview_texture_location_ = 0;
    glm::vec4 light_dir_;
    const int vertices_ = 36;
};
}

#endif //CLOUDLARKXR_SKYBOX_H
