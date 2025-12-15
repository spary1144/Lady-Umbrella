// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ThrowGrenade.h"
#include <LadyUmbrella/AI/Controllers/AIControllerMafia.h>
#include <LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h>

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

EBTNodeResult::Type UBTT_ThrowGrenade::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMafiaCharacter* Character = Cast<AMafiaCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	if (Character->GetHasGrenadeToken())
	{
		//get player location
		const AActor* Player = Cast<AActor>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
		if (!IsValid(Player))
		{
			return EBTNodeResult::Failed;
		}
	
		float Distance = FVector::Distance(Character->GetActorLocation(), Player->GetActorLocation());
		if (Distance > Character->GetMinThrowGrenadeDistance())
		{
			Character->ThrowGrenade();
		}
		else
		{
			Character->OnGrenadeTokenReleased.Broadcast();
		}

		Character->SetHasGrenadeToken(false);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
