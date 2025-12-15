// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_IsPlayerAiming.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

EBTNodeResult::Type UBTT_IsPlayerAiming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	APlayerCharacter* Player = Cast<APlayerCharacter>(Blackboard->GetValueAsObject(PlayerKey.SelectedKeyName));
	if (!Player)
	{
		return EBTNodeResult::Failed;
	}

	if (Player->IsAiming())
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
