// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_IsPlayerAimingFromCover.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"

EBTNodeResult::Type UBTT_IsPlayerAimingFromCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(Blackboard->GetValueAsObject(PlayerKey.SelectedKeyName));
	if (!IsValid(Player))
	{
		return EBTNodeResult::Failed;
	}

	if (!bCheckIsAimingFromCover && !Player->GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		return EBTNodeResult::Succeeded;
	}

	if (bCheckIsAimingFromCover && !Player->GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		if (Player->GetCoverMovementComponent()->GetInsideCoverState() == EInsideCoverState::Aiming)
		{
			return EBTNodeResult::Succeeded;
		}
	}
	
	return EBTNodeResult::Failed;
}
