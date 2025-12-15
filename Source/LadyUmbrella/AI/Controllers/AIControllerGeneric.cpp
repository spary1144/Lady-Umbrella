// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerGeneric.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "Navigation/CrowdFollowingComponent.h"

void AAIControllerGeneric::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn && behaviourTree)
	{
		OwnerPawn = InPawn;
		
		//Set blackboard
		UBlackboardComponent* BB;
		UseBlackboard(behaviourTree->BlackboardAsset, BB);
		Blackboard = BB;

		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(OwnerPawn);
		if (IsValid(EnemyCharacter))
		{
			EnemyCharacter->SetAIController(this);
		}

		//Initialize keys
		// Blackboard->SetValueAsInt(TEXT("IndexNextWaypoint"), 0);
		// Blackboard->SetValueAsEnum(TEXT("States"),Patrol);
		//
		// Blackboard->SetValueAsFloat(TEXT("FleeRadius"),FleeRadius);
		// Blackboard->SetValueAsFloat(TEXT("CloseRadius"),CloseRadius);
		//
		// Blackboard->SetValueAsBool(TEXT("IsCovered"),false);
		//
		// Blackboard->SetValueAsBool("FleeingNextZone",false);	
		//Run Behaviour Tree
		RunBehaviorTree(behaviourTree);
	}
}

void AAIControllerGeneric::BeginPlay()
{
	Super::BeginPlay();

	UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();
	if (CrowdFollowingComponent)
	{
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
		CrowdFollowingComponent->SetCrowdSeparation(true);
		CrowdFollowingComponent->SetCrowdSeparationWeight(50.f);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);	
	}
}

AAIControllerGeneric::AAIControllerGeneric(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	BaseTime = 0.2f; // min delay
	DetectionSpeedFactor = 1000.f; // 1s per each 10m
}

void AAIControllerGeneric::InitializeController(AArenaManager* ArenaManager,
	const AAISpawnPoint& SpawnPoint)
{
	CurrentArenaManager = ArenaManager;
	SetZone(SpawnPoint.GetZone());	
}

void AAIControllerGeneric::SetPlayerInBlackboard(AActor* PlayerCharacter) const
{
	Blackboard->SetValueAsObject(FName("Player"), PlayerCharacter);
}

void AAIControllerGeneric::SetRushPlayerBlackboard() const
{
	if (!IsValid(Blackboard) || Blackboard->GetValueAsBool("IsCloseCombat"))
	{
		return;
	}
	
	Blackboard->SetValueAsBool(FName("RushPlayer"),true);
}

void AAIControllerGeneric::DetectPlayer(AActor* PlayerCharacter,bool bDetectionAccordingToDistance)
{
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	if (!bDetectionAccordingToDistance)
	{
		SetPlayerInBlackboard(PlayerCharacter);
		return;
	}

	if (!IsValid(OwnerPawn))
	{
		return;
	}

	//detect by distance
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector MyLocation = OwnerPawn->GetActorLocation();
	float DistanceToPlayer = FVector::Dist(PlayerLocation, MyLocation);

	float DetectionTime = BaseTime + (DistanceToPlayer / DetectionSpeedFactor);
	
	FTimerHandle TimerHandleDetection;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleDetection, [this,PlayerCharacter]()->void
	{
		SetPlayerInBlackboard(PlayerCharacter);
	}, DetectionTime, false, 0);
}

void AAIControllerGeneric::SetZone(int Newzone)
{
	//Blackboard->SetValueAsInt(TEXT("CurrentZone"), Newzone);
	//CurrentArenaManager->ReduceEnemyInZone(CurrentZone);
	CurrentZone = Newzone;
	Blackboard->SetValueAsInt(TEXT("CurrentZone"), CurrentZone);
	CurrentArenaManager->AddEnemyZone(CurrentZone);
	// if (CurrentZone != 0)
	// {
	int Nextzone = CurrentZone + 1;
	Blackboard->SetValueAsInt(TEXT("NextZone"), Nextzone);
	//}
}

void AAIControllerGeneric::TryReduceCloseCombatCounter() const
{
	if (Blackboard->GetValueAsBool("IsCloseCombat"))
	{
		if (!IsValid(CurrentArenaManager))
		{
			return;
		}

		if (!CurrentArenaManager->GetCloseCombatQueue())
		{
			return;
		}
		
		CurrentArenaManager->GetCloseCombatQueue()->Remove(GetPawn()->GetUniqueID());
	}
}

void AAIControllerGeneric::StopBehaviourTree()
{
	UnPossess();
}

void AAIControllerGeneric::CleanBlackboard() const
{
	Blackboard->ClearValue("Player");
}

AArenaManager* AAIControllerGeneric::GetArena() const
{
	return CurrentArenaManager;
}

bool AAIControllerGeneric::GetShowDebugIcon()
{
	return bShowDebugIcons;
}

bool AAIControllerGeneric::IsInCloseCombat() const
{
	if (!IsValid(Blackboard))
	{
		return false;
	}

	return Blackboard->GetValueAsBool(TEXT("IsCloseCombat"));
}

