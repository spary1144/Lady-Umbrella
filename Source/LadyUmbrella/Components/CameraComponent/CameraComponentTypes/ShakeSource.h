#pragma once
 
#include "CoreMinimal.h"
#include "ShakeSource.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "EShakeSource"))
enum class EShakeSource : uint8
{
	IDLE			UMETA(DisplayName = "Player Idle"),
	SWAY			UMETA(DisplayName = "Movement Sway"),
	RECOIL			UMETA(DisplayName = "Recoil"),
	GADGET_RECOIL	UMETA(DisplayName = "Gadget Recoil"),
	ENVIRONMENT		UMETA(DisplayName = "Environment"), // Falling Element and Explosions
	DIAGONAL		UMETA(DisplayName = "Directed diagonal"), // Melee and Cover slide or enter
	IMPACT			UMETA(DisplayName = "Impacts"), // Shield impacts, Vault Kick impact, Hook impact
	FREEFALL		UMETA(DisplayName = "Player Freefall"),
	SWING			UMETA(DisplayName = "Rope or Bar swing"),
	VIBRATION		UMETA(DisplayName = "Vibrating Sway"), // Zipline (Impact on loop)
	UNKNOWN			UMETA(DisplayName = "Unknown")
};