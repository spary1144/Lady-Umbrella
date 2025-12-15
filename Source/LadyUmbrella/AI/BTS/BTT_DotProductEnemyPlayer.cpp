// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DotProductEnemyPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

UBTT_DotProductEnemyPlayer::UBTT_DotProductEnemyPlayer()
{
	MinDotValue = -0.5;
	MaxDotValue = 0.5;
}

EBTNodeResult::Type UBTT_DotProductEnemyPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	AActor* PlayerCharacter = Cast<AActor>(Blackboard->GetValueAsObject(PlayerKey.SelectedKeyName));
	APawn* EnemyCharacter = Controller->GetPawn();

	if (!IsValid(PlayerCharacter) || !IsValid(EnemyCharacter))
	{
		return EBTNodeResult::Failed;
	}

	FVector EnemytoPlayer = (EnemyCharacter->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
	FVector PlayerFordward = PlayerCharacter->GetActorForwardVector();

	float DotValue = FVector::DotProduct(EnemytoPlayer, PlayerFordward);
	//FLogger::DebugLog("DotProductValue: %f", DotValue);
	if (DotValue >= MinDotValue && DotValue <= MaxDotValue)
	{
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
