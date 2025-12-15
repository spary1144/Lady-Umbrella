#pragma once
 
#include "CoreMinimal.h"
#include "CurrentHitEnvironment.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "ECurrentHitEnvironment"))
enum class ECurrentHitEnvironment : uint8
{
	WALL			UMETA(DisplayName = "Wall"),
	CORNER			UMETA(DisplayName = "Corner"),
	HALLWAY			UMETA(DisplayName = "Hallway"),
	HWY_CORNER		UMETA(DisplayName = "Hallway Corner"),
	NONE			UMETA(DisplayName = "No Hit")
};
