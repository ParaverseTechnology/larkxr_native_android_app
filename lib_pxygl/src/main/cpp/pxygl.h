// 
// Created by fcx@pingxingyun.com
// 2020-03-10 10:29
//
#pragma once
#ifndef PXYGL_INCLUDE
#define PXYGL_INCLUDE

#if defined(_WIN32) && defined(LARK_PXYGL_DLL)
#if defined(LARK_PXYGL_EXPORT)
#define CLOUDLARK_PXYGL_API __declspec(dllexport)
#else
#define CLOUDLARK_PXYGL_API __declspec(dllimport)
#endif
#endif

#if !defined(CLOUDLARK_PXYGL_API)
#if !defined(POCO_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
#define CLOUDLARK_PXYGL_API __attribute__ ((visibility ("default")))
#else
#define CLOUDLARK_PXYGL_API
#endif
#endif

//
// Automatically link Common library.
//
#if defined(_MSC_VER)
#define LARK_LIB_SUFFIX ".lib"

#if !defined(LARK_PXYGL_EXPORT)
#pragma comment(lib, "lark_pxygl" LARK_LIB_SUFFIX)
#endif
#endif

#ifdef __ANDROID__
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#else
#include <GL\glew.h>
#endif // __ANDROID__

namespace lark {
class CLOUDLARK_PXYGL_API PxyGl {
public:
	void Test();
	void Init();

#ifdef  __ANDROID__
	bool InitGl();
#endif
};
}
#endif // PXYGL_INCLUDE