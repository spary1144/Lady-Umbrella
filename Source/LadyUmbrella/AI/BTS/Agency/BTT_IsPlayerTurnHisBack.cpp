// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_IsPlayerTurnHisBack.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"

EBTNodeResult::Type UBTT_IsPlayerTurnHisBack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAgencyCharacter* Character = Cast<AAgencyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}

	if (!Character->IsPlayerTurnHisBack())
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Succeeded;
}
