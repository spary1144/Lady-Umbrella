// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckIfCloseCombat.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

class AArenaManager;

UBTService_CheckIfCloseCombat::UBTService_CheckIfCloseCombat()
{
	bNotifyBecomeRelevant = true;
}

void UBTService_CheckIfCloseCombat::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsValid(Pawn) || !IsValid(Player))
	{
		return;
	}
	
	if (FVector::Distance(Pawn->GetActorLocation(), Player->GetActorLocation()) > Blackboard->GetValueAsFloat("CloseRadius"))
	{
		return;
	}

	if (CloseCombatQueue->Contains(IdPawn))
	{
		return;
	}

	if (CloseCombatQueue->Num() < MaxEnemiesInCloseCombat)
	{
		Blackboard->SetValueAsBool("IsCloseCombat", true);
		CloseCombatQueue->Add(IdPawn);
	}
}

void UBTService_CheckIfCloseCombat::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	Blackboard = OwnerComp.GetBlackboardComponent();
	AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());

	if (!IsValid(Blackboard) || !IsValid(Controller))
	{
		return;
	}

	Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	Player = Cast<APlayerCharacter>(Blackboard->GetValueAsObject("Player"));

	Arena = Controller->GetArena();
	if (!IsValid(Arena))
	{
		return;
	}

	CloseCombatQueue = Arena->GetCloseCombatQueue();
	if (!CloseCombatQueue)
	{
		return;
	}

	IdPawn = Pawn->GetUniqueID();
}
