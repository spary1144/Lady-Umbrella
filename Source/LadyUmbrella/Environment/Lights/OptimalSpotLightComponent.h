// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h"
#include "OptimalSpotLightComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LADYUMBRELLA_API UOptimalSpotLightComponent : public USpotLightComponent
{
	GENERATED_BODY()

	UOptimalSpotLightComponent();
};