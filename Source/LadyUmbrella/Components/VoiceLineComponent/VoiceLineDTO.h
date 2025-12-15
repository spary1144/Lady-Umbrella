// 
// VoiceLineState.h
// 
// Defines a complete Voice Line Request to storage in Priority Queue 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "VoiceLineState.h"
#include "VoiceLineActor.h"
#include "VoiceLineDTO.generated.h"

enum class EVoiceLineActor : uint8;
class AGenericCharacter;

USTRUCT(BlueprintType)
struct FVoiceLineDTO
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	EVoiceLineState State = EVoiceLineState::OTHER;

	UPROPERTY(BlueprintReadWrite)
	EVoiceLineActor Actor = EVoiceLineActor::AGENCY;
	
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AGenericCharacter> RequestingActor;

	bool PriorityAboveAll = false; //true when the action that trigger this voice lines is prioritary

	uint8_t IndexState = MAX_uint8;
};