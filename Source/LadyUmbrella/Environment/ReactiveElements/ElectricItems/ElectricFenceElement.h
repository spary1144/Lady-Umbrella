// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuddleElement.h"
#include "ElectricFenceElement.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AElectricFenceElement : public AElectricElement
{
	GENERATED_BODY()
public:
	AElectricFenceElement();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PerformElectrification() override;
	virtual void ResetElectrifiedState() override;
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
