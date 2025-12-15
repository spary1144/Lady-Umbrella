// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ShouldExposeRandomly.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_ShouldExposeRandomly : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "AI")
	float ExposureProbability = 0.7f; // probability to expose
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
