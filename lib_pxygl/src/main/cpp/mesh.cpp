//
// Created by fcx@pingxingyun.com on 2019/11/12.
//

#include <memory>
#include "mesh.h"
#include "vertex_array_object.h"
#include "logger.h"
#include "asset_loader.h"

#define LOG_TAG "pxygl_mesh"

namespace {
    const char* vertexShaderSource = 
#ifdef __ANDROID__
                                     "#version 300 es\n"
#else
                                     "#version 410 core\n"
#endif
                                     "\n"
                                     "  layout (location = 0) in vec3 aPos;\n"
                                     "  layout (location = 1) in vec3 aNormal;\n"
                                     "  layout (location = 2) in vec2 aTexCoords;\n"
                                     "\n"
                                     "  uniform mat4 uModel;\n"
                                     "  uniform mat4 uView;\n"
                                     "  uniform mat4 uProjection;\n"
                                     "\n"
                                     "  out vec3 FragPos;\n"
                                     "  out vec3 Normal;\n"
                                     "  out vec2 TexCoords;\n"
                                     "void main()\n"
                                     "{\n"
                                     "    TexCoords = aTexCoords;\n"
                                     "    FragPos = vec3(uModel * vec4(aPos, 1.0));\n"
                                     "    Normal = mat3(transpose(inverse(uModel))) * aNormal;\n"
                                     "    gl_Position = uProjection * uView * vec4(FragPos, 1.0);\n"
                                     "}";
    const char * fragmentShaderSource = 
#ifdef __ANDROID__
                                        "#version 300 es\n"
#else
                                        "#version 410 core\n"
#endif
                                        "precision mediump float;\n"
                                        "out vec4 FragColor;\n"
                                        "  struct Light {\n"
                                        "      vec3 position;\n"
                                        "      vec3 ambient;\n"
                                        "      vec3 diffuse;\n"
                                        "      vec3 specular;\n"
                                        "  };\n"
                                        "  in vec3 Normal;\n"
                                        "  in vec3 FragPos;\n"
                                        "  in vec2 TexCoords;\n"
                                        "  uniform vec4 uColor;\n"
                                        "  uniform vec3 uViewPos;\n"
                                        "  uniform float uShininess;\n"
                                        "  uniform Light uLight;\n"
                                        "  uniform sampler2D texture_diffuse1;\n"
                                        "  uniform sampler2D texture_specular1;\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "    // \n"
                                        "    vec4 baseColor = uColor;\n"
                                        "    vec4 diffuse1 = texture(texture_diffuse1, TexCoords);\n"
                                        "    if (diffuse1.a == 0.0) {\n"
                                        "        diffuse1 = baseColor;\n"
                                        "    }\n"
                                        "    vec4 specular1 = texture(texture_specular1, TexCoords);\n"
                                        "    // ambient\n"
                                        "    vec3 ambient = uLight.ambient * diffuse1.rgb;\n"
                                        "    // diffuse \n"
                                        "    vec3 norm = normalize(Normal);\n"
                                        "    vec3 lightDir = normalize(uLight.position - FragPos);\n"
                                        "    float diff = max(dot(norm, lightDir), 0.0);\n"
                                        "    vec3 diffuse = uLight.diffuse * diff * diffuse1.rgb;  \n"
                                        "    \n"
                                        "    // specular\n"
                                        "    vec3 viewDir = normalize(uViewPos - FragPos);\n"
                                        "    vec3 reflectDir = reflect(-lightDir, norm);  \n"
                                        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);\n"
                                        "    vec3 specular = uLight.specular * spec * specular1.rgb;  \n"
                                        "    vec3 result = ambient + diffuse + specular;\n"
                                        "    FragColor = vec4(result, diffuse1.a);\n"
                                        "}";
}

namespace lark {
Mesh::Mesh()
{
    Init();
}

Mesh::Mesh(const std::vector<MeshVertex> &vertex,
           const std::vector<unsigned int> &indices,
           const std::vector<std::shared_ptr<Texture>> & textures)
{
    AddVerties(vertex);
    AddIndices(indices);
    AddTexture(textures);

    Init();
    SetupMesh();
}

Mesh::~Mesh() {
    LOGV("release mesh");
}

void Mesh::Init() {
    LOGV("init mesh");
    // claer error;
    HasGLError();

    name_ = LOG_TAG;
    enable_ = false;

    LoadShader("shader/vertex/mesh_vertex.glsl", "shader/fragment/mesh_fragment.glsl", vertexShaderSource, fragmentShaderSource);

    model_location_ = shader_->GetUniformLocation("uModel");
    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");

    color_loaction_ = shader_->GetUniformLocation("uColor");
    light_position_location_ = shader_->GetUniformLocation("uLight.position");
    light_ambient_location_ = shader_->GetUniformLocation("uLight.ambient");
    light_diffuse_location_ = shader_->GetUniformLocation("uLight.diffuse");
    light_specular_location_ = shader_->GetUniformLocation("uLight.specular");
    view_pos_location_ = shader_->GetUniformLocation("uViewPos");
    shininess_location_ = shader_->GetUniformLocation("uShininess");

    vao_ = std::make_shared<VertexArrayObject>(true, true);
    enable_ = true;
}

void Mesh::SetupMesh() {
    if(!enable_ || vao_ == nullptr) {
        return;
    }
    vao_->BindVAO();
    vao_->BindArrayBuffer();
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(MeshVertex), &vertices_[0], GL_STATIC_DRAW);
    vao_->BindElementArrayBuffer();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)nullptr);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(3 * sizeof(float)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(6 * sizeof(float)));

#ifdef ENABLE_ASSIMP
    // vertex tangent
//    glEnableVertexAttribArray(3);
//    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(8 * sizeof(float)));
    // vertex bitangent
//    glEnableVertexAttribArray(4);
//    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(11 * sizeof(float)));
#endif // ENABLE_ASSIMP

    vao_->UnbindVAO();
    vao_->UnbindArrayBuffer();

    if (HasGLError()) {
        enable_ = false;
    }
}

void Mesh::Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) {
    if (!enable_  || vao_ == nullptr)
        return;

    Object::Draw(eye, projection, eyeView);
    shader_->UseProgram();
    // mvp
    glUniformMatrix4fv(model_location_, 1, GL_FALSE, glm::value_ptr(GetTransforms()));
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(eyeView));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));
    // light properties
    glUniform4fv(color_loaction_, 1, glm::value_ptr(color_));
    glUniform3fv(light_position_location_, 1, glm::value_ptr(light_.posotion));
    glUniform3fv(light_ambient_location_, 1, glm::value_ptr(light_.ambient));
    glUniform3fv(light_diffuse_location_, 1, glm::value_ptr(light_.diffuse));
    glUniform3fv(light_specular_location_, 1, glm::value_ptr(light_.specular));

    glUniform3fv(view_pos_location_, 1, glm::value_ptr(glm::vec3(eyeView[3][0], eyeView[3][1], eyeView[3][2])));
    glUniform1f(shininess_location_, shininess_);

    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    if (!textures_.empty()) {
        //    for(unsigned int i = 0; i < textures_.size(); i++)
        // shader only support 1 texture
        for(unsigned int i = 0; i < 1; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures_[i]->type_name();
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

            // now set the sampler to the correct texture unit
            glUniform1i(shader_->GetUniformLocation((name + number).c_str()), i);
            // and finally bind the texture
            textures_[i]->BindTexture();
        }
    }

    // draw mesh
    vao_->BindVAO();
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    vao_->UnbindVAO();

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
    shader_->UnUseProgram();

    HasGLError();
}
void Mesh::DrawMultiview(const glm::mat4& projection, const glm::mat4& eyeView)
{
    if (!enable_ || vao_ == nullptr)
        return;

    Object::DrawMultiview(projection, eyeView);

    multiview_shader_->UseProgram();
    // mvp
    glUniformMatrix4fv(model_location_, 1, GL_FALSE, glm::value_ptr(GetTransforms()));
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(eyeView));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));
    // light properties
    glUniform4fv(color_loaction_, 1, glm::value_ptr(color_));
    glUniform3fv(light_position_location_, 1, glm::value_ptr(light_.posotion));
    glUniform3fv(light_ambient_location_, 1, glm::value_ptr(light_.ambient));
    glUniform3fv(light_diffuse_location_, 1, glm::value_ptr(light_.diffuse));
    glUniform3fv(light_specular_location_, 1, glm::value_ptr(light_.specular));

    glUniform3fv(view_pos_location_, 1, glm::value_ptr(glm::vec3(eyeView[3][0], eyeView[3][1], eyeView[3][2])));
    glUniform1f(shininess_location_, shininess_);

    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
//    for(unsigned int i = 0; i < textures_.size(); i++)
    // shader only support 1 texture
    for(unsigned int i = 0; i < 1; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures_[i]->type_name();
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
        else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream

        // now set the sampler to the correct texture unit
        glUniform1i(shader_->GetUniformLocation((name + number).c_str()), i);
        // and finally bind the texture
        textures_[i]->BindTexture();
    }

    // draw mesh
    vao_->BindVAO();
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    vao_->UnbindVAO();

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
    multiview_shader_->UnUseProgram();

    HasGLError();
}
}