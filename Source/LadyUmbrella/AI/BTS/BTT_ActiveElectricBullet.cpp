// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ActiveElectricBullet.h"

#include "AIController.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"

EBTNodeResult::Type UBTT_ActiveElectricBullet::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	AAgencyCharacter* Character = Cast<AAgencyCharacter>(Pawn);
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	AGenericWeapon* EquippedWeapon = Character->GetEquippedWeapon();
	if (!IsValid(EquippedWeapon))
	{
		return EBTNodeResult::Failed;
	}

	TObjectPtr<UStatusEffectComponent> CharacterStatus = Character->GetStatusEffectComponent();

	if (!IsValid(CharacterStatus))
	{
		FLogger::ErrorLog("Invalid CharacterStatus pointer in BTT_ActiveElectricBullet::ExecuteTask()");
		return EBTNodeResult::Failed;
	}
	
	if (!EquippedWeapon->IsElectricBulletActivated() && !CharacterStatus->IsElectrified())
	{
		if (!IsValid(FGlobalPointers::PlayerCharacter))
		{
			return EBTNodeResult::Failed;
		}
		
		if (Character->IsEnemyInFrontOf() || Character->IsPlayerTurnHisBack() || Character->IsReloading())
		{
			Character->OnElectricBulletTokenRequest.Broadcast(false, Character);
			return EBTNodeResult::Failed; //Ally in front of enemy
		}
		
		EquippedWeapon->SetWeaponBulletType(EBulletType::Electric);
		EquippedWeapon->SetElectricBulletActivated(true);
		Character->FireElectricProjectile();
	}

	return EBTNodeResult::Succeeded;
}

