// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimeDilationComponent.generated.h"


UCLASS()
class LADYUMBRELLA_API UTimeDilationComponent : public UActorComponent
{
	GENERATED_BODY()

	UTimeDilationComponent();

	const float INVALID_CURVE_TIME = -1.f;

	UPROPERTY(VisibleAnywhere)
	float CurrentTimeDilation;

	UPROPERTY()
	float CurrentTimeDilationCurveTime;

	float PreviousRealTime;

	UPROPERTY()
	TObjectPtr<const UCurveFloat> CurrentTimeDilationCurve;
	
public:
	UFUNCTION()
	void ActivateTimeDilationCurve(const UCurveFloat* TimeDilationCurve);

private:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};