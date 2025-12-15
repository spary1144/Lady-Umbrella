// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CheckIfCanEnterCloseCombat.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_CheckIfCanEnterCloseCombat : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int MaxEnemiesInCloseCombat = 2;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
};
