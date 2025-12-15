// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckIfCanFlee.h"

#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

UBTT_CheckIfCanFlee::UBTT_CheckIfCanFlee()
{
	// if (bShootUntilRunOutAllBullets)
	// {
	bNotifyTick = true;
	bCreateNodeInstance = true;
	//}
}

EBTNodeResult::Type UBTT_CheckIfCanFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerMafia* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	//FLogger::DebugLog("Check if Can flee");
	int32 CurrentZone = Controller->GetZone();
	AArenaManager* Arena = Controller->GetArena();
	if (!IsValid(Arena))
	{
		return EBTNodeResult::Failed;
	}

	if (Arena->GetFleeQueue())
	{
		if (Arena->GetFleeQueue()->Contains(CurrentZone))
		{
			// if (Controller->GetArena()->GetEnemiesInZone(CurrentZone) <= 1)
			// {
			// 	FLogger::DebugLog("ESTOY SOLO EN MI ZONA: %d", CurrentZone);
			// 	return EBTNodeResult::Failed;
			// }
			int32 IDEnemy;
			
			if (Arena->GetFleeQueue()->Find(CurrentZone) && Controller->GetArena()->GetFleeQueue()->Find(CurrentZone)->EnemiesIDs.IsValidIndex(0))
			{
				IDEnemy = Controller->GetArena()->GetFleeQueue()->Find(CurrentZone)->EnemiesIDs[0];
			}
			else
			{
				return EBTNodeResult::Failed;
			}
			
			int32 EnemiesInZone = Controller->GetArena()->GetEnemiesInZone(CurrentZone);
			if (EnemiesInZone <= IAConstants::MIN_NUMBER_OF_ENEMIES_IN_ZONE)
			{
				return EBTNodeResult::Failed;
			}
			if (IDEnemy == Controller->GetPawn()->GetUniqueID())
			{
				Controller->GetArena()->ReduceEnemyInZone(Controller->GetZone());
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
