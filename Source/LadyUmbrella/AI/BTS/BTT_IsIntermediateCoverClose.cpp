// 
// BTT_IsIntermediateCoverClose.cpp
// 
// Implementation of Task
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "BTT_IsIntermediateCoverClose.h"
// #include "LadyUmbrella/AI/Controllers/AIControllerFlankAgency.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

EBTNodeResult::Type UBTT_IsIntermediateCoverClose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerRegularAgency* controller = Cast<AAIControllerRegularAgency>(OwnerComp.GetAIOwner());
	if (!controller)
	{
		return EBTNodeResult::Failed;
	}
	APawn* OnwerPawn = controller->GetPawn();
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	if (!bb)
	{
		return EBTNodeResult::Failed;
	}
	auto cover = Cast<AActor>(bb->GetValueAsObject(Cover.SelectedKeyName));
	double dist = FVector::Distance(cover->GetActorLocation(), OnwerPawn->GetActorLocation());
	if (dist < RadiusSq)
	{
		bb->SetValueAsEnum(FName("StatesFlank"), MoveAroundCovers);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
