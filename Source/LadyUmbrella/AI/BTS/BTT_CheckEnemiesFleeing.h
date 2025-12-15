// 
// BTT_CheckIfCanFlee.h
// 
// BehaviourTree: Task to know how many enemies are inside one zone
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CheckEnemiesFleeing.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_CheckEnemiesFleeing : public UBTTaskNode
{
	GENERATED_BODY()

	
	
	UPROPERTY(EditAnywhere)
	int MaxEnemiesFleeing = 1;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
