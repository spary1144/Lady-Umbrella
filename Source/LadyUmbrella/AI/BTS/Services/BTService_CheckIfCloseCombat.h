// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckIfCloseCombat.generated.h"

class UBlackboardComponent;
class APlayerCharacter;
class AArenaManager;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTService_CheckIfCloseCombat : public UBTService
{
	GENERATED_BODY()

	UBTService_CheckIfCloseCombat();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY()
	APlayerCharacter* Player;

	UPROPERTY()
	UBlackboardComponent* Blackboard;

	UPROPERTY()
	APawn* Pawn;

	UPROPERTY()
	AArenaManager* Arena;
	
	TArray<int32>* CloseCombatQueue;

	int32 IdPawn;

	UPROPERTY(EditAnywhere)
	int MaxEnemiesInCloseCombat = 2;
};
