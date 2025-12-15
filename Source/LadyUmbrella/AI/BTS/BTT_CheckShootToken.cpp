// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckShootToken.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"

EBTNodeResult::Type UBTT_CheckShootToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Controller->GetPawn());
	if (!IsValid(EnemyCharacter))
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsBool("HasToken", EnemyCharacter->GetHasToken());

	return EBTNodeResult::Succeeded;
}
