#pragma once


#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FVoiceLineEventNames.generated.h"


USTRUCT(BlueprintType)
struct FVoiceLineEventNames : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString StateName = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString PathToBank = "";
	
};
