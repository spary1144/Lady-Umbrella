#pragma once

#include "CoreMinimal.h"
#include "ShapeToVis.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "EShapeToVis"))
enum class EShapeToVis : uint8
{
	LINE			UMETA(DisplayName = "Line"),
	BOX				UMETA(DisplayName = "Box"),
	SPHERE			UMETA(DisplayName = "Sphere"),
};
