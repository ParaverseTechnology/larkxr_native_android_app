//
// Created by fcx on 2020/9/16.
//

#ifndef CLOUDLARKXR_BUILD_CONFIG_H
#define CLOUDLARKXR_BUILD_CONFIG_H

#include <jni.h>
#include "iostream";

class BuildConfig {
public:
    inline static void init(bool debug, const std::string& applicationId, const std::string& buildType, int versionCode, const std::string& versionName) {
        is_inited_ = true;
        debug_ = debug;
        application_id_ = applicationId;
        build_type_ = buildType;
        version_code_ = versionCode;
        version_name_ = versionName;
    };

    inline static void init(JNIEnv *env, const char* javaBuildConfigClassName) {
        // init build config.
        jclass jclazz = env->FindClass(javaBuildConfigClassName);
        jfieldID f_debug = env->GetStaticFieldID(jclazz, "DEBUG", "Z");
        jfieldID f_appId =  env->GetStaticFieldID(jclazz, "APPLICATION_ID", "Ljava/lang/String;");
        jfieldID f_buildType = env->GetStaticFieldID(jclazz, "BUILD_TYPE", "Ljava/lang/String;");
        jfieldID f_versionCode =  env->GetStaticFieldID(jclazz, "VERSION_CODE", "I");
        jfieldID f_versionName = env->GetStaticFieldID(jclazz, "VERSION_NAME", "Ljava/lang/String;");
        jboolean j_debug = env->GetStaticBooleanField(jclazz, f_debug);
        jstring j_appId = (jstring)env->GetStaticObjectField(jclazz, f_appId);
        jstring j_buildType = (jstring)env->GetStaticObjectField(jclazz, f_buildType);
        jint j_versionCode = env->GetStaticIntField(jclazz, f_versionCode);
        jstring j_versionName = (jstring)env->GetStaticObjectField(jclazz, f_versionName);

        const char *appid = env->GetStringUTFChars(j_appId, 0);
        const char *buildType = env->GetStringUTFChars(j_buildType, 0);
        const char *versionName = env->GetStringUTFChars(j_versionName, 0);
        BuildConfig::init(j_debug, appid, buildType, j_versionCode, versionName);

        env->ReleaseStringUTFChars(j_appId, appid);
        env->ReleaseStringUTFChars(j_buildType, buildType);
        env->ReleaseStringUTFChars(j_versionName, versionName);
        env->DeleteLocalRef(jclazz);
    }

    inline static bool is_inited() { return is_inited_; }
    inline static bool debug() { return debug_; }
    inline static const std::string& application_id() { return application_id_; }
    inline static const std::string& build_type() { return build_type_; }
    inline static int version_code() { return version_code_; }
    inline static const std::string& version_name() { return version_name_; }
private:
    static bool is_inited_;
    static bool debug_;
    static std::string application_id_;
    static std::string build_type_;
    static int version_code_;
    static std::string version_name_;
};

#endif //CLOUDLARKXR_BUILD_CONFIG_H
