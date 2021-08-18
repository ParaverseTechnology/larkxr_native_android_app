// ++ LICENSE-HIDDEN HEADER ++
/** @file
    @brief Header

    @date 2016

    @author
    hTC, Inc.
*/

#ifndef _OPENVR_LOG
#define _OPENVR_LOG

#ifndef LOG_TAG
#define LOG_TAG "VRCORE_REMOVE"
#endif

#if defined(ANDROID)
#include <android/log.h>

#ifdef DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#else
#include <stdio.h>

#define LOGD(format,...) printf("D [%s] " format, LOG_TAG, ##__VA_ARGS__)
#define LOGI(format,...) printf("I [%s] " format, LOG_TAG, ##__VA_ARGS__)
#define LOGW(format,...) printf("W [%s] " format, LOG_TAG, ##__VA_ARGS__)
#define LOGE(format,...) printf("E [%s] " format, LOG_TAG, ##__VA_ARGS__)
#define LOGF(format,...) printf("F [%s] " format, LOG_TAG, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
#ifdef DEBUG
#ifdef __STDC__
#define LOGENTRY(...) vr::log::LogEntry local_log_entry(LOG_TAG, __func__)
#else  // __STDC__
// Need non stdc by -std=gnu++0x
#define LOGENTRY(...) vr::log::LogEntry local_log_entry(LOG_TAG, __func__, ##__VA_ARGS__)
#endif  // __STDC__
#else // DEBUG
#define LOGENTRY(...)
#endif // DEBUG
#define LOGLEAVE()

namespace vr {
namespace log {
class LogEntry {
private:
    const char * mLogTag;
    const char * mFuncName;
    LogEntry() {
    }
public:
    LogEntry(const char * logtag, const char * func) : mLogTag(logtag), mFuncName(func) {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, mLogTag, " +++%s", mFuncName);
#else
        printf(" +++%s\n", mFuncName);
#endif
    }
    LogEntry(const char * logtag, const char * func, const char * format, ...) : mLogTag(logtag), mFuncName(func) {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, mLogTag, " +++%s", mFuncName);
        if (format != nullptr) {
            va_list args;
            va_start (args, format);
            __android_log_vprint(ANDROID_LOG_DEBUG, mLogTag, format, args);
            va_end(args);
        }
#else
        printf(" +++%s\n", mFuncName);
#endif
    }
    ~LogEntry() {
#ifdef ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, mLogTag, " ---%s", mFuncName);
#else
        printf(" ---%s\n", mFuncName);
#endif
    }
};
}  // namespace log
}  // namespace vr

#else  // cplusplus
    #define LOGENTRY() LOGD(" +++" ## __func__)
    #define LOGLEAVE() LOGD(" ---" ## __func__)
#endif  // cplusplus

#endif  // _OPENVR_LOG
