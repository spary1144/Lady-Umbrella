// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FireRafague.h"

#include "AIController.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"

UBTT_FireRafague::UBTT_FireRafague()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	NumBulletsInRafague = 0;
	MinRangueOfBulletsInRafague = 3;
	MaxRangueOfBulletsInRafague = 5;
}

bool UBTT_FireRafague::IsCharacterReadyToFire() const
{
	return (IsValid(EnemyCharacter) && EnemyCharacter->GetHasToken() && EnemyCharacter->GetEquippedWeapon());
}

EBTNodeResult::Type UBTT_FireRafague::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EnemyCharacter = Cast<AAgencyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!IsValid(EnemyCharacter))
	{
		return EBTNodeResult::Failed;
	}
	if (!IsCharacterReadyToFire())
	{
		return EBTNodeResult::Failed;
	}
	//if can not advance then generate random rafagues again
	if (EnemyCharacter->GetRemainingRafagues() == 0)
	{
		EnemyCharacter->SetRandomNumRafagues();
	}
	
	NumBulletsInRafague = FMath::RandRange(MinRangueOfBulletsInRafague,MaxRangueOfBulletsInRafague);
	
	return EBTNodeResult::InProgress;
}

void UBTT_FireRafague::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!EnemyCharacter)
	{
		return;
	}
	
	AGenericWeapon* GenericWeapon = EnemyCharacter->GetEquippedWeapon();
	if (!IsValid(GenericWeapon))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (EnemyCharacter->TryToFire())
	{
		NumBulletsInRafague--;
		if (NumBulletsInRafague == 0)
		{
			int RafaguesRemaining = EnemyCharacter->GetRemainingRafagues();
			EnemyCharacter->SetRemainingRafagues(--RafaguesRemaining);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else if (GenericWeapon->GetAmmoCurrent() <= 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
