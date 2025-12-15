#pragma once


#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FCheckpointDataDriven.generated.h"


USTRUCT(BlueprintType)
struct FCheckpointDataDriven : public FTableRowBase
{
	GENERATED_BODY()
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ButtonId = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Level = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName LevelName = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Identifier = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EncounterName = "";
	
};
