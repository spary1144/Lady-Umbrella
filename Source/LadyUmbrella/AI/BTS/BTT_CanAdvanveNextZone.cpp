// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CanAdvanveNextZone.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"


EBTNodeResult::Type UBTT_CanAdvanveNextZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerRegularAgency* Controller = Cast<AAIControllerRegularAgency>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	//check flanker is dead
	if (Controller->GetArena()->GetSpecialistEnemies() != 0)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	//check has shoot all rafagues
	AAgencyCharacter* AgencyCharacter = Cast<AAgencyCharacter>(Controller->GetPawn());
	int CurrentRemainingRafagues = AgencyCharacter->GetRemainingRafagues();
	if (CurrentRemainingRafagues > 0)
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* PlayerCharacter = Cast<AActor>(Blackboard->GetValueAsObject(PlayerKey.SelectedKeyName));
	if (!IsValid(PlayerCharacter))
	{
		return EBTNodeResult::Failed;
	}
	
	// check enemy is behind player
	const FVector& PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector& EnemyLocation  = Controller->GetPawn()->GetActorLocation();
	
	const FVector EnemyToPlayer  = EnemyLocation - PlayerLocation;
	const FVector& PlayerForward = PlayerCharacter->GetActorForwardVector();

	const float DotValue = FVector::DotProduct(EnemyToPlayer.GetSafeNormal(), PlayerForward.GetSafeNormal());
	if (DotValue <= 0.f)
	{
		return EBTNodeResult::Failed;
	}
	//Check player is not close
	const float Distance = FVector::Distance(PlayerLocation, EnemyLocation);
	if (Distance <= Blackboard->GetValueAsFloat(AdvanceRadiusKey.SelectedKeyName))
	{
		return EBTNodeResult::Failed;
	}
	//check nextzone is valid
	const int32 CurrentZone = Blackboard->GetValueAsInt(CurrentZoneKey.SelectedKeyName);
	const int32 NextZone = CurrentZone - 1;
	if (NextZone < 0)
	{
		return EBTNodeResult::Failed;
	}

	AgencyCharacter->SetRandomNumRafagues();
	return EBTNodeResult::Succeeded;
}
