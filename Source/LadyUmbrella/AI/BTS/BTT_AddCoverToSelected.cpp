// 
// BTT_AddCoverToSelected.cpp
// 
// Task executed by Behaviour Tree : AddCoverToSelected
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "BTT_AddCoverToSelected.h"
// #include "LadyUmbrella/AI/Controllers/AIControllerFlankAgency.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTT_AddCoverToSelected::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	//AAIControllerFlankAgency* Controller = Cast<AAIControllerFlankAgency>(OwnerComp.GetAIOwner());
	auto Cover = Cast<AActor>(Blackboard->GetValueAsObject(FName("SelectedCover")));
	if (Cover)
	{
		//Controller->AddCoverAsSelected(Cover);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
