//
// Created by fcx@pingxingyun.com on 2019/11/11.
//

#ifndef MY_APPLICATION_IMAGE_H
#define MY_APPLICATION_IMAGE_H

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "object.h"
#include "vertex_array_object.h"
#include "base.h"
#include <mutex>
#include <thread>

class Image: public lark::Object, public component::Base {
public:
    class ImageChangeCallback {
    public:
        virtual void OnImageInited(Image* image) = 0;
    };

    enum ImageScaleType {
        CONTAIN = 0,
        FILL,
    };

    Image();
    ~Image() override;

    // call on any thread.
    // init after render.
    void LoadTexture(const char* buffer, int len);
    // call on render thread.
    // reload the image
    void SetPath(const std::string & path, bool isLocal = false);
    void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) override;

    void set_size(const glm::vec2& size) override;
    void set_position(const glm::vec3& position ) override;
    void set_position(float x, float y, float z) override;
    void SetPositionX(float x) override;
    void SetPositionY(float y) override;
    void SetPositionZ(float z) override;
    void set_scale(float scale) override;
    glm::vec2 GetSize() override;
    glm::vec2 GetTextureSize();
    inline void set_callback(ImageChangeCallback* callback) { callback_ = callback; }
private:
    glm::vec2 GetScaledSize();

    int model_location_ = 0;
    int view_location_ = 0;
    int projection_location_ = 0;

    std::string path_ = "";
    bool need_update_cover_ = false;

    ImageScaleType scale_type_ = FILL;

    std::vector<char> image_buffer_ = {};
    bool need_load_ = false;
    ImageChangeCallback* callback_ = nullptr;
    std::mutex load_mutex_;
};


#endif //MY_APPLICATION_IMAGE_H
