// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetFocusTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_SetFocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//return Super::ExecuteTask(OwnerComp, NodeMemory);
	UBlackboardComponent* bb = OwnerComp.GetBlackboardComponent();
	
	//AActor* QueryOwner = OwnerComp.GetOwner();
	AAIController* MyController = Cast<AAIController>(OwnerComp.GetOwner());
	if (MyController)
	{
		const AActor* targetActor = Cast<AActor>(bb->GetValueAsObject(target.SelectedKeyName));

		if (!IsValid(targetActor)) return EBTNodeResult::Failed;
		
		MyController->SetFocalPoint(targetActor->GetActorLocation(),EAIFocusPriority::Move);
		MyController->UpdateControlRotation(GetWorld()->DeltaTimeSeconds,true);
	}
	return EBTNodeResult::Succeeded;
}
