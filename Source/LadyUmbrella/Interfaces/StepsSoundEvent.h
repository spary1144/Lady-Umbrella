// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StepsSoundEvent.generated.h"

class UUpgradeComponent;
UINTERFACE(MinimalAPI)
class UStepsSoundEvent : public UInterface
{
	GENERATED_BODY()
};

class LADYUMBRELLA_API IStepsSoundEvent
{
	GENERATED_BODY()

public:
	virtual void CallStepsSounds(const uint8 StepSurface) = 0;
	
};