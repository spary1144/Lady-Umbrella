// 
// EState.h
// 
// States for buttons. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

UENUM(Blueprintable)
enum class EUIState : uint8
{
	Idle,
	Hovered,
	Pressed,
	Disabled
};
