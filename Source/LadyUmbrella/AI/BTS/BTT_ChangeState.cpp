// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChangeState.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"

EBTNodeResult::Type UBTT_ChangeState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIControllerMafia* Controller = Cast<AAIControllerMafia>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}
	// Cambiar el estado en el Blackboard
	Blackboard->SetValueAsEnum(EnumState.SelectedKeyName, NewState);
	if (NewState == EStates::Attack)
	{
		Controller->DetectPlayer(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0),false);
	}

	return EBTNodeResult::Succeeded;
}
