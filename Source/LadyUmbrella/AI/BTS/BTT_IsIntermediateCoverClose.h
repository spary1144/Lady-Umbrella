// 
// BTT_IsIntermediateCoverClose.h
// 
// BehaviourTree: Task responsible of check if an intermediate cover is close to Agent
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_IsIntermediateCoverClose.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_IsIntermediateCoverClose : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	float RadiusSq;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector Cover;
};
