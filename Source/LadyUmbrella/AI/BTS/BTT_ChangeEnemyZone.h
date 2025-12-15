// 
// BTT_ChangeEnemyZone.h
// 
// BehaviourTree: Task to know how many free covers are inside one zone
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangeEnemyZone.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_ChangeEnemyZone : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CurrentZone;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
