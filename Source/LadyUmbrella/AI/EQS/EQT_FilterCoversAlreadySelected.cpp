// 
// EQT_FilterCoversAlreadySelected.cpp
// 
// Enviroment query test -> Implementation of RunTest
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "EQT_FilterCoversAlreadySelected.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/Environment/CoverSystem/AICoverEntryPoint.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"


UEQT_FilterCoversAlreadySelected::UEQT_FilterCoversAlreadySelected()
{
	SetWorkOnFloatValues(false);
	TestPurpose = EEnvTestPurpose::FilterAndScore;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
}

void UEQT_FilterCoversAlreadySelected::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* Owner = QueryInstance.Owner.Get();

	if (!IsValid(Owner))
	{
		return;
	}
	// get pawn
	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!IsValid(PawnOwner))
	{
		return;
	}

	// get aicontroller
	// AAIControllerMafia* Controller = Cast<AAIControllerMafia>(PawnOwner->GetController());
	// if (!Controller)
	// {
	// 	return;
	// }
	AAIController* Controller = Cast<AAIController>(PawnOwner->GetController());
	if (!IsValid(Controller))
	{
		return;
	}
	
	//Set last Cover
	UBlackboardComponent* bb = Controller->GetBlackboardComponent();
	AAICoverEntryPoint* lastCover = Cast<AAICoverEntryPoint>(bb->GetValueAsObject(FName("SelectedCover")));
	if (lastCover)
	{
		bb->SetValueAsObject(FName("LastSelectedCover"), lastCover);
	}
	
	BoolValue.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	const bool bwantinside = BoolValue.GetValue();
	//get selected covers
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		if (AActor* Actor = GetItemActor(QueryInstance, It.GetIndex()))
		{
			bool bInside = false;
			// Cast to cover entry point.
			// enter ACoverSplineComponent with owning Spline.
			// check if actor is contained in array of CharactersUsingTheSpline.
			// Filter spline covers already ocupped
			AAICoverEntryPoint* CurrentCover = Cast<AAICoverEntryPoint>(Actor);
			if (CurrentCover)
			{
				AGenericCharacter* Character = Cast<AGenericCharacter>(PawnOwner);
				//if (CurrentCover->OwningSpline->IsCharacterUsingTheSpline(Character)
				if (CurrentCover->GetOwningSpline())
				{
					if (CurrentCover->GetOwningSpline()->IsSplineEmpty()) //si esta ocupado fuera
					{
						bInside = true;
					}
				}
			}
			//covers ocuped are filter out
			It.SetScore(TestPurpose, FilterType, bInside, bwantinside);
		}
	}
}
