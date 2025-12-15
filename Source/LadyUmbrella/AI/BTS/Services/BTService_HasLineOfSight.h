// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_HasLineOfSight.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTService_HasLineOfSight : public UBTService
{
	GENERATED_BODY()

	UPROPERTY()
	UBlackboardComponent* Blackboard;
	
	UPROPERTY()
	AActor* Player;

	UPROPERTY()
	APawn* EnemyPawn;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerKeySelector;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector LineOfSightKeySelector;

	UBTService_HasLineOfSight();
	
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};


