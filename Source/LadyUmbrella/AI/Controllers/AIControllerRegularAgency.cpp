// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerRegularAgency.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"

void AAIControllerRegularAgency::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//biter data
	Blackboard->SetValueAsFloat(TEXT("CloseRadius"),CloseRadius);
	Blackboard->SetValueAsFloat(TEXT("AdvanceRadius"),AdvanceRadius);
	//flanker data
	Blackboard->SetValueAsEnum(FName("StatesFlank"), SearchFlankCover);
	Blackboard->SetValueAsFloat(TEXT("ShootRadius"),ShootRadius);
	
}

void AAIControllerRegularAgency::OnUnPossess()
{
	if (!IsValid(Blackboard))
	{
		return;
	}
	
	if (Blackboard->GetValueAsBool(TEXT("IsFlanker")) && IsValid(CurrentArenaManager))
	{
		CurrentArenaManager->RemoveSpecialistEnemies();
	}
	
	Super::OnUnPossess();
}

void AAIControllerRegularAgency::InitializeController(AArenaManager* ArenaManager,
                                                      const AAISpawnPoint& SpawnPoint)
{
	if (!IsValid(ArenaManager))
	{
		return;
	}
	Super::InitializeController(ArenaManager, SpawnPoint);

	//check if is flanker
	if (SpawnPoint.bIsFlanker)
	{
		ArenaManager->AddSpecialistEnemies();
		Blackboard->SetValueAsBool(BlackboardKeyNameToKnowIfIsFlanker, true);
	}

	//set zone
	Blackboard->SetValueAsInt(BlackboardKeyNameToKnowCurrentZone, ArenaManager->GetHighestZone());

	//check if is reinforcement
	if (SpawnPoint.bIsReinforcement || SpawnPoint.bEnterArenaThroughDoor || SpawnPoint.bEnterArenaThroughWindow)
	{
		Blackboard->SetValueAsBool(BlackboardKeyNameToKnowIfIsReinforcement,true);
		Blackboard->SetValueAsBool(BlackboardKeyNameRushPlayer,true); 
	}
}

void AAIControllerRegularAgency::SetZone(int Newzone)
{
	Super::SetZone(Newzone);
}

void AAIControllerRegularAgency::AddCoverAsSelected(AAICoverEntryPoint* cover)
{
	CoversSelected.Add(cover);
}
