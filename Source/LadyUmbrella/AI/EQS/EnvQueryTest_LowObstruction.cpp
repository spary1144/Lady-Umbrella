// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_LowObstruction.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UEnvQueryTest_LowObstruction::UEnvQueryTest_LowObstruction()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_Point::StaticClass();
	TestPurpose = EEnvTestPurpose::Filter;
}

void UEnvQueryTest_LowObstruction::RunTest(FEnvQueryInstance& QueryInstance) const
{
	AActor* Querier = Cast<AActor>(QueryInstance.Owner.Get());

	if (!IsValid(Querier))
	{
		return;
	}

	const FVector ForwardVector = Querier->GetActorForwardVector();

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		const FVector TraceStart = ItemLocation;
		const FVector TraceEnd = TraceStart + ForwardVector * 200;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Querier);

		const bool bHit = QueryInstance.World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

		const bool bPasses = !bHit;
		float Score;
		if (bPasses)
		{
			Score = 1;
		}
		else
		{
			Score = 0;
		}
		
		It.SetScore(TestPurpose, EEnvTestFilterType::Range, Score, 0, 1);
	}
	
}
