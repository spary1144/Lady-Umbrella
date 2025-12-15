// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FPlayerDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FPlayerDataDriven : public FTableRowBase
{
	GENERATED_BODY()
	/** Max Health the player can have*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxHealth = 0;

	/** Time without getting hit the player has to be to start Health Regen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegenerationCoolDown = 0;

	/** Time without getting hit the player has to be to start Health Regen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegenerationValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RegenerationSpeed = 0;

	/** Amount of health is added when regenerating health per second */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 HealthRegenerateFactor = 0;

	/** Walk speed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WalkSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StrafeAimingSpeed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StrafeShieldingSpeed = 0.f;

	/** Speed while in cover */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OnCoverSpeed = 0.f;

	/** Walking Speed while talking with communicator */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OnTalkingSpeed = 0.f;

	/** MaxJumpHeight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxJumpHeight = 0.f;
	
	/** MaxJumpDistance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxJumpDistance = 0.f;

	/** Air control...? Ask design */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AirControl = 0.f;

	/** KickStaggerTime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float KickStaggerTime = 0.f;
	
	/** Time you get stunned when hit with the Electric Dart */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DartStunTime = 0.f;
	
};
