// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricElement.h"
#include "ElectricPlatform.generated.h"


UCLASS()
class LADYUMBRELLA_API AElectricPlatform : public AElectricElement
{
	GENERATED_BODY()

public:
	AElectricPlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PerformElectrification() override;
	virtual void ResetElectrifiedState() override;

protected:
	FVector StartLocation;
	UPROPERTY(EditAnywhere, Category = "Platform Movement", meta = (MakeEditWidget = true))
	FVector EndLocation;
	UPROPERTY(EditAnywhere, Category = "Platform Movement")
	float MinTargetThreshold;
	UPROPERTY(EditAnywhere, Category = "Platform Movement")
	float MoveSpeed;

	bool bIsMoving = false;

	FVector TargetLocation;
};
