// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FireWeapon.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/EnemyCharacter.h>
#include <LadyUmbrella/Weapons/GenericWeapon.h>
#include <LadyUmbrella/Util/LoggerUtil.h>

#include "BehaviorTree/BlackboardComponent.h"

UBTT_FireWeapon::UBTT_FireWeapon()
{
	// if (bShootUntilRunOutAllBullets)
	// {
	bNotifyTick = true;
	bCreateNodeInstance = true;
	//}
}

// bool UBTT_FireWeapon::HandleFire()
// {
// 	if (!IsCharacterReadyToFire(EnemyCharacter))
// 	{
// 		return false;
// 	}
// 	
// 	if (ElapsedTime >= EnemyCharacter->GetEquippedWeapon()->GetWeaponCadence())
// 	{
// 		ElapsedTime = ElapsedTime - EnemyCharacter->GetEquippedWeapon()->GetWeaponCadence();
//
// 		if (EnemyCharacter->GetEquippedWeapon()->GetAmmoCurrent() > 0)
// 		{
// 			EnemyCharacter->Fire();
// 			return false;
// 		}
// 		return true;
// 	}
// 	
// 	ElapsedTime += GetWorld()->GetDeltaSeconds();
// 	
// 	return false;
// }

bool UBTT_FireWeapon::IsCharacterReadyToFire()
{
	return (IsValid(EnemyCharacter) && EnemyCharacter->GetHasToken() && EnemyCharacter->GetEquippedWeapon() && !EnemyCharacter->IsReloading());
}

EBTNodeResult::Type UBTT_FireWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EnemyCharacter = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!IsValid(EnemyCharacter))
	{
		return EBTNodeResult::Failed;
	}

	Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}
	
	// //dispara las balas hasta quedarse sin munición
	// if (bShootUntilRunOutAllBullets)
	// {
	// 	return EBTNodeResult::InProgress;
	// }
	
	if (!IsCharacterReadyToFire())
	{
		Blackboard->SetValueAsBool(FName("HasToken"), false);
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsBool(FName("HasToken"), true);

	GenericWeapon = EnemyCharacter->GetEquippedWeapon();
	if (!IsValid(GenericWeapon))
	{
		return EBTNodeResult::Failed;
	}

	WeaponCadence = GenericWeapon->GetRandomWeaponCadence();
	
	//Dispara una bala si puede y succes la task
	// HandleFire();
	return EBTNodeResult::InProgress;
}

void UBTT_FireWeapon::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!IsValid(EnemyCharacter))
	{
		return;
	}

	if (GenericWeapon->GetAmmoCurrent() > 0)
	{
		EnemyCharacter->TryToFire();

		if (!bShootUntilRunOutAllBullets)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
