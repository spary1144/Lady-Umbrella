// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SecondOrderStructs.h"
#include "SecondOrder1DSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API USecondOrder1DSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USecondOrder1DSystem();

	/**
 	* Creates a Second Order 1D System using the provided parameters.
 	* @param _Frequency The responsiveness of the system. Higher values make the system catch up with the desired value faster.
 	* @param _Dampening The amount of dampening applied to the system. A value >= 1 will create steady transitions to the desired value. A value in the (0, 1) range will result in a vibrating system. A value of 0 not dampen the system (not recommended).
 	* @param _InitialResponse  The initial velocity of the system when a change in desired value is introduced. Values over 1 will make the system overshoot its target.
 	*/
	USecondOrder1DSystem(const float _Frequency, const float _Dampening, const float _InitialResponse);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Updates the SecondOrder System using the desired value as guide.
	 * @param _TargetValue The desired value of the system. Must not be NaN.
	 * @param _DeltaTime The time passed since the last time that the system was updated. Must be bigger than 0.
	 * @param _InputVelocity The Speed of the system at the time of the call. Might be useful for systems where the speed cannot be deduced with accuracy.
	 */
	UFUNCTION(BlueprintCallable)
	float Update(const float _TargetValue, const float _DeltaTime, const float _InputVelocity);

	/**
 	* Updates the SecondOrder System using the desired value as guide. Also estimates the velocity of the system.
 	* @param _TargetValue The desired value of the system. Must not be NaN.
 	* @param _DeltaTime The time passed since the last time that the system was updated. Must be bigger than 0.
 	*/
	UFUNCTION(BlueprintCallable)
	float UpdateWithEstimatedVelocity(const float _TargetValue, const float _DeltaTime);

	/**
	 * Updates the parameters that determine the behavior of this Second Order System.
	 * @param _Frequency The responsiveness of the system. Higher values make the system catch up with the desired value faster.
	 * @param _Dampening The amount of dampening applied to the system. A value >= 1 will create steady transitions to the desired value. A value in the (0, 1) range will result in a vibrating system. A value of 0 not dampen the system (not recommended).
	 * @param _InitialResponse  The initial velocity of the system when a change in desired value is introduced. Values over 1 will make the system overshoot its target.
	 */
	UFUNCTION(BlueprintCallable)
	void ChangeConstants(const float _Frequency, const float _Dampening, const float _InitialResponse);

	
	/**
	 * Resets the speed of the system to 0 and the LastSmoothedValue to the LastDesiredValue.
	 */
	UFUNCTION(BlueprintCallable)
	void ResetSystem();

	UFUNCTION(BlueprintCallable)
	void ResetSystemToValue(const float _InitValue);

	UFUNCTION(BlueprintCallable)
	void SetSystemStoredSpeed(const float _NewSpeed);

	UFUNCTION(BlueprintCallable)
	void SetSystemStoredValue(const float _NewValue);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetSystemStoredSpeed() const { return LastSmoothedSpeed; };
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetSystemStoredValue() const { return LastSmoothedValue; };

private:
	UPROPERTY()
	float StoredUnsmoothedTargetValue = 0;
	UPROPERTY()
	float LastSmoothedValue = 0;
	UPROPERTY()
	float LastSmoothedSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly)
	FSecondOrderParams Parameters;

	UPROPERTY()
	FSecondOrderConstants MyConstants;
};
