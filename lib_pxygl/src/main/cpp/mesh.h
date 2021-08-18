//
// Created by fcx@pingxingyun.com on 2019/11/12.
//

#ifndef CLOUDLARK_OCULUS_DEMO_MESH_H
#define CLOUDLARK_OCULUS_DEMO_MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "texture.h"
#include "object.h"

namespace lark {
struct MeshVertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
#ifdef ENABLE_ASSIMP
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
#endif // ENABLE_ASSIMP
};

struct MeshLight {
    glm::vec3 posotion;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class CLOUDLARK_PXYGL_API Mesh: public Object  {
public:
    Mesh();
    Mesh(const std::vector<MeshVertex> & vertex,const std::vector<unsigned int> & indices, const std::vector<std::shared_ptr<Texture>> & textures);
    ~Mesh();
    // call after data setup.
    void SetupMesh();

    void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view) override;
    void DrawMultiview(const glm::mat4& projection, const glm::mat4& view) override;

    inline void AddVerties(const MeshVertex & vertex) { vertices_.push_back(vertex); }
    inline void AddVerties(const std::vector<MeshVertex> & vertex) {
        vertices_.insert(vertices_.end(), vertex.begin(), vertex.end());
    }
    inline void AddIndices(unsigned int indices) { indices_.push_back(indices); }
    inline void AddIndices(std::vector<unsigned int> indices) {
        indices_.insert(indices_.end(), indices.begin(), indices.end());
    }
    inline void AddTexture(Texture * texture) {
        std::shared_ptr<Texture> t(texture);
        textures_.push_back(t);
    };
    inline void AddTexture(const std::shared_ptr<Texture> & texture) { textures_.push_back(texture); }
    inline void AddTexture(const std::vector<std::shared_ptr<Texture>> & textures) {
        textures_.insert(textures_.end(), textures.begin(), textures.end());
    }
    inline std::vector<MeshVertex>& vertices() { return  vertices_; }
    inline std::vector<unsigned int>& indices() { return indices_; }
    inline std::vector<std::shared_ptr<Texture>>& textures() { return textures_; }

    // set color
    inline void set_color(const glm::vec4& color) { color_ = color; }
    // set light
    inline void set_light(const MeshLight& light) { light_ = light; }
    //
    inline void set_shininess(float shininess) { shininess_ = shininess; }
private:
    void Init();

    /*  Mesh Data  */
    std::vector<MeshVertex> vertices_ = {};
    std::vector<unsigned int> indices_ = {};
    std::vector<std::shared_ptr<Texture>> textures_ = {};

    int model_location_ = 0;
    int view_location_ = 0;
    int projection_location_ = 0;

    int color_loaction_ = 0;
    int light_position_location_ = 0;
    int light_ambient_location_ = 0;
    int light_diffuse_location_ = 0;
    int light_specular_location_ = 0;
    int view_pos_location_ = 0;
    int shininess_location_ = 0;

    float shininess_ = 0.5;
    glm::vec4 color_ = { 0.08F, 0.08F, 0.08F, 1.0F };
    MeshLight light_ = {
            { -100, 100, 100 },
            { 0.1f, 0.1f, 0.1f },
            { 0.8f, 0.8f, 0.8f },
            { 0.1f, 0.1f, 0.1f },
    };
};
}

#endif //CLOUDLARK_OCULUS_DEMO_MESH_H
