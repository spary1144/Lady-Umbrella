// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AimFromCover.h"

#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_AimFromCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	AGenericCharacter* GenericCharacter = Cast<AGenericCharacter>(Controller->GetPawn());
	if (!IsValid(GenericCharacter)) return EBTNodeResult::Failed;

	GenericCharacter->UpdateAimState(bIsAiming);

	return EBTNodeResult::Succeeded;
}
