//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#ifndef CLOUDLARK_OCULUS_DEMO_BUTTON_H
#define CLOUDLARK_OCULUS_DEMO_BUTTON_H

#include <string.h>
#include <object.h>
// TODO add ray cast
//#include <scene/ray_sphereIntersection.h>
#include "color_box.h"
#include "image.h"
#include "text.h"
#include "ui/aa_bb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
class Button: public lark::Object, public AABB, public component::Base {
public:
    Button();
    Button(const std::wstring &txt);

    inline bool picked() { return picked_; }
    inline bool pressed() { return pressed_; }

    inline void ClearStatus() {
        picked_ = false;
        pressed_ = false;
    }

    // component base.
    virtual glm::vec2 GetSize() override ;
    virtual void set_color(component::color color) override;
    virtual void SetPositionZ(float z) override;

    void SetText(const std::wstring & txt);
    std::wstring GetText();
    void SetFontSize(int fontSize);
    // aabb
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;
protected:
//    Point3D mOriPoint;
    bool picked_;
    bool pressed_;

    std::shared_ptr<Text> title_;
    std::shared_ptr<Image> icon_;
    std::shared_ptr<Image> hover_icon_;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class SetupButton : public Button {
public:
    SetupButton();
protected:
    void init();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class BackButton: public Button {
public:
    BackButton();
protected:
    void init();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class ResetButton: public Button {
public:
    ResetButton();
protected:
    void init();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class PageButton: public Button {
public:
    explicit PageButton(bool isLeft);
protected:
    void init();

private:
    bool mIsLeft;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class TextButton: public Button {
public:
    explicit TextButton(const std::wstring &txt);
protected:
    void init();
};
#endif //CLOUDLARK_OCULUS_DEMO_BUTTON_H
