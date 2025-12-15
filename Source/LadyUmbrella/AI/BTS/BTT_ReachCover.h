// 
// BTT_ReachCover.h
// 
// BehaviourTree: Task that notify player that has reached one cover
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ReachCover.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_ReachCover : public UBTTaskNode
{
	GENERATED_BODY()
	UBTT_ReachCover();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
