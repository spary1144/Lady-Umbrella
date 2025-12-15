// 
// BTT_CheckIfCanFlee.h
// 
// BehaviourTree: Task to know if agent can flee
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CheckIfCanFlee.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_CheckIfCanFlee : public UBTTaskNode
{
	GENERATED_BODY()

	UBTT_CheckIfCanFlee();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
