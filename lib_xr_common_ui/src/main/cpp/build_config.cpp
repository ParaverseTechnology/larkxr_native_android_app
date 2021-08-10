//
// Created by fcx on 2020/9/16.
//

#include "build_config.h"

bool BuildConfig::is_inited_ = false;

bool BuildConfig::debug_ = false;

std::string BuildConfig::application_id_ = "";

std::string BuildConfig::build_type_ = "";

int BuildConfig::version_code_ = 1;

std::string BuildConfig::version_name_ = "";