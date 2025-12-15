// 
// BTT_AddToFlankingPath.h
// 
// BehaviourTree: Task responsible of add flank cover to path
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AddToFlankingPath.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_AddToFlankingPath : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector CoverToFlank;
};
