// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FireRafague.generated.h"

class AAgencyCharacter;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_FireRafague : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY()
	AAgencyCharacter* EnemyCharacter;

	int32 NumBulletsInRafague;
	
	UPROPERTY(EditAnywhere)
	int32 MinRangueOfBulletsInRafague;
	
	UPROPERTY(EditAnywhere)
	int32 MaxRangueOfBulletsInRafague;
		
	UBTT_FireRafague();
	bool IsCharacterReadyToFire() const;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
