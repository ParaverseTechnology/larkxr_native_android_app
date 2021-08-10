// 
// Created by fcx@pingxingyun.com
// 2020-03-10 16:05
//
#pragma once
#ifndef LOG_INCLUDE
#define LOG_INCLUDE

#include "pxygl.h"

#ifdef  __ANDROID__

#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "pxygl"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#define LogD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define LogE(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)

#else //  __ANDROID__

// TODO ADD LOG
#define LOGV(msg) (void)0
#define LOGD(msg) (void)0
#define LOGI(msg) (void)0
#define LOGW(msg) (void)0
#define LOGE(msg) (void)0
#define LOGF(msg) (void)0

#endif //

namespace lark {
    class CLOUDLARK_PXYGL_API Logger {
    public:
        static void InitLogger();
    private:
    };
}
#endif // LOG_INCLUDE