#pragma once

#include "CoreMinimal.h"
#include "FEnemyDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FEnemyDataDriven : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxHealth = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AimHandling = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DartStunTime = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CloseCombatSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EnterArenaSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FleeSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PatrolSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MoveToCoverSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MoveBetweenCoverSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MoveAroundCoverSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxNumRafagues = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinNumRafagues = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinShootDistance = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxShootDistance = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinMissingChance = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxMissingChance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AproachPlayerSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OnBeingAttackedSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OnFindPlayerCoveredSpeed = 0.f;
};
