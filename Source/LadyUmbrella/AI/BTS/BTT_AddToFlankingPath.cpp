// 
// BTT_AddToFlankingPath.cpp
// 
// BehaviourTree: Implementation of Task
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "BTT_AddToFlankingPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"

EBTNodeResult::Type UBTT_AddToFlankingPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerRegularAgency* Controller = Cast<AAIControllerRegularAgency>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	//get goalCover
	auto Cover = Cast<AAICoverEntryPoint>(Blackboard->GetValueAsObject(CoverToFlank.SelectedKeyName));
	//
	bool bFirsttime = Controller->IsFlankCoverEmpty();
	//Add location to array
	if (Cover)
	{
		Controller->AddFlankCover(Cover);
	}
	if (bFirsttime)
	{
		//Chang	e State , calcule intermediate points
		Blackboard->SetValueAsEnum(FName("StatesFlank"),SearchIntermediatePoints);
	}


	return EBTNodeResult::Succeeded;
}
