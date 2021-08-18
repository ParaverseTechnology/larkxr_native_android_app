// 
// Created by fcx@pingxingyun.com
// 2020-03-10 10:29
//
#include "pxygl.h"
#include <iostream>
#include "logger.h"


namespace lark {
void PxyGl::Test() {
}
void PxyGl::Init() {
}

#ifdef __ANDROID__
bool PxyGl::InitGl() {
	return false;
}
#endif
}