// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_RotateToTarget : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;
	
	UPROPERTY()
	APawn* EnemyCharacter;

	UPROPERTY()
	UBlackboardComponent* Blackboard;

	
	FVector Target;

	UPROPERTY(EditAnywhere, Category="Rotate")
	float RotationSpeed;

	const float MIN_ANGLE_DIFFERENCE = 5.f;
	
	UBTT_RotateToTarget();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
