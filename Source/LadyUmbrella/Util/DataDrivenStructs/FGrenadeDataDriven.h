#pragma once

#include "CoreMinimal.h"
#include "FGrenadeDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FGrenadeDataDriven : public FTableRowBase 
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDmg = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDmg = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SphereDmgRadius = 0.f;

};
