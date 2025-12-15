// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckFreeCoversInZone.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UBTT_CheckFreeCoversInZone::UBTT_CheckFreeCoversInZone()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_CheckFreeCoversInZone::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerMafia* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}
	int Zone = Blackboard->GetValueAsInt(CurrentZone.SelectedKeyName);
	if (CurrentZone.SelectedKeyName.IsEqual(NextZone))
	{
		Blackboard->SetValueAsBool(FleeingNextZone ,true);
		//Controller->SetZone(Zone);
		//Blackboard->SetValueAsInt(TEXT("CurrentZone"),++Zone);
		//Controller->SetZone(Zone);
		//Blackboard->SetValueAsInt(TEXT("NextZone"), Blackboard->GetValueAsInt(TEXT("NextZone")) + 1);
	}
	else
	{
		Blackboard->SetValueAsBool(FleeingNextZone, false);
	}

	int FreeCovers = Controller->GetArena()->GetFreeCoversInZone(Zone);
	if (FreeCovers != IAConstants::INVALID_NUMBER_DF_ENEMIES && FreeCovers >= IAConstants::MIN_NUMBER_OF_COVERS_IN_ZONE) 
	{ //free covers available
		
		return EBTNodeResult::Succeeded;
	}
	//no free covers
	return EBTNodeResult::Failed;
}
