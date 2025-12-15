// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CanAdvanveNextZone.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_CanAdvanveNextZone : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AdvanceRadiusKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentZoneKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
