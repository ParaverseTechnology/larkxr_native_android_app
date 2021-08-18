//
// Created by fcx on 2020/7/1.
//

#include "test_obj.h"
#include "log.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#define LOG_TAG "test_obj"

namespace {
    // uv
    //  左上角          右上角
    // (0,0)---(0.5,0)---(1,0)
    //  |        |        |
    // (0,1)---(0.5,1)---(1,1)
    //  左下角           右下角

    static const float verticesTexture[] = {
            // 位置              // 颜色           - 纹理坐标 -
//            -1.0F, -1.0F, -1.0F, 1.0F, 0.0F, 0.0F,  0.0F, 1.0F,  // 左下角
//            1.0F, -1.0F, -1.0F,  0.0F, 1.0F, 0.0F,  0.5F, 1.0F,  // 右下角
//            1.0F, 1.0F, -1.0F,   0.0F, 0.0F, 1.0F,  0.5F, 0.0F,  // 右上角
//            -1.0F, 1.0F, -1.0F,  0.0F, 0.0F, 1.0F,  0.0F, 0.0F   // 左上角

            // 位置              // 颜色           - 纹理坐标 -
            -1.0F, -1.0F, 0.0F, 1.0F, 0.0F, 0.0F,  0.0F, 1.0F,  // 左下角
            1.0F, -1.0F, 0.0F,  0.0F, 1.0F, 0.0F,  0.5F, 1.0F,  // 右下角
            1.0F, 1.0F, 0.0F,   0.0F, 0.0F, 1.0F,  0.5F, 0.0F,  // 右上角
            -1.0F, 1.0F, 0.0F,  0.0F, 0.0F, 1.0F,  0.0F, 0.0F   // 左上角
            // 位置              // 颜色           - 纹理坐标 -
//            -1.0F, -1.0F, -1.0F,
//            1.0F, 0.0F, 0.0F,  0.0F, 0.0F,  // 左下角
//            1.0F, -1.0F, -1.0F,
//            0.0F, 1.0F, 0.0F,  1.0F, 0.0F,  // 右下角
//            -1.0F, 1.0F, -1.0F,
//            0.0F, 0.0F, 1.0F,  0.0F, 1.0F,  // 左上角

//            1.0F, -1.0F, -1.0F,
//            1.0F, 0.0F, 0.0F,  0.0F, 0.0F,  // 左下角
//            1.0F, 1.0F, -1.0F,
//            0.0F, 0.0F, 1.0F,  1.0F, 1.0F,  // 右上角
//            -1.0F, 1.0F, -1.0F,
//            0.0F, 0.0F, 1.0F,  0.0F, 1.0F,  // 左上角
            // front
//            -1.0F, -1.0F,  1.0F,
//            -1.0F,  1.0F,  1.0F,
//            1.0F,  1.0F,  1.0F,
//            1.0F,  1.0F,  1.0F,
//            1.0F, -1.0F,  1.0F,
//            -1.0F, -1.0F,  1.0F,
            // back
//            -1.0F,  1.0F, -1.0F,
//            -1.0F, -1.0F, -1.0F,
//            1.0F, -1.0F, -1.0F,
//            1.0F, -1.0F, -1.0F,
//            1.0F,  1.0F, -1.0F,
//            -1.0F,  1.0F, -1.0F,
    };

    GLushort indicesAll[] = { // 注意索引从0开始!
            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
    };

    static const float SkyBoxVertices[] = {
            // Positions
            // right
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,

            // left
            -1.0F, -1.0F,  1.0F,
            -1.0F, -1.0F, -1.0F,
            -1.0F,  1.0F, -1.0F,
            -1.0F,  1.0F, -1.0F,
            -1.0F,  1.0F,  1.0F,
            -1.0F, -1.0F,  1.0F,

            // up
            -1.0F,  1.0F, -1.0F,
            1.0F,  1.0F, -1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            -1.0F,  1.0F,  1.0F,
            -1.0F,  1.0F, -1.0F,

            // down
            -1.0F, -1.0F, -1.0F,
            -1.0F, -1.0F,  1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            -1.0F, -1.0F,  1.0F,
            1.0F, -1.0F,  1.0F,

            // front
            -1.0F, -1.0F,  1.0F,
            -1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F,  1.0F,  1.0F,
            1.0F, -1.0F,  1.0F,
            -1.0F, -1.0F,  1.0F,

            // back
            -1.0F,  1.0F, -1.0F,
            -1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F, -1.0F, -1.0F,
            1.0F,  1.0F, -1.0F,
            -1.0F,  1.0F, -1.0F,
    };

    const char* vertexShaderSource = "#version 300 es\n"
                                     "layout (location = 0) in vec3 position;\n"
                                     "layout (location = 1) in vec3 color;\n"
                                     "layout (location = 2) in vec3 textureCoord;\n"
                                     "uniform mat4 projection;\n"
                                     "out vec3 v3fCoord;\n"
                                     "out vec3 v3fColor;\n"
                                     "out vec3 v3fTextCoord;\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    gl_Position = projection * vec4(position.xyz, 1.0);\n"
//                                     "    gl_Position = vec4(position, 1.0);\n"
                                     "    v3fCoord = position;\n"
                                     "    v3fColor = color;\n"
                                     "    v3fTextCoord = textureCoord;\n"
                                     "}";
    const char * fragmentShaderSource = "#version 300 es\n"
                                        "precision mediump float;\n"
                                        "in vec3 v3fCoord;\n"
                                        "in vec3 v3fColor;\n"
                                        "out vec4 FragColor;\n"
//                                        "uniform samplerCube atexture;\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
//                                        "    FragColor = texture(atexture, v3fCoord);\n"
                                        "    FragColor = vec4(v3fColor, 1.0);\n"
                                        "}";
}

namespace PVR
{

    void PvrCheckGlError(const char* file, int line)
    {
        for (GLint error = glGetError(); error; error = glGetError())
        {
            char *pError;
            switch (error)
            {
                case GL_NO_ERROR:                       pError = (char *)"GL_NO_ERROR";                         break;
                case GL_INVALID_ENUM:                   pError = (char *)"GL_INVALID_ENUM";                     break;
                case GL_INVALID_VALUE:                  pError = (char *)"GL_INVALID_VALUE";                    break;
                case GL_INVALID_OPERATION:              pError = (char *)"GL_INVALID_OPERATION";                break;
                case GL_OUT_OF_MEMORY:                  pError = (char *)"GL_OUT_OF_MEMORY";                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:  pError = (char *)"GL_INVALID_FRAMEBUFFER_OPERATION";    break;

                default:
                    LOGE("glError (0x%x) %s:%d\n", error, file, line);
                    return;
            }

            LOGE("glError (%s) %s:%d\n", pError, file, line);
            return;
        }
        return;
    }

    void PvrCheckEglError(const char* file, int line)
    {
        for (int i = 0; i < 10; i++)
        {
            const EGLint error = eglGetError();
            if (error == EGL_SUCCESS)
            {
                break;
            }

            char *pError;
            switch (error)
            {
                case EGL_SUCCESS:				pError = (char *)"EGL_SUCCESS"; break;
                case EGL_NOT_INITIALIZED:		pError = (char *)"EGL_NOT_INITIALIZED"; break;
                case EGL_BAD_ACCESS:			pError = (char *)"EGL_BAD_ACCESS"; break;
                case EGL_BAD_ALLOC:				pError = (char *)"EGL_BAD_ALLOC"; break;
                case EGL_BAD_ATTRIBUTE:			pError = (char *)"EGL_BAD_ATTRIBUTE"; break;
                case EGL_BAD_CONTEXT:			pError = (char *)"EGL_BAD_CONTEXT"; break;
                case EGL_BAD_CONFIG:			pError = (char *)"EGL_BAD_CONFIG"; break;
                case EGL_BAD_CURRENT_SURFACE:	pError = (char *)"EGL_BAD_CURRENT_SURFACE"; break;
                case EGL_BAD_DISPLAY:			pError = (char *)"EGL_BAD_DISPLAY"; break;
                case EGL_BAD_SURFACE:			pError = (char *)"EGL_BAD_SURFACE"; break;
                case EGL_BAD_MATCH:				pError = (char *)"EGL_BAD_MATCH"; break;
                case EGL_BAD_PARAMETER:			pError = (char *)"EGL_BAD_PARAMETER"; break;
                case EGL_BAD_NATIVE_PIXMAP:		pError = (char *)"EGL_BAD_NATIVE_PIXMAP"; break;
                case EGL_BAD_NATIVE_WINDOW:		pError = (char *)"EGL_BAD_NATIVE_WINDOW"; break;
                case EGL_CONTEXT_LOST:			pError = (char *)"EGL_CONTEXT_LOST"; break;
                default:
                    LOGE("eglError (0x%x) %s:%d\n", error, file, line);
                    return;
            }
            LOGE("eglError (%s) %s:%d\n", pError, file, line);
            return;
        }
        return;
    }

}//End namespace Pvr

#define GL( func ) func; PVR::PvrCheckGlError(__FILE__,__LINE__);
#define EGL( func ) func; PVR::PvrCheckEglError(__FILE__,__LINE__);

using namespace lark;

TestObj::TestObj() {
    InitGL();
}

TestObj::~TestObj() {

}

void TestObj::InitGL() {
    HasGLError();

    name_ = LOG_TAG;

    LoadShader("shader/vertex/test_obj_vertex.glsl", "shader/fragment/test_obj_fragment.glsl",
               vertexShaderSource, fragmentShaderSource);

    if (has_error_) {
        LOGW("load shader failed.");
        return;
    }

    uniform_projection_ = shader_->GetUniformLocation("projection");
    vao_ = std::make_shared<VertexArrayObject>(true, true);
//    vao_->BindVAO();
//    vao_->BindArrayBuffer();
////    glBufferData(GL_ARRAY_BUFFER, sizeof(SkyBoxVertices), SkyBoxVertices, GL_STATIC_DRAW);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTexture), verticesTexture, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
//    vao_->UnbindArrayBuffer();
//    vao_->UnbindVAO();

    InitVao((void*)verticesTexture, sizeof(verticesTexture), (void*)indicesAll, sizeof(indicesAll), vao_.get());
}

void TestObj::Draw(lark::Object::Eye eye, const glm::mat4 &projection, const glm::mat4 &view) {
    Object::Draw(eye, projection, view);
    if (!shader_ || !vao_) {
        LOGW("test obj drwa failed %p %p", shader_.get(), vao_.get());
        return;
    }
    PVR::PvrCheckGlError(__FILE__,__LINE__);
    glm::mat4 viewClone = view;
    viewClone[3][0] = 0;
    viewClone[3][1] = 0;
    viewClone[3][2] = 0;
    viewClone[3][3] = 1;
    glm::mat4 mvp = projection * viewClone * GetTransforms();
//    glm::mat4 mvp = projection * viewClone;
    lark::VertexArrayObject * vao = vao_.get();

    PVR::PvrCheckGlError(__FILE__,__LINE__);
    shader_->UseProgram();
    PVR::PvrCheckGlError(__FILE__,__LINE__);
    glUniformMatrix4fv(uniform_projection_, 1, GL_FALSE, glm::value_ptr(mvp));
    PVR::PvrCheckGlError(__FILE__,__LINE__);
//    vao->BindArrayBuffer();
    vao->BindVAO();
    PVR::PvrCheckGlError(__FILE__,__LINE__);
//    vao->BindElementArrayBuffer();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    PVR::PvrCheckGlError(__FILE__,__LINE__);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indicesAll);
    vao->UnbindVAO();
    PVR::PvrCheckGlError(__FILE__,__LINE__);
//    vao->UnbindArrayBuffer();
//    vao->UnbindElementArrayBuffer();
    shader_->UnUseProgram();
    PVR::PvrCheckGlError(__FILE__,__LINE__);

//    glDisable(GL_DEPTH_TEST);
//    glDepthMask(GL_FALSE);

//    shader_->UseProgram();
//    glUniformMatrix4fv(matrix_location_, 1, GL_FALSE, glm::value_ptr(matrix));
//    glActiveTexture(GL_TEXTURE0);
//    texture_->BindTextureCubeMap();
//    glUniform1i(texture_location_, 0);
//    vao_->BindVAO();
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDrawArrays(GL_TRIANGLES, 0, 36);

//    vao_->UnbindVAO();
//    texture_->UnbindTextureCubeMap();
//    shader_->UnUseProgram();

//    glDepthMask(GL_TRUE);
//    glEnable(GL_DEPTH_TEST);
}

void TestObj::InitVao(void *vertices, int verticesSize, void *indices, int indicesSize,
                      lark::VertexArrayObject *vao) {
    vao->BindVAO();
    vao->BindArrayBuffer();
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STREAM_DRAW);
    vao->BindElementArrayBuffer();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    vao->UnbindVAO();
    vao->UnbindArrayBuffer();
}
