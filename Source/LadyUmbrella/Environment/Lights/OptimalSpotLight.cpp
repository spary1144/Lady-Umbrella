// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimalSpotLight.h"

#include "Components/SpotLightComponent.h"

AOptimalSpotLight::AOptimalSpotLight()
{
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	SetRootComponent(SpotLightComponent);
	SpotLightComponent->MaxDrawDistance = 7000.f;
	SpotLightComponent->MaxDistanceFadeRange = 4000.f;
	SpotLightComponent->SetUseInverseSquaredFalloff( false);
}

ULightComponent* AOptimalSpotLight::GetLightComponent()
{
	return SpotLightComponent;
}
