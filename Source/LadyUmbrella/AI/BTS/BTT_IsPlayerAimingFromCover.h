// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_IsPlayerAimingFromCover.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_IsPlayerAimingFromCover : public UBTTaskNode
{
	GENERATED_BODY()
	
    UPROPERTY(EditAnywhere)
    FBlackboardKeySelector PlayerKey;

	UPROPERTY(EditAnywhere)
	bool bCheckIsAimingFromCover;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    
};
