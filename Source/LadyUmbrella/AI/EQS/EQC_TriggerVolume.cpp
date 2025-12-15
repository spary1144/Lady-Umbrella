// Fill out your copyright notice in the Description page of Project Settings.


#include "EQC_TriggerVolume.h"

#include "Engine/TriggerVolume.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

void UEQC_TriggerVolume::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	
	UObject* Querier = QueryInstance.Owner.Get();
	if (!IsValid(Querier))
	{
		return;
	}
	
	AActor* QuerierActor = Cast<AActor>(Querier);
	if (!IsValid(QuerierActor))
	{
		return;
	}

	AAIControllerGeneric* AIController = Cast<AAIControllerGeneric>(QuerierActor->GetInstigatorController());
	if (!IsValid(AIController))
	{
		return;
	}

	AArenaManager* ArenaManager = AIController->GetArena();

	if (!IsValid(ArenaManager))
	{
		return;
	}
	
	if (!IsValid(ArenaManager->GetEnemySpawnerComponent()))
	{
		return;
	}

	ATriggerVolume* TriggerVolume = ArenaManager->GetEnemySpawnerComponent()->GetTriggerVolume();
	if (!TriggerVolume)
	{
		return;
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, TriggerVolume);
}
