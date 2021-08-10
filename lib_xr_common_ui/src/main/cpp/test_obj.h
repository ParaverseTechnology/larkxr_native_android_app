//
// Created by fcx on 2020/7/1.
//

#ifndef CLOUDLARKXR_TEST_OBJ_H
#define CLOUDLARKXR_TEST_OBJ_H

#include "object.h"
#include "vertex_array_object.h"

class TestObj: public lark::Object {
public:
    TestObj();
    ~TestObj();

    void InitGL();

    // draw
    virtual void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view);
private:
    void InitVao(void* vertices, int verticesSize, void* indices, int indicesSize, lark::VertexArrayObject * vao);

    int uniform_projection_;
};


#endif //CLOUDLARKXR_TEST_OBJ_H
