#pragma once
 
#include "CoreMinimal.h"
#include "CameraStateKey.generated.h"

UENUM(BlueprintType, meta = (DisplayName = "ECameraStateKey"))
enum class ECameraStateKey : uint8
{
	DEFAULT		UMETA(DisplayName = "Default"),			// Basic over-the-shoulder camera (Rule of Thirds alignment)
	CINEMATIC	UMETA(DisplayName = "Cinematic Acion"),	// Scripted camera movement for events (Uses cubic splines)
	LOOKAT		UMETA(DisplayName = "LookAt"),			// Contextual focus on objects/areas (Temporarily overrides player input)
	COVER		UMETA(DisplayName = "Cover"),			// Zooms slightly, keeps reticule consistent (Lateral offset preserved)
	AIM			UMETA(DisplayName = "Aim"),				// Snaps to nearest valid shoulder (Adapts to in cover movement)
	SHIELD		UMETA(DisplayName = "Shield Aiming"),   // Snaps to nearer than normal aim so that shield occupies a larger portion of the screen
};
