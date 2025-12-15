// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DrawDebugFlankingPath.generated.h"

class AAICoverEntryPoint;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_DrawDebugFlankingPath : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void DrawNextCover();

	UPROPERTY()
	TArray<AAICoverEntryPoint*> Covers;

	int32 CurrentIndex = 0;

	FTimerHandle TimerHandle;
	UWorld* World = nullptr;
	UBehaviorTreeComponent* BehaviorTreeComp = nullptr;

};
