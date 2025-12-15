// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ReduceZone.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"

EBTNodeResult::Type UBTT_ReduceZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	int CurrentZone = Blackboard->GetValueAsInt(CurrentZoneKey.SelectedKeyName);
	CurrentZone--;
	Blackboard->SetValueAsInt(CurrentZoneKey.SelectedKeyName, CurrentZone);

	return EBTNodeResult::Succeeded;
}
