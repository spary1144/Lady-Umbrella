// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChangeEnemyStateIcon.h"

#include "AIController.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"

EBTNodeResult::Type UBTT_ChangeEnemyStateIcon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	if (!Controller->GetShowDebugIcon())
	{
		return EBTNodeResult::Succeeded;
	}
	
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(Controller->GetPawn());
	
	if (!IsValid(EnemyCharacter))
	{
		return EBTNodeResult::Failed;
	}
	
	if (IsValid(StateIcon))
	{
		if (IsValid(EnemyCharacter->GetWidget()))
		{
			EnemyCharacter->GetWidget()->SetImageState(StateIcon);
		}
	}

	return EBTNodeResult::Succeeded;
}
