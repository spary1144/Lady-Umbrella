#pragma once
 
#include "CoreMinimal.h"
#include "CameraAttachment.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "ECameraAttachment"))
enum class ECameraAttachment : uint8
{
	CAPSULE_TOP		UMETA(DisplayName = "Capsule Top"),	
	MESH_TOP		UMETA(DisplayName = "Player Mesh Top"),	
	WEAPON_MUZZLE	UMETA(DisplayName = "Umbrella Muzzle")
};