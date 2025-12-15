// 
// BTT_NextFlankCover.cpp
// 
// BehaviourTree: Implementation of Task
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "BTT_NextFlankCover.h"
// #include "LadyUmbrella/AI/Controllers/AIControllerFlankAgency.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerRegularAgency.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"

EBTNodeResult::Type UBTT_NextFlankCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerRegularAgency* Controller = Cast<AAIControllerRegularAgency>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	AAgencyCharacter* Character = Cast<AAgencyCharacter>(Controller->GetPawn());
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	if(!Controller->IsFlankCoverEmpty()) {
		AAICoverEntryPoint* Cover = Controller->GetCoversSelected().Pop();
		if (IsValid(Cover))
		{
			Blackboard->SetValueAsObject(NextLocation.SelectedKeyName, Cover);
			Blackboard->SetValueAsVector(TEXT("LocationNextFlankPoint"), Cover->GetActorLocation());
			Character->PlayFlankVoiceLine();
			return EBTNodeResult::Succeeded;
		}
	}

	Character->PlayFlankVoiceLine();
	Blackboard->SetValueAsBool(TEXT("IsAlreadyAtGoal"),true);
	//selected covers is empty-> chase player
	//bb->SetValueAsBool(FName("ChasePlayer"), true);
	//bb->SetValueAsEnum(FName("States"),Attack);
	return EBTNodeResult::Succeeded;
}
