#pragma once
 
#include "CoreMinimal.h"
#include "CameraInterpolation.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "ECameraInterpolation"))
enum class ECameraInterpolation : uint8
{
	CUSTOM		= 0		UMETA(DisplayName = "Custom"),		
	EASE		= 1		UMETA(DisplayName = "Basic Ease"),
	EASE_I		= 2		UMETA(DisplayName = "Ease-In"),
	EASE_O		= 3		UMETA(DisplayName = "Ease-Out"),
	EASE_IO		= 4		UMETA(DisplayName = "Ease-In-Out"),
	SPRING_I	= 5		UMETA(DisplayName = "Elastic Ease-In"),
	SPRING_O	= 6		UMETA(DisplayName = "Elastic Ease-Out"),
	SPRING_IO	= 7		UMETA(DisplayName = "Elastic Ease-In-Out"),
	LINEAR		= 8		UMETA(DisplayName = "Linear")
};

