//
// Created by Administrator on 2021/7/15.
//

#ifndef CLOUDLARKXR_XMS_SCENE_LOCAL_H
#define CLOUDLARKXR_XMS_SCENE_LOCAL_H

#include <ui/component/color_box.h>
#include <ui/component/border.h>
#include <ui/component/slider.h>
#include <ui/home/home.h>
#include <ui/controller.h>
#include <ui/loading/loading.h>
#include <test_obj.h>
#include "ui/navigation.h"
#include "skybox.h"
#include "xms_scene.h"

class XmsSceneLocal: public XmsScene {
public:
    XmsSceneLocal();
    ~XmsSceneLocal();

    inline void set_input(Svr::SvrInput* input) { input_ = input; };
    virtual void InitGl(Svr::SvrApplicationContext* context) override;
    virtual void UpdateHMDPose(const svrHeadPoseState& headPoseState) override;
    virtual void Draw(int eye) override;
    virtual void Submit() override ;

    void HomePage();

private:
    void HandleInput();
    void OnCloseApp();

    std::shared_ptr<lark::SkyBox> sky_box_ = nullptr;
    std::shared_ptr<lark::Controller> controller_left_ = nullptr;
    std::shared_ptr<lark::Controller> controller_right_ = nullptr;
    std::shared_ptr<Navigation> navigation_ = nullptr;
    std::shared_ptr<TestObj> test_obj_ = nullptr;
    std::shared_ptr<Image> test_image_ = nullptr;

    Svr::SvrInput* input_ = nullptr;
};


#endif //CLOUDLARKXR_XMS_SCENE_LOCAL_H
