// 
// EQG_GenerateCovers.h
// 
// Implementation of Items generation
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "EQG_GenerateCovers.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQG_GenerateCovers::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	if (SearchedActorClass == nullptr)
	{
		return;
	}

	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(QueryOwner, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return;
	}

	GenerateOnlyActorsInRadius.BindData(QueryOwner, QueryInstance.QueryID);
	bool bUseRadius = GenerateOnlyActorsInRadius.GetValue();

	TArray<AActor*> MatchingActors;
	if (bUseRadius)
	{
		TArray<FVector> ContextLocations;
		QueryInstance.PrepareContext(SearchCenter, ContextLocations);

		SearchRadius.BindData(QueryOwner, QueryInstance.QueryID);
		const float RadiusValue = SearchRadius.GetValue();
		const float RadiusSq = FMath::Square(RadiusValue);

		for (TActorIterator<AActor> ItActor = TActorIterator<AActor>(World, SearchedActorClass); ItActor; ++ItActor)
		{
			for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ++ContextIndex)
			{
				if (FVector::DistSquared(ContextLocations[ContextIndex], ItActor->GetActorLocation()) < RadiusSq
					/* && !ItActor->ActorHasTag(TagFilter) */ )
				{
					MatchingActors.Add(*ItActor);
					break;
				}
			}
		}
	}
	else
	{	// If radius is not positive, ignore Search Center and Search Radius and just return all actors of class.
		for (TActorIterator<AActor> ItActor = TActorIterator<AActor>(World, SearchedActorClass); ItActor; ++ItActor)
		{
			MatchingActors.Add(*ItActor);
		}
	}

	ProcessItems(QueryInstance, MatchingActors);
	QueryInstance.AddItemData<UEnvQueryItemType_Actor>(MatchingActors);
}
