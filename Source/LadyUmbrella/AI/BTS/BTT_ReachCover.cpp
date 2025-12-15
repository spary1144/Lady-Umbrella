// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ReachCover.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"

UBTT_ReachCover::UBTT_ReachCover()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ReachCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = Controller->GetBlackboardComponent();
	if (Controller)
	{
		auto GenericCharacter = Cast<AGenericCharacter>(Controller->GetPawn());
		GenericCharacter->GetCoverMovementComponent()->NotifyReachedCoverTravellingTowards();
		BB->SetValueAsBool(TEXT("IsCovered"),true);
	}
	return EBTNodeResult::Succeeded;
}
