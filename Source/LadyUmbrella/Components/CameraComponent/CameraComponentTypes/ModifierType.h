#pragma once
 
#include "CoreMinimal.h"
#include "ModifierType.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "EModifierType"))
enum class EModifierType : uint8
{
	DEFAULT			UMETA(DisplayName = "Default Sensibility"), // Multiplier applied to Master Sensibility
	AIM				UMETA(DisplayName = "Aim Sensibility"),		// Multiplier applied to Master Sensibility
	YAW				UMETA(DisplayName = "Horizontal Inversion"),
	PITCH			UMETA(DisplayName = "Vertical Inversion"),
};