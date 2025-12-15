#pragma once


#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlayerStateEnum.h"
#include "PlayerStateGateStruct.generated.h"


USTRUCT(Blueprintable, BlueprintType)
struct LADYUMBRELLA_API FPlayerStateGateStruct
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	TMap<EPlayerState, bool> AllowedTransitionsMap;
};