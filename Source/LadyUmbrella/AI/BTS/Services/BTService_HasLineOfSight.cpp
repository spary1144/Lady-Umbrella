// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_HasLineOfSight.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTService_HasLineOfSight::UBTService_HasLineOfSight()
{
	bNotifyBecomeRelevant = true;
}

void UBTService_HasLineOfSight::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	EnemyPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!IsValid(EnemyPawn))
	{
		return;
	}
	
	AAIController* Controller = Cast<AAIController>(EnemyPawn->GetController());
	if (!IsValid(Controller))
	{
		return;
	}

	Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return;
	}

	Player = Cast<AActor>(Blackboard->GetValueAsObject(PlayerKeySelector.SelectedKeyName));
}

void UBTService_HasLineOfSight::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (!IsValid(Player))
	{
		return;
	}

	FVector StartEnemy;
	FRotator PlayerRotation;
	EnemyPawn->GetController()->GetPlayerViewPoint(StartEnemy, PlayerRotation);
	
	FVector PlayerCoveredLocation = Player->GetActorLocation() - Player->GetActorUpVector() * 20;
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(EnemyPawn);
	
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), StartEnemy, PlayerCoveredLocation, 10, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FColor::Blue);
	if (OutHit.bBlockingHit)
	{
		if (OutHit.GetActor() == Player)
		{
			Blackboard->SetValueAsBool(LineOfSightKeySelector.SelectedKeyName,true);
		}
	}
}
