// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckIfCanEnterCloseCombat.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

class AArenaManager;

EBTNodeResult::Type UBTT_CheckIfCanEnterCloseCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	AArenaManager* Arena = Controller->GetArena();
	if (!IsValid(Arena))
	{
		return EBTNodeResult::Failed;
	}

	TArray<int32>* CloseCombatQueue = Arena->GetCloseCombatQueue();
	if (!CloseCombatQueue)
	{
		return EBTNodeResult::Failed;
	}

	int32 IdPawn = Pawn->GetUniqueID();

	if (CloseCombatQueue->Contains(IdPawn))
	{
		return EBTNodeResult::Succeeded;
	}
	

	if (CloseCombatQueue->Num() < MaxEnemiesInCloseCombat)
	{
		CloseCombatQueue->Add(IdPawn);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
