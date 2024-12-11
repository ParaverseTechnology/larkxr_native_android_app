// 
// Created by fcx@pingxingyun.com
// 2024-12-10 17:37
//
#pragma once
#ifndef HTTP_CLIENT_INCLUDE
#define HTTP_CLIENT_INCLUDE

#include <functional>

namespace lark
{
// HttpClient 类，用于处理 HTTP 请求
class HttpClient {
public:
    // OnResponse，用于处理响应
    // int code：表示响应的状态码
    // const char* status_message：表示响应的状态消息
    // const char* body：表示响应的主体内容
    typedef std::function<void(int code, const char* status_message, const char* body)> OnResponse;

    HttpClient();
    ~HttpClient();

    // 发送 GET 请求的同步方法
    // @param url 目标 URL 地址
    // @param on_response 响应处理函数
    int Get(const char* url, OnResponse on_response);

    // 发送 GET 请求的异步方法
    // @param url 目标 URL 地址
    // @param on_response 响应处理函数
    int GetAsync(const char* url, OnResponse on_response);

    // 发送 POST 请求的同步方法
    // @param url 目标 URL 地址
    // @param body 请求体内容
    // @param content_type 请求体的内容类型
    // @param on_response 响应处理函数
    int Post(const char* url, const char* body, const char* content_type, OnResponse on_response);

    // 发送 POST 请求的异步方法
    // @param url 目标 URL 地址
    // @param body 请求体内容
    // @param content_type 请求体的内容类型
    // @param on_response 响应处理函数
    int PostAsync(const char* url, const char* body, const char* content_type, OnResponse on_response);
private:
    class Impl;
    Impl* impl_;
};

} // namespace lark

#endif // HTTP_CLIENT_INCLUDE