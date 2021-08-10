//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#ifndef CLOUDLARKVRDEMO_ITEM_BASE_H
#define CLOUDLARKVRDEMO_ITEM_BASE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>

#include "ui/component/base.h"
#include "ui/component/color_box.h"
#include "ui/component/text.h"
#include "ui/aa_bb.h"
#include "object.h"

class ItemBase: public lark::Object, public component::Base, public AABB {
public:
    ItemBase(int group);
    virtual ~ItemBase();

    void set_size(const glm::vec2& size) override;
    void setBgColor(uint64_t rgba);
    void setTitle(const std::wstring & title);

    virtual void Reset() {};
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;
    virtual void FreshData() {};
    virtual void Enter() = 0;
    virtual void Leave() = 0;
    inline int group() const { return group_; }
protected:
    void init();
    void resize();

    void PushAABB(AABB * aabb);
    // clear all aabb;
    void clearAABB();
    // TODO add aaabb remove.

    uint64_t                  bg_color_;
    uint64_t                  bg_color_active_;
    uint64_t                  border_up_color_;

    std::shared_ptr<ColorBox> bg_;
    std::shared_ptr<ColorBox> border_up_;
    std::shared_ptr<Text>     title_;

    // aabb check list.
    std::list<AABB*>          aabb_list_;

    bool                      picked_;
    int                       group_;
};


#endif //CLOUDLARKVRDEMO_ITEM_BASE_H
