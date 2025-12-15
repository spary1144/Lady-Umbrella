// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChangeEnemyZone.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"

EBTNodeResult::Type UBTT_ChangeEnemyZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	int Zone = Blackboard->GetValueAsInt(CurrentZone.SelectedKeyName);
	
	Controller->SetZone(Zone);
	
	return EBTNodeResult::Succeeded;
}
