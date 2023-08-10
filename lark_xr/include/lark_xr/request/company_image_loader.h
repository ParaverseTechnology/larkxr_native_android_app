//
// Created by fcx@pingxingyung.com on 2020/10/29.
// Copyright (c) 2020 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_COMPANY_IMAGE_LOADER_H
#define CLOUDLARKXR_COMPANY_IMAGE_LOADER_H

#include <iostream>
#include "../common.h"

namespace lark {
class CompanyImageLoaderImp;
class LARK_COMMON_API CompanyImageLoader {
public:
    enum CompanyImageType {
        CompanyImageType_Header = 0,
        CompanyImageType_Footer,
        CompanyImageType_Logo,
    };
    class CompanyImageLoaderCallback {
    public:
        virtual void OnImageLoadSuccess(CompanyImageType type, const char* data, int size) = 0;
        virtual void OnImageLoadFailed(const std::string& err) = 0;
    };
    CompanyImageLoader(CompanyImageType type, CompanyImageLoaderCallback* callback);
    ~CompanyImageLoader();

    void SendAsync();
    CompanyImageLoader::CompanyImageType type();
    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    CompanyImageLoaderImp* imp_;
};
}

#endif //CLOUDLARKXR_COMPANY_IMAGE_LOADER_H
