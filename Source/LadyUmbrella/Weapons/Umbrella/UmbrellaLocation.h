#pragma once
#include "CoreMinimal.h"
#include "UmbrellaLocation.generated.h"

UENUM(BlueprintType)
enum class EUmbrellaLocation : uint8
{
	Back			UMETA(DisplayName="Back"),
	Hand			UMETA(DisplayName="Hand"),
	UmbrellaCenter	UMETA(DisplayName="UmbrellaCenter"),
	LeftHand		UMETA(DisplayName="LeftHand"),
	WorkBench		UMETA(DisplayName="WorkBench"),
};