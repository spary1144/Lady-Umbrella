// 
// BTT_CheckIfCanFlee.h
// 
// BehaviourTree: Task to modify agent velocity
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangeMovementSpeed.generated.h"

class APawn;

UENUM(BlueprintType)
enum class EBehaviorStatus : uint8
{
	Flee,
	CloseCombat,
	EnterArena,
	Patrol,
	MoveToCover,
	MoveBetweenCover,
	MoveAroundCover,
	AproachPlayer,
	AttackedByPlayer,
	FindPlayerCovered
};
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_ChangeMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()

	EBTNodeResult::Type SetSpeedMafia() const;
	EBTNodeResult::Type SetSpeedRegularAgency() const;
	EBTNodeResult::Type SetSpeedShieldAgency() const;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// UPROPERTY(EditAnywhere)
	// float NewSpeed;
	UPROPERTY()
	APawn* EnemyPawn;
	
	UPROPERTY(EditAnywhere)
	EBehaviorStatus Behaviour;
};
