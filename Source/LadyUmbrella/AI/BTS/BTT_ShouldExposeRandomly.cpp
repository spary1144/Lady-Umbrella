// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ShouldExposeRandomly.h"

EBTNodeResult::Type UBTT_ShouldExposeRandomly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float RandomValue = FMath::FRand();
	
	if (RandomValue <= ExposureProbability)
	{
		return EBTNodeResult::Succeeded;  // Expose
	}
	return EBTNodeResult::Failed; // stay behind cover
}
