#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EScreenPosition : uint8
{
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP,
	BOTTOM,
	RIGHT,
	LEFT,
	CENTER
};
