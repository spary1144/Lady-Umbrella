// Fill out your copyright notice in the Description page of Project Settings.


#include "EQT_MultiTraceTo.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"

UEQT_MultiTraceTo::UEQT_MultiTraceTo()
{
	SetWorkOnFloatValues(false);
	TestPurpose = EEnvTestPurpose::FilterAndScore;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
}

void UEQT_MultiTraceTo::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* Owner = QueryInstance.Owner.Get();

	if (!Owner)
	{
		return;
	}
	// get pawn
	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!PawnOwner)
	{
		return;
	}
	// get ai controller
	AAIController* Controller = Cast<AAIController>(PawnOwner->GetController());
	if (!Controller)
	{
		return;
	}
	//bind bool test
	BoolValue.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	const bool bwantinside = BoolValue.GetValue();
	//get player and playerlocation
	AActor* Player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));

	if (!IsValid(Player))
	{
		return;
	}
	
	//Check for all items if are contained in OcupedCovers
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		if (AActor* actor = GetItemActor(QueryInstance, It.GetIndex()))
		{
			//trace
			bool binside = false;

			//entry point
			AAICoverEntryPoint* cover = Cast<AAICoverEntryPoint>(actor);

			FVector CoverLocationOffset = FVector(actor->GetActorLocation().X, actor->GetActorLocation().Y, actor->GetActorLocation().Z + 580.f);
			
			//multi line trace
			TArray<struct FHitResult> HitResults;
			UKismetSystemLibrary::LineTraceMulti(
				GetWorld(),
				actor->GetActorLocation(),
				Player->GetActorLocation(), //probar a meter offset
				static_cast<ETraceTypeQuery>(ECC_Camera), //antiguo->ETraceTypeQuery::TraceTypeQuery1
				false,
				{},
				// SET TO FOR DURATION FOR DEBUG
				EDrawDebugTrace::None,
				HitResults,
				false,
				FLinearColor::Blue,
				FLinearColor::Red,
				5.f
				);

			//Sort by distance
			HitResults.Sort([](const FHitResult& A, const FHitResult& B) {
					return A.Distance < B.Distance;
			});

			//if hits with their own cover, pass test
			if(HitResults.Num() > 0)
			{
				if (cover->GetOwningSpline())
				{
					if (HitResults[0].GetActor() == cover->GetOwningSpline()->GetOwner())
					{
						binside = true;
					}	
				}
			}
			
			It.SetScore(TestPurpose, FilterType, binside, bwantinside);
		}
	}
}
