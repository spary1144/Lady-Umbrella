// Fill out your copyright notice in the Description page of Project Settings.

#include "BTT_TryToVault.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"

EBTNodeResult::Type UBTT_TryToVault::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	if (FMath::RandBool() && !bShouldAlwaysVault)
	{
		return EBTNodeResult::Failed;
	}
	
	if (Character->GetInteractiveMovementComponent())
	{
		Character->GetCoverMovementComponent()->NotifyExitCover();
		bool bCanVault = Character->GetInteractiveMovementComponent()->CheckVault();
		
		return bCanVault ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
