//
// Created by fcx@pingxingyun.com on 2019/11/30.
//

#include "aa_bb.h"

AABB::AABB(uint64_t id): id_(id), size_(), center_(), position_() {
}

AABB::AABB(uint64_t id, glm::vec2 size, glm::vec2 position):id_(id), size_(size), position_(position), center_(0,0) {
    center_.x = position.x + size.x / 2;
    center_.y = position.y + size.y / 2;
}

AABB::~AABB() = default;

bool AABB::CheckCollision(const AABB::Ball &ball) {
    // 获取圆的中心
    glm::vec2 center(ball.position + ball.radius);
    glm::vec2 aabb_half_extents(size_.x / 2, size_.y / 2);

    glm::vec2 difference = center - center_;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = center_ + clamped;

    difference = closest - center;
    return glm::length(difference) < ball.radius;
}

bool AABB::CheckPointIn(const glm::vec2 &point) {
    float dx = point.x - position_.x;
    float dy = point.y - position_.y;
    return dx > 0 && dx < size_.x && dy > 0 && dy < size_.y;
}

void AABB::SetAABBSize(const glm::vec2 & size) {
    size_ = size;
}

void AABB::SetAABBBCenter(const glm::vec2 & center) {
    center_ = center;
    position_.x = center.x - size_.x / 2;
    position_.y = center.y - size_.y / 2;
}

void AABB::SetAABBPositon(const glm::vec2 & position) {
    position_ = position;
    center_.x = position.x + size_.x / 2;
    center_.y = position.y + size_.y / 2;
}
