//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#ifndef MY_APPLICATION_SHADER_H
#define MY_APPLICATION_SHADER_H

#include "pxygl.h"

#include <vector>
#include <memory>

namespace lark {
class CLOUDLARK_PXYGL_API Shader {
public:
    Shader(const char* name, const char* vname, const char* vertex,
        const char* fname, const char* fragment);

    ~Shader();

    inline void UseProgram() {
        glUseProgram(program_id_);
    }

    inline void UnUseProgram() {
        glUseProgram(0);
    }

    inline GLuint program_id() {
        return program_id_;
    }
    bool Compile();
    int GetUniformLocation(const char* name);
    int GetAttributeLocation(const char* name);
private:
    bool HasShaderError(const char* type, int shaderId);

    const char* name_;
    const char* vname_;
    const char* fname_;
    const char* vertex_shader_;
    const char* fragment_shader_;
    GLuint program_id_;
};
}

#endif //MY_APPLICATION_SHADER_H
