//
// Created by Administrator on 2021/7/19.
//

#ifndef CLOUDLARKXR_XMS_SCENE_CLOUD_H
#define CLOUDLARKXR_XMS_SCENE_CLOUD_H

#include <skybox.h>
#include <ui/loading/loading.h>
#include <ui/controller.h>
#include <ui/menu_view.h>
#include "xms_scene.h"

class XmsSceneCloud: public XmsScene {
public:
    XmsSceneCloud();
    ~XmsSceneCloud();

    inline void set_input(Svr::SvrInput* input) { input_ = input; };
    virtual void InitGl(Svr::SvrApplicationContext* context) override;
    virtual void UpdateHMDPose(const svrHeadPoseState& headPoseState) override;
    virtual void Draw(int eye) override;
    virtual void Submit() override ;
    void Submit(larkxrTrackingFrame trackingFrame);

    void OnConnect();
    void OnMediaReady(int nativeTextrueLeft, int nativeTextureRight);
    void OnClose();
private:
    void OnCloseApp();

    std::shared_ptr<lark::SkyBox> sky_box_;
    std::shared_ptr<Loading> loading_;

    Svr::SvrInput* input_ = nullptr;
    //左右眼的紋理id
    int frame_texture_left_ = 0;
    int frame_texture_right_ = 0;
};


#endif //CLOUDLARKXR_XMS_SCENE_CLOUD_H
