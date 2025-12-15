// Fill out your copyright notice in the Description page of Project Settings.


#include "EQT_LineTraceToPlayer.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"

UEQT_LineTraceToPlayer::UEQT_LineTraceToPlayer()
{
	SetWorkOnFloatValues(false);
	TestPurpose = EEnvTestPurpose::FilterAndScore;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
}

void UEQT_LineTraceToPlayer::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* Owner = QueryInstance.Owner.Get();

	if (!IsValid(Owner))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	// get pawn
	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!IsValid(PawnOwner))
	{
		return;
	}
	// get ai controller
	// AAIControllerMafia* Controller = Cast<AAIControllerMafia>(PawnOwner->GetController());
	// if (!Controller)
	// {
	// 	return;
	// }
	//bind bool test
	BoolValue.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	const bool bwantinside = BoolValue.GetValue();
	//get player and playerlocation
	AActor* Player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(World,0));
	
	//Check for all items if are contained in OcupedCovers
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		if (AActor* actor = GetItemActor(QueryInstance, It.GetIndex()))
		{
			//trace
			bool binside = false;

			//entry point
			AAICoverEntryPoint* cover = Cast<AAICoverEntryPoint>(actor);

			//FVector CoverLocationOffset = FVector(actor->GetActorLocation().X, actor->GetActorLocation().Y, actor->GetActorLocation().Z + 580.f);
			
			//multi line trace
			// TArray<struct FHitResult> HitResults;
			// UKismetSystemLibrary::LineTraceMulti(
			// 	GetWorld(),
			// 	actor->GetActorLocation(),
			// 	Player->GetActorLocation(), //probar a meter offset
			// 	static_cast<ETraceTypeQuery>(ECC_Camera), //antiguo->ETraceTypeQuery::TraceTypeQuery1
			// 	false,
			// 	{},
			// 	// SET TO FOR DURATION FOR DEBUG
			// 	EDrawDebugTrace::None,
			// 	HitResults,
			// 	false
			// 	);
			if (World && IsValid(actor) && IsValid(Player))
			{
				struct FHitResult OutHit;
				World->LineTraceSingleByChannel(
					OutHit,
					cover->GetActorLocation(),
					Player->GetActorLocation(),
					ECollisionChannel::ECC_Visibility //ECollisionChannel::
				);
				if (IsValid(OutHit.GetActor()) && IsValid(cover->GetOwningSpline()) && OutHit.GetActor() == cover->GetOwningSpline()->GetOwner())
				{
					binside = true;
				}
			}

			//Sort by distance
			// HitResults.Sort([](const FHitResult& A, const FHitResult& B) {
			// 		return A.Distance < B.Distance;
			// });

			//if hits with their own cover, pass test
			// if(HitResults.Num() > 0)
			// {
			// 	if (cover->GetOwningSpline())
			// 	{
			// 		if (HitResults[0].GetActor() == cover->GetOwningSpline()->GetOwner())
			// 		{
			// 			binside = true;
			// 		}	
			// 	}
			// }
			
			It.SetScore(TestPurpose, FilterType, binside, bwantinside);
		}
	}
}
