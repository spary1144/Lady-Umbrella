// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_IsElectricProjectileShooted.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"


UBTT_IsElectricProjectileShooted::UBTT_IsElectricProjectileShooted()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_IsElectricProjectileShooted::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	
	APawn* Pawn = Controller->GetPawn();
	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}
	
	Character = Cast<AAgencyCharacter>(Pawn);
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	Weapon = Character->GetEquippedWeapon();
	if (!IsValid(Weapon))
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::InProgress;
}

void UBTT_IsElectricProjectileShooted::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	// TODO: Check if Character & Character->GetStatusComponent() are valid. 
	if (Character->GetStatusEffectComponent()->IsElectrified())
	{
		Character->AbortElectricShoot();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	if (Weapon->IsElectricBulletShooted())
	{
		Weapon->SetElectricBulletShooted(false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

