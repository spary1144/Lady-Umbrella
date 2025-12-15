// 
// BTT_SetBoolValue.h
// 
// BehaviourTree: Task responsible of set bool blackboard value
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetBoolValue.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_SetBoolValue : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BoolValue;

	UPROPERTY(EditAnywhere)
	bool value;
};
