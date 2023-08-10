//
// Created by fcx on 2020/2/26.
//

#ifndef CLOUDLARKXR_CLOULARK_XR_H
#define CLOUDLARKXR_CLOULARK_XR_H

#if defined(_WIN32) && defined(LARK_XR_DLL)
#if defined(LARK_XR_EXPORT)
		#define LARK_XR_API __declspec(dllexport)
	#else
		#define LARK_XR_API __declspec(dllimport)
	#endif
#endif

#if !defined(LARK_XR_API)
#if defined (__GNUC__) && (__GNUC__ >= 4)
#define LARK_XR_API __attribute__ ((visibility ("default")))
#else
#define LARK_XR_API
#endif
#endif

#define DC_CALL __stdcall

#endif //CLOUDLARKXR_CLOULARK_XR_H
