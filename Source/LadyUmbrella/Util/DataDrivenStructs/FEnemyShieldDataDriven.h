#pragma once

#include "CoreMinimal.h"
#include "FEnemyShieldDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FEnemyShieldDataDriven : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Endurance = 0.f;
};
