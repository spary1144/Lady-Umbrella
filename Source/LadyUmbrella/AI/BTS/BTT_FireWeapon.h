// 
// BTT_HasLineOfSightToPlayer.h
// 
// BehaviourTree: Task to notify one agent that begin firing
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FireWeapon.generated.h"

/**
 * 
 */
class AEnemyCharacter;
class AGenericWeapon;

UCLASS()
class LADYUMBRELLA_API UBTT_FireWeapon : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bShootUntilRunOutAllBullets = true;

	UPROPERTY()
	AEnemyCharacter* EnemyCharacter;

	UPROPERTY()
	UBlackboardComponent* Blackboard;

	UPROPERTY()
	AGenericWeapon* GenericWeapon;

	UPROPERTY()
	float WeaponCadence;
	
	UBTT_FireWeapon();
	// bool HandleFire();
	bool IsCharacterReadyToFire();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// void Fire(UBehaviorTreeComponent& OwnerComp);
};
