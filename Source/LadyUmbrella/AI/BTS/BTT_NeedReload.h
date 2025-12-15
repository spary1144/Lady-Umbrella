// 
// BTT_NeedReload.h
// 
// BehaviourTree: Task to know if agent need to reload
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_NeedReload.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_NeedReload : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
