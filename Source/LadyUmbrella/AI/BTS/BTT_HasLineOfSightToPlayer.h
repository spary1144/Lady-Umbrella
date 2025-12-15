// 
// BTT_HasLineOfSightToPlayer.h
// 
// BehaviourTree: Task to know if agent has line of sight
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "BTT_HasLineOfSightToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_HasLineOfSightToPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true,ClampMax = 0.f))
	float StartTraceVerticalOffset = 0.f;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true,ClampMax = 0.f))
	bool bNeedPlayerCoverState = true;

	bool CheckIfOutOfCover(APlayerCharacter* Player);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
