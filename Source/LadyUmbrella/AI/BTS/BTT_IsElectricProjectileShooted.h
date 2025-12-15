// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_IsElectricProjectileShooted.generated.h"


class AAgencyCharacter;
class AGenericWeapon;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UBTT_IsElectricProjectileShooted : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY()
	AAgencyCharacter* Character;
	
	UPROPERTY()
	AGenericWeapon* Weapon;
	
	UBTT_IsElectricProjectileShooted();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
