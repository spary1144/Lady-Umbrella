// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_IsPlayerCloseToCover.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

EBTNodeResult::Type UBTT_IsPlayerCloseToCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller)) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	AActor* CoverSelected = Cast<AActor>(Blackboard->GetValueAsObject(CoverActor.SelectedKeyName));
	if (!IsValid(CoverSelected)) return EBTNodeResult::Failed;

	AActor* PlayerReference = Cast<AActor>(Blackboard->GetValueAsObject(Player.SelectedKeyName));
	if (!IsValid(PlayerReference)) return EBTNodeResult::Failed;
	
	float Distance = FVector::Distance(CoverSelected->GetActorLocation(), PlayerReference->GetActorLocation());
	if (Distance > AccetableRadius)
	{
		//FLogger::DebugLog("Esta el player lejos de la cover");
		return EBTNodeResult::Succeeded;
	}
	//FLogger::DebugLog("Esta el player CERCA de la cover");
	return EBTNodeResult::Failed;
}
