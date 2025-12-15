// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AddToFleeQueue.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

EBTNodeResult::Type UBTT_AddToFleeQueue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerMafia* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();

	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}
	//unique id del pawn
	int32 IdPawn = Pawn->GetUniqueID();
	//zone del pawn
	int Currentzone = Controller->GetZone();
	//arena manager del controller
	AArenaManager* CurrentArena = Controller->GetArena();
	if (IsValid(CurrentArena))
	{
		//Add
		if (option == EOptions::Enter)
		{
			//si existe la zona X
			if (CurrentArena->GetFleeQueue()->Contains(Currentzone))
			{
				//comprobar si en esta zona ya existe el enemigo
				if (!CurrentArena->GetFleeQueue()->Find(Currentzone)->EnemiesIDs.Contains(IdPawn))
				{
					//si no existe se le anade
					CurrentArena->GetFleeQueue()->Find(Currentzone)->EnemiesIDs.Add(IdPawn);
				}
			}
			else
			{
				//si no existe la zona, anadir con enemigo
				CurrentArena->GetFleeQueue()->Add(Currentzone).EnemiesIDs.Add(IdPawn);
			}
		}
		else
		{
			//exit
			if (CurrentArena->GetFleeQueue()->Contains(Currentzone))
			{
				CurrentArena->GetFleeQueue()->Find(Currentzone)->EnemiesIDs.Remove(IdPawn);
			}
		}
	}
	return EBTNodeResult::Succeeded;
}
