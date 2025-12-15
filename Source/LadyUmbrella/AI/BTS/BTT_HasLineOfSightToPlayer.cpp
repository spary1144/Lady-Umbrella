// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_HasLineOfSightToPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Components/CoverComponent/CoverEnums.h"

bool UBTT_HasLineOfSightToPlayer::CheckIfOutOfCover(APlayerCharacter* Player)
{
	return (Player->GetCoverMovementComponent()->IsMovementControlledByCover() && Player->GetCoverMovementComponent()->GetInsideCoverState() ==
		EInsideCoverState::Aiming) || !(Player->GetCoverMovementComponent()->IsMovementControlledByCover()) || !bNeedPlayerCoverState;
}

EBTNodeResult::Type UBTT_HasLineOfSightToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* Controller =  Cast<AAIController>(Character->GetController());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector StartEnemy;
	FRotator PlayerRotation;
	FVector PlayerEye;
	
	Controller->GetPlayerViewPoint(StartEnemy, PlayerRotation);
	
	StartEnemy.Z += StartTraceVerticalOffset;
	
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	TArray<AActor*> AttachedActors;
	Character->GetAttachedActors(AttachedActors);

	for (AActor* Child : AttachedActors)
	{
		ActorsToIgnore.Add(Child);
	}
	const UWorld* World = GetWorld();
	if (!IsValid(World)) return EBTNodeResult::Failed;
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
	if (!IsValid(Player))
	{
		return EBTNodeResult::Failed;
	}

	if (IsValid(Player->GetWeapon()))
	{
		ActorsToIgnore.Add(Player->GetWeapon());
	}
	
	if (CheckIfOutOfCover(Player))
	{
		Player->GetActorEyesViewPoint(PlayerEye, PlayerRotation);

		UKismetSystemLibrary::LineTraceSingle(World, StartEnemy, PlayerEye, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Emerald);
	
		if (OutHit.bBlockingHit)
		{
			if (OutHit.GetActor() == Player)
			{
				Blackboard->SetValueAsBool("HasLineOfSight", true);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	else
	{
		FVector PlayerCoveredLocation = Player->GetActorLocation() - Player->GetActorUpVector() * 20;
		//UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartEnemy, PlayerCoveredLocation, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true, FColor::Blue);
		UKismetSystemLibrary::SphereTraceSingle(World, StartEnemy, PlayerCoveredLocation, 10, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Blue);
		if (OutHit.bBlockingHit)
		{
			if (OutHit.GetActor() == Player)
			{
				Blackboard->SetValueAsBool("HasLineOfSight", true);
				return EBTNodeResult::Succeeded;
			}
		}
	}

	Blackboard->SetValueAsBool("HasLineOfSight", false);
	return EBTNodeResult::Failed;
}
