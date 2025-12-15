#pragma once

#include "CoreMinimal.h"
#include "FPickableSaveData.generated.h"

USTRUCT(BlueprintType)
struct FPickableSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	bool bHasToSpawn = false;
	
	UPROPERTY(SaveGame)
	FName ChapterName = FName();
	
};
