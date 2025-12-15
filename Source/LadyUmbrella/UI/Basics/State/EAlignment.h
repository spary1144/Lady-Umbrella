#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EAlignment : uint8
{
	Left = 0 UMETA(DisplayName = "Left-Column"),
	Center = 1 UMETA(DisplayName = "Center-Column"), 
	Right = 2 UMETA(DisplayName = "Right-Column")
};
