// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckEnemiesInZone.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UBTT_CheckEnemiesInZone::UBTT_CheckEnemiesInZone()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_CheckEnemiesInZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}
	
	int CurrentZone = Blackboard->GetValueAsInt("CurrentZone");//controller->GetZone();
	
	if (!IsValid( Controller->GetArena())) return EBTNodeResult::Failed;

	int NumEnemies = Controller->GetArena()->GetEnemiesInZone(CurrentZone);

	if (NumEnemies != IAConstants::INVALID_NUMBER_DF_ENEMIES && NumEnemies > IAConstants::MIN_NUMBER_OF_ENEMIES_IN_ZONE)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
