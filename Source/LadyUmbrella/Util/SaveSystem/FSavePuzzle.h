#pragma once

#include "CoreMinimal.h"
#include "FSavePuzzle.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="FSavePuzzle"))
struct FSavePuzzle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform WorldTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSolved = false;
};
