// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "BTT_ChangeState.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_ChangeState : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector EnumState;

	UPROPERTY(EditAnywhere, Category = "AI")
	TEnumAsByte<EStates> NewState;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
