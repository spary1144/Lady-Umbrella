// 
// BTT_NextFlankCover.h
// 
// BehaviourTree: Task responsible of select next flank cover
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_NextFlankCover.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_NextFlankCover : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector NextLocation;
};
