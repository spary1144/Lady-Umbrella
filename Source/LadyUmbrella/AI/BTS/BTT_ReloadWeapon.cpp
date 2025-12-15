// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ReloadWeapon.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/EnemyCharacter.h>
#include <LadyUmbrella/Weapons/GenericWeapon.h>

EBTNodeResult::Type UBTT_ReloadWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!Character)
	{
		return EBTNodeResult::Failed;
	}

	if (!Character->GetEquippedWeapon())
	{
		return EBTNodeResult::Failed;
	}

	Character->Reload();
	return EBTNodeResult::Succeeded;
}