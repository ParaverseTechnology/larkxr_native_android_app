//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#include "vertex_array_object.h"

namespace lark {
VertexArrayObject::VertexArrayObject(bool hasAB, bool hasEAB):
     vertex_array_object_(0), 
     array_buffer_(0), 
     element_array_buffer_(0) 
{
    glGenVertexArrays(1, &vertex_array_object_);
    glBindVertexArray(vertex_array_object_);

    if (hasAB)
        glGenBuffers(1, &array_buffer_);

    if (hasEAB)
        glGenBuffers(1, &element_array_buffer_);
}

VertexArrayObject::~VertexArrayObject() {
    glDeleteVertexArrays(1, &vertex_array_object_);
    glDeleteBuffers(1, &array_buffer_);
    glDeleteBuffers(1, &element_array_buffer_);
}
}