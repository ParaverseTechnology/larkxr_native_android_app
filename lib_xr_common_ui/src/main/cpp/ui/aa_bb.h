//
// Created by fcx@pingxingyun.com on 2019/11/30.
//

#ifndef CLOUDLARKVRDEMO_AA_BB_H
#define CLOUDLARKVRDEMO_AA_BB_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <transform.h>

class AABB {
public:
    struct Ball {
        glm::vec2 position;
        float radius;
    };

    AABB(uint64_t id);
    AABB(uint64_t id, glm::vec2 size, glm::vec2 position);
    virtual ~AABB();

    bool CheckCollision(const Ball & ball);
    bool CheckPointIn(const glm::vec2 & point);

    virtual void SetAABBSize(const glm::vec2 & size);
    virtual void SetAABBBCenter(const glm::vec2 & center);
    // AABB positon in global space.
    virtual void SetAABBPositon(const glm::vec2 & position);

     glm::vec2 GetAABBSize() { return size_; }
    inline glm::vec2 GetAABBCenter() { return center_; }
    inline uint64_t GetAABBId() { return id_; }
    inline glm::vec2 GetAABBPosition() { return position_; }

    // TODO object active
    // set active
    inline bool aabb_active() { return active_; }
    inline void set_aabb_active(bool active) { active_ = active; }

    // check input.
    virtual void HandleInput(glm::vec2 * point, int pointCount) = 0;
private:
    bool active_ = true;
    uint64_t  id_;
    glm::vec2 size_;
    glm::vec2 center_;
    glm::vec2 position_;
};


#endif //CLOUDLARKVRDEMO_AA_BB_H
