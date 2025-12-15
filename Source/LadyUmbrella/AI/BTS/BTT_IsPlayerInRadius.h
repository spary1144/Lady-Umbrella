// 
// BTT_IsPlayerInRadius.h
// 
// BehaviourTree: Task responsible of check if player is within a given radius
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_IsPlayerInRadius.generated.h"

//fordward declaration
struct FBlackboardKeySelector;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_IsPlayerInRadius : public UBTTaskNode
{
 	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AccetableRadius;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerLocation;

	UPROPERTY(EditAnywhere)
	bool IsGreater;

	UPROPERTY(EditAnywhere)
	bool bReturnFailIfConditionMet;
};
