// 
// BTT_AddCoverToSelected.h
// 
// BehaviourTree: Task responsible of set cover as selected( used only by mafia)
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AddCoverToSelected.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_AddCoverToSelected : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
