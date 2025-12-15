// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RotateToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_RotateToTarget::UBTT_RotateToTarget()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	RotationSpeed = 1.f;
}

EBTNodeResult::Type UBTT_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!IsValid(AICon)) 
	{
		return EBTNodeResult::Failed;
	}
	
	EnemyCharacter = AICon->GetPawn();
	if (!IsValid(EnemyCharacter))
	{
		return EBTNodeResult::Failed;
	}

	Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Actor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(Actor))
	{
		Target = Blackboard->GetValueAsVector(TargetKey.SelectedKeyName);
	}
	else
	{
		Target = Actor->GetActorLocation();
	}
	
	return EBTNodeResult::InProgress;
}

void UBTT_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!IsValid(EnemyCharacter))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	
	FVector ToTarget = (Target - EnemyCharacter->GetActorLocation()).GetSafeNormal2D();
	FRotator CurrentRot = EnemyCharacter->GetActorRotation();
	FRotator TargetRot = ToTarget.Rotation();

	// Interp con velocidad angular
	FRotator NewRot = FMath::RInterpConstantTo(CurrentRot, TargetRot, DeltaSeconds, RotationSpeed);

	EnemyCharacter->SetActorRotation(NewRot);

	float AngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw));
	if (AngleDiff < MIN_ANGLE_DIFFERENCE)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
