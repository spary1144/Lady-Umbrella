// 
// BTT_UpdateNextWaypoint.cpp
// 
// Implementation of Task UpdateNextWaypoint: Action executed by a BehaviurTree.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "BTT_UpdateNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/AI/SplinePath.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

EBTNodeResult::Type UBTT_UpdateNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}
	
	AEnemyCharacter* Character = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	ASplinePath* Spline = Character->GetSplinePath();
	if (!IsValid(Spline))
	{
		return EBTNodeResult::Failed;
	}
	
	//Get current index
	int32 currentIndex = Blackboard->GetValueAsInt(TEXT("IndexNextWaypoint"));

	//get spline point
	USplineComponent* SplineComponent = Spline->GetSplineComponent();
	if (!IsValid(SplineComponent))
	{
		return EBTNodeResult::Failed;
	}
	
	FVector location = SplineComponent->GetLocationAtSplinePoint(currentIndex,ESplineCoordinateSpace::World);
	
	//Next waypoint
	++currentIndex;
	//Check if next waypoint is out of max length
	//if (currentIndex >= character->GetPathLength())
	if (currentIndex >= Spline->Spline->GetNumberOfSplinePoints())
	{
		currentIndex = 0;
	}
	//Set index
	Blackboard->SetValueAsInt(TEXT("IndexNextWaypoint"), currentIndex);
	//Set Location
	Blackboard->SetValueAsVector(TEXT("LocationNextWaypoint"), location);
	
	//return success
	return EBTNodeResult::Succeeded;
}
