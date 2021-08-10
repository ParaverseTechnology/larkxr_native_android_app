//
// Created by fcx@pingixngyun.com on 2019/11/19.
//

#ifndef CLOUDLARK_OCULUS_DEMO_LOADING_H
#define CLOUDLARK_OCULUS_DEMO_LOADING_H

#include "ui/view.h"
#include "lark_xr/request/company_image_loader.h"

class Loading: public View, public lark::CompanyImageLoader::CompanyImageLoaderCallback, public Image::ImageChangeCallback {
public:
    Loading(Navigation *navigation);
    ~Loading();

    void SetQuitTips(const std::wstring& tips);
    // switch to current page.
    virtual void Enter() override;
    // load lopo
    virtual void OnImageLoadSuccess(lark::CompanyImageLoader::CompanyImageType type, const char* data, int size) override;
    virtual void OnImageLoadFailed(const std::string& err) override;
    // image callback
    virtual void OnImageInited(Image* img) override;
protected:
    virtual void Init() override;
private:
    std::shared_ptr<Text> text_;
    std::shared_ptr<Text> tips_;
    std::shared_ptr<Image> icon_;
    std::shared_ptr<Image> footer_;
    lark::CompanyImageLoader center_loader_;
    lark::CompanyImageLoader footer_loader_;
    bool first_load_ = true;
    bool load_success_ = false;
};


#endif //CLOUDLARK_OCULUS_DEMO_LOADING_H
