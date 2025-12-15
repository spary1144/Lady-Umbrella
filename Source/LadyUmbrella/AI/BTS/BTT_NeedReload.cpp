// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_NeedReload.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/EnemyCharacter.h>
#include <LadyUmbrella/Weapons/GenericWeapon.h>
#include <LadyUmbrella/Util/LoggerUtil.h>

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_NeedReload::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	AEnemyCharacter* Character = Cast<AEnemyCharacter>(Controller->GetPawn());
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}

	AGenericWeapon* EquippedWeapon = Character->GetEquippedWeapon();
	if (!IsValid(EquippedWeapon))
	{
		return EBTNodeResult::Failed;
	}

	float MinAmmoToReload;
	if (Blackboard->GetValueAsBool("IsCloseCombat"))
	{
		MinAmmoToReload = 0;
	}
	else
	{
		MinAmmoToReload = EquippedWeapon->GetMinAmmoToStartReloading();
	}

	if (EquippedWeapon->GetAmmoCurrent() <= MinAmmoToReload && !Character->IsReloading())
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
