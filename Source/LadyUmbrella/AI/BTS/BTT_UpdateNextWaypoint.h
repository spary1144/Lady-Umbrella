// 
// BTT_UpdateNextWaypoint.h
// 
// BehaviourTree: Task responsible for updating the current waypoint index
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_UpdateNextWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_UpdateNextWaypoint : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
