#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EPlatform : uint8
{
	PC = 0 UMETA(DisplayName = "Computer"),
	PS = 1 UMETA(DisplayName = "PlayStation"), 
	XBOX = 2 UMETA(DisplayName = "Xbox")
};
