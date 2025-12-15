// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RequestToken.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"


EBTNodeResult::Type UBTT_RequestToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = Controller->GetPawn();
	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	AAgencyCharacter* Character = Cast<AAgencyCharacter>(Pawn);
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComponent))
	{
		return EBTNodeResult::Failed;
	}

	if (TokenType == ETokenType::ELECTRIC)
	{
		Character->OnElectricBulletTokenRequest.Broadcast(bIsRequesting, Character);

		if (Character->GetHasElectricBullet() == bIsRequesting)
		{
			return EBTNodeResult::Succeeded;
		}
		return EBTNodeResult::Failed;
	}
	else if (TokenType == ETokenType::FLANK)
	{
		Character->OnFlankTokenRequest.Broadcast(bIsRequesting, Character);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
