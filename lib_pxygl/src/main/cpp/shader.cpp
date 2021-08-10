//
// Created by fcx@pingxingyun.com on 2019/11/7.
//

#include "shader.h"
#include "logger.h"
#define LOG_TAG "shader"

namespace lark {
Shader::Shader(const char * name, const char * vname, const char * vertex, const char * fname, const char * fragment) :
        name_(name), vname_(vname), fname_(fname), vertex_shader_(vertex), fragment_shader_(fragment), program_id_(0) {
}

Shader::~Shader() {
    if (program_id_ == 0)
        return;
    glDeleteProgram(program_id_);
    program_id_ = 0;
}

bool Shader::HasShaderError(const char * name, int shaderId) {
    GLint result = GL_FALSE;
    int infoLength = 0;
    glGetShaderiv(static_cast<GLuint>(shaderId), GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        glGetShaderiv(static_cast<GLuint>(shaderId), GL_INFO_LOG_LENGTH, &infoLength);
        std::vector<GLchar> errorMessage(static_cast<unsigned int>(infoLength + 1));
        glGetShaderInfoLog(static_cast<GLuint>(shaderId), infoLength, nullptr, &errorMessage[0]);
        // std::cout << "Unable to compile shader" << std::string(name) << ";" << shaderId << ";" << std::string(&errorMessage[0]) << std::endl;
        LOGE("Unable to compile shader \"%s\" (%d):\n%s", name, shaderId, &errorMessage[0]);
        return false;
    }
    return true;
}

bool Shader::Compile() {
    if (program_id_ != 0)
        return false;

    if (vertex_shader_ == nullptr) {
        return false;
    } else {
        if (fragment_shader_ == nullptr)
            return false;
    }

    program_id_ = glCreateProgram();

    int vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex_shader_, nullptr);
    glCompileShader(vshader);
    if (!HasShaderError(vname_, vshader)) {
        glDeleteShader(vshader);
        glDeleteProgram(program_id_);
        program_id_ = 0;
        return false;
    }
    glAttachShader(program_id_, vshader);
    glDeleteShader(vshader);

    int fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragment_shader_, nullptr);
    glCompileShader(fshader);
    if (!HasShaderError(fname_, fshader)) {
        glDeleteShader(vshader);
        glDeleteShader(fshader);
        glDeleteProgram(program_id_);
        program_id_ = 0;
        return false;
    }
    glAttachShader(program_id_, fshader);
    glDeleteShader(fshader);

    glLinkProgram(program_id_);
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        LOGE("%s - Error linking program %d!\n", name_, program_id_);
        glDeleteProgram( program_id_ );
        program_id_ = 0;
        return false;
    }

    vertex_shader_ = nullptr;
    fragment_shader_ = nullptr;
    glUseProgram(program_id_);
    glUseProgram(0);

    LOGD("%s - Program %d Compiled", name_, program_id_);
    return true;
}

int Shader::GetUniformLocation(const char * name) {
    int location = glGetUniformLocation(program_id_, name);
    if (location == -1)
        LOGE("Unable to find \"%s\" uniform in \"%s\" shader(%d)", name, name_, program_id_);
    return location;
}

int Shader::GetAttributeLocation(const char * name) {
    int location = glGetAttribLocation(program_id_, name);
    if (location == -1)
        LOGE("Unable to find \"%s\" attrib in \"%s\" shader(%d)", name, name_, program_id_);
    return location;
}
}