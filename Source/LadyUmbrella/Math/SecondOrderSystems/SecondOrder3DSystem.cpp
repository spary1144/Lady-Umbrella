// Fill out your copyright notice in the Description page of Project Settings.


#include "SecondOrder3DSystem.h"

// Sets default values for this component's properties
USecondOrder3DSystem::USecondOrder3DSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MyConstants = FSecondOrderConstants(Parameters);
}

USecondOrder3DSystem::USecondOrder3DSystem(const float _Frequency, const float _Dampening, const float _InitialResponse)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Parameters.Frequency = _Frequency;
	Parameters.Dampening = _Dampening;
	Parameters.InitialResponse = _InitialResponse;

	MyConstants = FSecondOrderConstants(Parameters);
}


// Called when the game starts
void USecondOrder3DSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USecondOrder3DSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector USecondOrder3DSystem::Update(const FVector& _TargetValue, const float _DeltaTime, const FVector& _InputVelocity)
{
	// Checking that the DeltaTime is not 0.
	if (_DeltaTime <= 0)
	{
		return LastSmoothedValue;
	}

	StoredUnsmoothedTargetValue = _TargetValue;

	float k1_stable, k2_stable;

	// Clamp k2 to guarantee that the system does not create jitter.
	if (MyConstants.w * _DeltaTime < MyConstants.z)
	{
		k1_stable = MyConstants.k1;
		k2_stable = FMath::Max3(
			MyConstants.k2,
			_DeltaTime * _DeltaTime / 2 + _DeltaTime * MyConstants.k1 / 2,
			_DeltaTime * MyConstants.k1);
	}
	else // Use pole matching when the system is very fast.
	{
		float t1 = FMath::Exp(-MyConstants.z * MyConstants.w / _DeltaTime);
		float alpha = 2 * t1 * (MyConstants.z <= 1 ?
				FMath::Cos(_DeltaTime * MyConstants.d)
				: FMath::Cosh(_DeltaTime * MyConstants.d));
		float beta = t1 * t1;
		float t2 = _DeltaTime / (1 + beta - alpha);
		k1_stable = (1 - beta) * t2;
		k2_stable = _DeltaTime * t2;
	}

	LastSmoothedValue = LastSmoothedValue + _DeltaTime * LastSmoothedSpeed;
	// The new speed is calculated after the new smoothed value.
	LastSmoothedSpeed = LastSmoothedSpeed + _DeltaTime *
		(_TargetValue + MyConstants.k3 * _InputVelocity - LastSmoothedValue - k1_stable * LastSmoothedSpeed)
		/ k2_stable;

	return LastSmoothedValue;
}

FVector USecondOrder3DSystem::UpdateWithEstimatedVelocity(const FVector& _TargetValue, const float _DeltaTime)
{
	// Checking that the DeltaTime is not 0.
	if (_DeltaTime <= 0)
	{
		return LastSmoothedValue;
	}

	// We estimate the velocity of the system.
	FVector inputVelocity = (_TargetValue - StoredUnsmoothedTargetValue) / _DeltaTime;

	// Calling the general function.
	return Update(_TargetValue, _DeltaTime, inputVelocity);
}

void USecondOrder3DSystem::ChangeConstants(const float _Frequency, const float _Dampening, const float _InitialResponse)
{
	// Detecting if there was a change in the variables.
	if (Parameters.Frequency != _Frequency
		|| Parameters.Dampening != _Dampening
		|| Parameters.InitialResponse != _InitialResponse)
	{
		Parameters.Frequency = _Frequency;
		Parameters.Dampening = _Dampening;
		Parameters.InitialResponse = _InitialResponse;

		MyConstants = FSecondOrderConstants(Parameters);
	}
}

void USecondOrder3DSystem::ResetSystem()
{
	LastSmoothedValue = StoredUnsmoothedTargetValue;
	LastSmoothedSpeed = FVector::Zero();
}

void USecondOrder3DSystem::ResetSystemToValue(const FVector& _InitValue)
{
	StoredUnsmoothedTargetValue = _InitValue;
	LastSmoothedValue = StoredUnsmoothedTargetValue;
	LastSmoothedSpeed = FVector::Zero();
}

void USecondOrder3DSystem::SetSystemStoredSpeed(const FVector& _NewSpeed)
{
	LastSmoothedSpeed = _NewSpeed;
}

void USecondOrder3DSystem::SetSystemStoredValue(const FVector& _NewValue)
{
	LastSmoothedValue = _NewValue;
}
