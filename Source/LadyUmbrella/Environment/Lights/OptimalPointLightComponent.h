// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PointLightComponent.h"
#include "OptimalPointLightComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LADYUMBRELLA_API UOptimalPointLightComponent : public UPointLightComponent
{
	GENERATED_BODY()

	UOptimalPointLightComponent();
};