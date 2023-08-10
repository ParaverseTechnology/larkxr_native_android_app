//
// Created by fcx@pingxingyung.com on 2020/10/29.
// Copyright (c) 2020 www.pingxingyun.com All rights reserved.
//

#ifndef CLOUDLARKXR_COVER_LOADER_H
#define CLOUDLARKXR_COVER_LOADER_H

#include <iostream>
#include "../common.h"

namespace lark {
class CoverLoaderImp;
class CoverLoader {
public:
    class LARK_COMMON_API CoverLoaderCallback {
    public:
        virtual void OnImageLoadSuccess(const char* data, int size) = 0;
        virtual void OnImageLoadFailed(const std::string& err) = 0;
    };
    CoverLoader(CoverLoaderCallback* callback);
    ~CoverLoader();

    void GetCoverAsync(const std::string& uri);

    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    CoverLoaderImp* imp_;
};
}



#endif //CLOUDLARKXR_COVER_LOADER_H
