#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ENavigationInput : uint8
{
	NAVIGATE_UP,
	NAVIGATE_DOWN,
	SWITCH_LEFT_TAB,
	SWITCH_RIGHT_TAB,
	INTERACT,
	INTERACT_RIGHT,
	INTERACT_LEFT,
	EXIT,
	NONE
};
