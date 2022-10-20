// 
// Created by fcx@pingxingyun.com
// 2020-01-09 18:23
//
#pragma once
#ifndef COMMON_INCLUDE
#define COMMON_INCLUDE

#if defined(_WIN32) && defined(LARK_COMMON_DLL)
	#if defined(LARK_COMMON_EXPORT)
		#define LARK_COMMON_API __declspec(dllexport)
	#else
		#define LARK_COMMON_API __declspec(dllimport)
	#endif
#endif

#if !defined(LARK_COMMON_API)
	#if defined (__GNUC__) && (__GNUC__ >= 4)
		#define LARK_COMMON_API __attribute__ ((visibility ("default")))
	#else
		#define LARK_COMMON_API
	#endif
#endif

//
// Automatically link Common library.
//
#if defined(_MSC_VER)
	#define LARK_LIB_SUFFIX ".lib"
	
	#if !defined(CLOUDLARK_COMMON_EXPORT)
		#pragma comment(lib, "lark_common" LARK_LIB_SUFFIX)
	#endif
#endif
#endif // COMMON_INCLUDE

// Define to desired default thread stack size
// Zero means OS default
#ifndef POCO_THREAD_STACK_SIZE
	#define POCO_THREAD_STACK_SIZE 0
#endif