//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#ifndef MY_APPLICATION_VERTEX_ARRAY_OBJECT_H
#define MY_APPLICATION_VERTEX_ARRAY_OBJECT_H

#include "pxygl.h"

namespace lark {
class CLOUDLARK_PXYGL_API VertexArrayObject {
public:
    VertexArrayObject(bool hasAB, bool hasEAB);
    ~VertexArrayObject();

    inline GLuint vertex_array_object() {return vertex_array_object_;}

    inline GLuint array_buffer() {return array_buffer_;}

    inline GLuint element_array_buffer() {return element_array_buffer_;}

    inline void BindVAO() {
        glBindVertexArray(vertex_array_object_);
    }

    inline void UnbindVAO() {
        glBindVertexArray(0);
    }

    inline void BindArrayBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    }

    inline void UnbindArrayBuffer() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    inline void BindElementArrayBuffer() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_);
    }

    inline void UnbindElementArrayBuffer() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

private:
    GLuint vertex_array_object_;
    GLuint array_buffer_;
    GLuint element_array_buffer_;
};
}
#endif //MY_APPLICATION_VERTEX_ARRAY_OBJECT_H
