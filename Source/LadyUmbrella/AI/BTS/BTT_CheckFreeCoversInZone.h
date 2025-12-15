// 
// BTT_CheckIfCanFlee.h
// 
// BehaviourTree: Task to know how many free covers are inside one zone
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CheckFreeCoversInZone.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_CheckFreeCoversInZone : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_CheckFreeCoversInZone();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentZone;

	//Pasar a blackboardKeySelector
	FName const NextZone = "NextZone";
	FName const FleeingNextZone = "FleeingNextZone";
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
};
