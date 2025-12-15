// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerGeneric.h"
#include "AIControllerShieldAgency.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AAIControllerShieldAgency : public AAIControllerGeneric
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float RadiusStop;
	

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitializeController(AArenaManager* ArenaManager,const AAISpawnPoint& SpawnPoint) override;
public:
	UFUNCTION()
	void NotifyShieldIsBroken() const;
};
