//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#ifndef CLOUDLARK_OCULUS_DEMO_COVER_ITEM_H
#define CLOUDLARK_OCULUS_DEMO_COVER_ITEM_H

#include <string.h>
#include <object.h>
#include <ui/component/border.h>
#include <ui/component/image.h>
#include <ui/component/text.h>
#include <ui/aa_bb.h>
#include "ui/component/color_box.h"
#include "lark_xr/request/cover_loader.h"
#include "lark_xr/lk_common_types.h"

class Navigation;
class CoverItem: public lark::Object, public AABB, public lark::CoverLoader::CoverLoaderCallback {
public:
    static const int PICKED_NONE = -1;

    explicit CoverItem(Navigation * navigation, const std::wstring& text);
    ~CoverItem() override;

    inline void set_app_id(const std::string& appId) { app_id_ = appId; }
    void SetTitle(const std::wstring& title);
    void SetTail(const std::wstring& tail);
    /**
     * set cover use net url.
     * @param coverUrl
     */
    void SetCoverUrl(const std::string& coverUrl, bool isLocal = false);
    void SetNormalBgColor(const glm::vec4& normalColor);
    void SetActiveBgColor(const glm::vec4& activeColor );
    void SetAppliType(larkAppliType type);

    // object
    void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) override;
    // aabb check
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;

    inline bool picked() { return picked_; }
    inline bool is_empty() { return is_empty_; }
    inline void set_is_empty(bool empty) { is_empty_ = empty; };

    // cover loader
    virtual void OnImageLoadSuccess(const char* data, int size) override;
    virtual void OnImageLoadFailed(const std::string& err) override;
private:
    Navigation* navigation_;

    glm::vec4 bg_color_dark_;
    glm::vec4 bg_color_active_;
    glm::vec2 size_;

//    Point3D mOriPoint;
    bool picked_;

    // appli id
    std::string app_id_;
    Text title_;
    Text tail_;
    Text app_type_icon_;
    std::string cover_url_;
    ColorBox bg_color_;
    std::shared_ptr<Image> cover_;
    std::shared_ptr<Image> border_;
    std::shared_ptr<Image> active_border_;
    std::shared_ptr<Border> test_border_;

    bool need_update_cover_;
    GLuint cover_texture_id_;

    bool is_empty_;
    lark::CoverLoader cover_loader_;

    int appli_type_ = larkAppliType::AppliType_VR;
};


#endif //CLOUDLARK_OCULUS_DEMO_COVER_ITEM_H
