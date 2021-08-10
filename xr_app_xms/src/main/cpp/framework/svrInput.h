//=============================================================================
// FILE: svrInput.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#pragma once

#include <log.h>
#include "android/keycodes.h"
#include "svrUtil.h"

#define INPUT_MAX_POINTERS	5

namespace Svr
{
	enum SvrKey
	{
		kSvrKey_UNKNOWN = -1,
		kSvrKey_0 = AKEYCODE_0,
		kSvrKey_1 = AKEYCODE_1,
		kSvrKey_2 = AKEYCODE_2,
		kSvrKey_3 = AKEYCODE_3,
		kSvrKey_4 = AKEYCODE_4,
		kSvrKey_5 = AKEYCODE_5,
		kSvrKey_6 = AKEYCODE_6,
		kSvrKey_7 = AKEYCODE_7,
		kSvrKey_8 = AKEYCODE_8,
		kSvrKey_9 = AKEYCODE_9,
		kSvrKey_A = AKEYCODE_A,
		kSvrKey_B = AKEYCODE_B,
		kSvrKey_C = AKEYCODE_C,
		kSvrKey_D = AKEYCODE_D,
		kSvrKey_E = AKEYCODE_E,
		kSvrKey_F = AKEYCODE_F,
		kSvrKey_G = AKEYCODE_G,
		kSvrKey_H = AKEYCODE_H,
		kSvrKey_I = AKEYCODE_I,
		kSvrKey_J = AKEYCODE_J,
		kSvrKey_K = AKEYCODE_K,
		kSvrKey_L = AKEYCODE_L,
		kSvrKey_M = AKEYCODE_M,
		kSvrKey_N = AKEYCODE_N,
		kSvrKey_O = AKEYCODE_O,
		kSvrKey_P = AKEYCODE_P,
		kSvrKey_Q = AKEYCODE_Q,
		kSvrKey_R = AKEYCODE_R,
		kSvrKey_S = AKEYCODE_S,
		kSvrKey_T = AKEYCODE_T,
		kSvrKey_U = AKEYCODE_U,
		kSvrKey_V = AKEYCODE_V,
		kSvrKey_W = AKEYCODE_W,
		kSvrKey_X = AKEYCODE_X,
		kSvrKey_Y = AKEYCODE_Y,
		kSvrKey_Z = AKEYCODE_Z,
		kSvrKey_SPACE = AKEYCODE_HOME,
		kSvrKey_ESC = AKEYCODE_BACK,
		kSvrKey_LEFT = AKEYCODE_DPAD_LEFT,
		kSvrKey_RIGHT = AKEYCODE_DPAD_RIGHT,
		kSvrKey_UP = AKEYCODE_DPAD_UP,
		kSvrKey_DOWN = AKEYCODE_DPAD_DOWN,
		kSvrKey_Volume_Down = AKEYCODE_VOLUME_DOWN,
		kSvrKey_Volume_Up = AKEYCODE_VOLUME_UP,
		kSvrKey_OK = AKEYCODE_ENTER,
		kSvrKey_LAST = 254
	};

	struct SvrPointerStatus
	{
		SvrPointerStatus()
			: down(false)
			, x(0.f)
			, y(0.f)
			, deltaX(0.f)
			, deltaY(0.f)
	        , deltaWheel(0)
		{};

		bool	down;
		float	x;
		float	y;
		float	deltaX;
		float	deltaY;
		int		deltaWheel;
	};

	struct SvrKeyStatus {
        SvrKeyStatus(): down(false), repeat(0) {
        };
	    bool down;
	    int repeat;
	    bool longPressTrigged;
	};

	class SvrInput
	{
	public:
		SvrInput();

		void	PointerDownEvent(int pointerId, float x, float y);
		void	PointerUpEvent(int pointerId, float x, float y);
		void	PointerMoveEvent(int pointerId, float x, float y);
		void	ScrollWheelEvent(int pointerId, short s);

		void	KeyDownEvent(int keyId, int repeat);
		void	KeyUpEvent(int keyId, int repeat);

		bool	IsKeyDown(SvrKey keyId);
		bool	IsKeyUp(SvrKey keyId);
		bool	IsKeyDownThisFrame(SvrKey keyId);
		bool	IsKeyUpThisFrame(SvrKey keyId, bool* isLongPressed);
		bool    IsLongPress(SvrKey keyId);

		bool	IsPointerDown(int pointerId);
		bool	GetPointerLocation(int pointerId, float& outX, float& outY);
	    bool  	GetMouseWheel(int pointerId, short& outWheel);
		bool	GetPointerDelta(int pointerId, float& outDeltaX, float& outDeltaY);

		void	Update();

	private:
		void InitializeKeyMap();

	private:
		unsigned char		mCounter;
        SvrKeyStatus		mKeyStatus[256] = {};
        SvrKeyStatus		mPrevKeyStatus[256] = {};
		SvrPointerStatus	mPointerStatus[INPUT_MAX_POINTERS];

	};

//-----------------------------------------------------------------------------
inline bool SvrInput::IsKeyDown(SvrKey keyId)
//-----------------------------------------------------------------------------
{
    return mKeyStatus[keyId].down;
}

//-----------------------------------------------------------------------------
inline bool SvrInput::IsKeyDownThisFrame(SvrKey keyId)
//-----------------------------------------------------------------------------
{
    if((mKeyStatus[keyId].down) && !(mPrevKeyStatus[keyId].down))
        return 1;
    return 0;
}

//-----------------------------------------------------------------------------
inline bool SvrInput::IsKeyUpThisFrame(SvrKey keyId, bool* isLongPressed)
//-----------------------------------------------------------------------------
{
//    LOGI("IsKeyUpThisFrame id %d; status %d; prestatus %d", keyId, mKeyStatus[keyId], mPrevKeyStatus[keyId]);

    if(!(mKeyStatus[keyId].down) && (mPrevKeyStatus[keyId].down)) {
        *isLongPressed = mPrevKeyStatus[keyId].repeat > 40;
        return 1;
    }
    *isLongPressed = false;
    return 0;
}


inline bool SvrInput::IsLongPress(SvrKey keyId) {
    if (mKeyStatus[keyId].down && mKeyStatus[keyId].repeat > 40 && !mKeyStatus[keyId].longPressTrigged) {
        mKeyStatus[keyId].longPressTrigged = true;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
inline bool SvrInput::IsPointerDown(int pointerId)
//-----------------------------------------------------------------------------
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
        return false;

	return mPointerStatus[pointerId].down;
}

//-----------------------------------------------------------------------------
inline bool SvrInput::GetPointerLocation(int pointerId, float& outX, float& outY)
//-----------------------------------------------------------------------------
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
	    outX = 0.0f;
	    outY = 0.0f;
        return false;
    }

	outX = mPointerStatus[pointerId].x;
	outY = mPointerStatus[pointerId].y;
	return IsPointerDown(pointerId);
}

//-----------------------------------------------------------------------------
inline bool SvrInput::GetMouseWheel(int pointerId, short& outWheel)
//-----------------------------------------------------------------------------
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
	    outWheel = 0;
        return false;
    }

	outWheel = mPointerStatus[pointerId].deltaWheel;
    mPointerStatus[pointerId].deltaWheel = 0;
	return outWheel != 0;
}

//-----------------------------------------------------------------------------
inline bool  SvrInput::GetPointerDelta(int pointerId, float& outDeltaX, float& outDeltaY)
//-----------------------------------------------------------------------------
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
	    outDeltaX = 0.0f;
	    outDeltaY = 0.0f;
        return false;
    }

	outDeltaX = mPointerStatus[pointerId].deltaX;
	outDeltaY = mPointerStatus[pointerId].deltaY;
	return IsPointerDown(pointerId);
}

}//End namespace Scorch
