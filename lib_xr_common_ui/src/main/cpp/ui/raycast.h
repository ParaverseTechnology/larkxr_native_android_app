//
// Created by fcx on 2020/4/14.
//

#ifndef CLOUDLARKXR_RAYCAST_H
#define CLOUDLARKXR_RAYCAST_H

#include "object.h"

class Raycast: public lark::Object {
public:
    Raycast();

    inline void set_color(const glm::vec4& color) { color_ = color; };
    virtual void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) override;
private:
    void Init();
    void InitVAO(void *vertices, int verticesSize, lark::VertexArrayObject *vao);

    glm::vec4 color_ = { 1.0, 1.0, 1.0, 1.0 };

    int model_location_ = 0;
    int view_location_ = 0;
    int projection_location_ = 0;

    int color_loaction_ = 0;
};


#endif //CLOUDLARKXR_RAYCAST_H
