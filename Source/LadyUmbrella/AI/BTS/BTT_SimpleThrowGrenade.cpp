// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SimpleThrowGrenade.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h"

EBTNodeResult::Type UBTT_SimpleThrowGrenade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMafiaCharacter* character = Cast<AMafiaCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!character)
	{
		return EBTNodeResult::Failed;
	}
	
	character->ThrowGrenade();

	return EBTNodeResult::Succeeded;
}
