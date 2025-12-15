// 
// BTT_CheckIfCanFlee.h
// 
// BehaviourTree: Task to add one enemy to flee queue
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AddToFleeQueue.generated.h"

UENUM(BlueprintType)
enum class EOptions : uint8
{
	Enter UMETA(DisplayName = "Enter"),
	Exit UMETA(DisplayName = "Exit")
};

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_AddToFleeQueue : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector Array;

	UPROPERTY(EditAnywhere)
	EOptions option = EOptions::Enter;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
