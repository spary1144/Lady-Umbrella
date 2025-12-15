// 
// VoiceLineInGameDTO.h
// 
// Holds all info required (alongside a VoiceLineDTO) for an in game cinematic. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "VoiceLineDTO.h"
#include "VoiceLineInGameDTO.generated.h"

class AAgencyCharacter;
class AMarcoCharacter;
class APlayerCharacter;
class AVincenzoCharacter;

USTRUCT(BlueprintType)
struct FVoiceLineInGameDTO
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 StateIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 MarcoIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AMarcoCharacter> MarcoCharacter;
	
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerIndex = 0;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY(BlueprintReadWrite)
	int32 AgencyIndex = 0;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AAgencyCharacter> AgencyCharacter;

	UPROPERTY(BlueprintReadWrite)
	int32 VincenzoIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AVincenzoCharacter> VincenzoCharacter;

	UPROPERTY(BlueprintReadWrite)
	int32 OrderArrayIndex = 0;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<EVoiceLineActor> OrderArray;
	
	UPROPERTY(BlueprintReadWrite)
	FVoiceLineDTO VoiceLineDTO;
};
