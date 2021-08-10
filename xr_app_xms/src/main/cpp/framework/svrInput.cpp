//=============================================================================
// FILE: svrInput.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#include <stdio.h>
#include <string.h>

#include "svrInput.h"
#include "svrUtil.h"

namespace Svr
{

SvrInput::SvrInput()
	: mCounter(1)
{
	InitializeKeyMap();
    memcpy(mPrevKeyStatus, mKeyStatus, 256 * sizeof(SvrKeyStatus));
}

void SvrInput::Update()
{
    memcpy(mPrevKeyStatus, mKeyStatus, 256 * sizeof(SvrKeyStatus));
    mCounter++;
}
void SvrInput::ScrollWheelEvent(int pointerId, short s)
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
        LOGE("Invalid pointerId: %d", pointerId);
        return;
    }

    static int sCount = 0;
        sCount+=s;
	mPointerStatus[pointerId].deltaWheel = s;
}

void SvrInput::PointerDownEvent(int pointerId, float x, float y)
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
        LOGE("Invalid pointerId: %d", pointerId);
        return;
    }

	mPointerStatus[pointerId].down = true;
	mPointerStatus[pointerId].x = x;
	mPointerStatus[pointerId].y = y;
	mPointerStatus[pointerId].deltaX = 0.f;
	mPointerStatus[pointerId].deltaY = 0.f;
}

void SvrInput::PointerUpEvent(int pointerId, float x, float y)
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
        LOGE("Invalid pointerId: %d", pointerId);
        return;
    }

	mPointerStatus[pointerId].down = false;
	mPointerStatus[pointerId].deltaX = x - mPointerStatus[pointerId].x;
	mPointerStatus[pointerId].deltaY = y - mPointerStatus[pointerId].y;
	mPointerStatus[pointerId].x = x;
	mPointerStatus[pointerId].y = y;
}

void SvrInput::PointerMoveEvent(int pointerId, float x, float y)
{
    if(pointerId < 0 || pointerId >= INPUT_MAX_POINTERS)
    {
        LOGE("Invalid pointerId: %d", pointerId);
        return;
    }

	mPointerStatus[pointerId].deltaX = x - mPointerStatus[pointerId].x;
	mPointerStatus[pointerId].deltaY = y - mPointerStatus[pointerId].y;
	mPointerStatus[pointerId].x = x;
	mPointerStatus[pointerId].y = y;
}

void SvrInput::KeyDownEvent(int keyId, int repeat)
{
    mKeyStatus[keyId].down = true;
    mKeyStatus[keyId].repeat = repeat;
//    LOGE("KeyDownEvent: %d %d", keyId, mKeyStatus[keyId]);
}

void SvrInput::KeyUpEvent(int keyId, int repeat)
{
    mKeyStatus[keyId].down = false;
    mKeyStatus[keyId].repeat = repeat;
    mKeyStatus[keyId].longPressTrigged = false;
//    LOGE("KeyUpEvent: %d %d", keyId, mKeyStatus[keyId]);
}

void SvrInput::InitializeKeyMap()
{

}

}//End namespace
