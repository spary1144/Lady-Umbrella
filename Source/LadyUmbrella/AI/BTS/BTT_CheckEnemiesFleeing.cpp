// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckEnemiesFleeing.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

void UBTT_CheckEnemiesFleeing::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	MaxEnemiesFleeing = 1;
}

EBTNodeResult::Type UBTT_CheckEnemiesFleeing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerMafia* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}
	const UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	int CurrentZone = Blackboard->GetValueAsInt("CurrentZone");//controller->GetZone();
	if (Controller->GetArena()->GetFleeQueue()->Contains(CurrentZone))
	{
		int EnemiesFleeingSize = Controller->GetArena()->GetFleeQueue()->Find(CurrentZone)->EnemiesIDs.Num();
		if (EnemiesFleeingSize > MaxEnemiesFleeing)
		{
			return EBTNodeResult::Succeeded;
		}
	}
	
	return EBTNodeResult::Failed;
}
