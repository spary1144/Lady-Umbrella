// 
// BTT_IsPlayerInRadius.cpp
// 
// Task executed by Behaviour Tree : IsPlayerInRadius
// 
// Copyright Zulo Interactive. All Rights Reserved.
//



#include "BTT_IsPlayerInRadius.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

EBTNodeResult::Type UBTT_IsPlayerInRadius::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerGeneric* Character = Cast<AAIControllerGeneric>(OwnerComp.GetAIOwner());
	if (!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	//get bb
	UBlackboardComponent* BB = Character->GetBlackboardComponent();
	if (!IsValid(BB))
	{
		return EBTNodeResult::Failed;
	}
	
	//get AI location
	FVector ControlledPLocation = Character->GetPawn()->GetActorLocation();
	//get player location
	AActor* Player = Cast<AActor>(BB->GetValueAsObject(PlayerLocation.SelectedKeyName));
	
	if (!IsValid(Player))
	{
		return EBTNodeResult::Failed;
	}
	
	FVector PLocation = Player->GetActorLocation();
	//get distance
	float Distance = FVector::Distance(ControlledPLocation, PLocation);
	//FLogger::DebugLog("Distance: %f and radius: %f", distance, bb->GetValueAsFloat(AccetableRadius.SelectedKeyName));
	if (IsGreater)
	{
		// if (FMath::Abs(ControlledPLocation.Z - PLocation.Z) > BB->GetValueAsFloat(AccetableRadius.SelectedKeyName))
		// {
		// 	return EBTNodeResult::Failed;
		// }
		
		if (Distance > BB->GetValueAsFloat(AccetableRadius.SelectedKeyName))
		{
			if (bReturnFailIfConditionMet)
			{
				return EBTNodeResult::Failed;
			}
			return EBTNodeResult::Succeeded;
		}
	}
	else
	{
		float MinHeightOffset = 50;
		if (FMath::Abs(ControlledPLocation.Z - PLocation.Z) > MinHeightOffset)
		{
			return EBTNodeResult::Failed;
		}
		
		if (Distance <= BB->GetValueAsFloat(AccetableRadius.SelectedKeyName))
		{
			if (bReturnFailIfConditionMet)
			{
				return EBTNodeResult::Failed;
			}
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
