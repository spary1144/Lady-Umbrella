// 
// BTT_SetCoverAsOcupied.h
// 
// BehaviourTree: Task responsible of mark cover as ocuped
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetCoverAsOcupied.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_SetCoverAsOcupied : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere)
	bool ocupied;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector cover;
};
