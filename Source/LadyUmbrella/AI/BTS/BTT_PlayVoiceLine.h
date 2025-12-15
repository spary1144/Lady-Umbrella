// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayVoiceLine.generated.h"

enum class EVoiceLineActor : uint8;
enum class EVoiceLineState : uint8;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_PlayVoiceLine : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EVoiceLineState VoiceLineState;

	UPROPERTY(EditAnywhere)
	EVoiceLineActor VoiceLineActor;

	UPROPERTY(EditAnywhere)
	bool IsPrioritary;

	UBTT_PlayVoiceLine();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
