#pragma once
 
#include "CoreMinimal.h"
#include "CameraInputSpeed.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "ECameraInputSpeed"))
enum class ECameraInputSpeed : uint8
{
	PEAKED,			
	ACCELERATING,
	DECELERATING,
	STATIC
};