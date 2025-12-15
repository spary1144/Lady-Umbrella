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
#include "BTT_CheckEnemiesInZone.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_CheckEnemiesInZone : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_CheckEnemiesInZone();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
