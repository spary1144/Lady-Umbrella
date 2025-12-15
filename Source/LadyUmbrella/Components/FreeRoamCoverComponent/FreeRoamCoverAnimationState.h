#pragma once

#include "CoreMinimal.h"
#include "FreeRoamCoverAnimationState.generated.h"

/**
* Used in FreeRoamCoverComponent to determine the animation state of a character
 */
UENUM(BlueprintType)
enum class ECoverHeightState : uint8
{
	Standing,
	Crouching,
	NoCover
};