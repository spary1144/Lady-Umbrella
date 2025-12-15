#pragma once

#include "CoreMinimal.h"
#include "FChaptersProgress.generated.h"

USTRUCT(BlueprintType)
struct FChaptersProgress
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	bool bIsCompleted = false;
	
	UPROPERTY(SaveGame)
	bool bLastChapterUnlocked = false;

	UPROPERTY(SaveGame)
	bool bLastChapterPlayed = false;

	UPROPERTY(SaveGame)
	bool bChapterUnlocked = false;

};
