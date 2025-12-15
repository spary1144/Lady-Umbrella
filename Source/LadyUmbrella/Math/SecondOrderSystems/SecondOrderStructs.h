// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SecondOrderStructs.generated.h"

struct FSecondOrderParams;

/**
 * Used within the SecondOrder Components to store the constants that determine a specific movement.
 */
USTRUCT(BlueprintType)
struct FSecondOrderConstants
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float w = 0;
	UPROPERTY(VisibleAnywhere)
	float z = 0;
	UPROPERTY(VisibleAnywhere)
	float d = 0;
	UPROPERTY(VisibleAnywhere)
	float k1 = 0;
	UPROPERTY(VisibleAnywhere)
	float k2 = 0;
	UPROPERTY(VisibleAnywhere)
	float k3 = 0;

	FSecondOrderConstants() = default;
	FSecondOrderConstants(const float _Frequency, const float _Dampening, const float _InitialResponse);
	FSecondOrderConstants(const FSecondOrderParams& _Parameters);
};

/**
 * The parameters that determine how a Second Order System behaves.
 */
USTRUCT(BlueprintType)
struct FSecondOrderParams
{
	GENERATED_BODY()

	// The responsiveness of the system.
	// Higher values make the system catch up with the desired value faster.
	UPROPERTY(EditAnywhere)
	float Frequency = 1;
	
	// The amount of dampening applied to the system.
	// A value >= 1 will create steady transitions to the desired value.
	// A value in the (0, 1) range will result in a vibrating system.
	// A value of 0 not dampen the system (not recommended).
	UPROPERTY(EditAnywhere)
	float Dampening = 1;
	
	// The initial velocity of the system when a change in desired value is introduced.
	// Values over 1 will make the system overshoot its target.
	UPROPERTY(EditAnywhere)
	float InitialResponse = 0;

	FSecondOrderParams() = default;
	FSecondOrderParams(float _Frequency, float _Dampening, float _InitialResponse);
};