// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SecondOrderStructs.h"
#include "SecondOrderFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API USecondOrderFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	/**
	* Generates a SecondOrderConstants Struct with the given parameters.
	* @param _Frequency The responsiveness of the system. Higher values make the system catch up with the desired value faster.
	* @param _Dampening The amount of dampening applied to the system. A value >= 1 will create steady transitions to the desired value. A value in the (0, 1) range will result in a vibrating system. A value of 0 not dampen the system (not recommended).
	* @param _InitialResponse The initial velocity of the system when a change in desired value is introduced. Values over 1 will make the system overshoot its target.
	*/
	UFUNCTION(BlueprintCallable)
	static FSecondOrderConstants CreateSecondOrderConstants(const float _Frequency, const float _Dampening, const float _InitialResponse);
};
