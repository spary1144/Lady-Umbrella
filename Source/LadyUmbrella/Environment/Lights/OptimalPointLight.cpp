// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimalPointLight.h"

#include "Components/PointLightComponent.h"

AOptimalPointLight::AOptimalPointLight()
{
	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	SetRootComponent(PointLightComponent);
	PointLightComponent->SetUseInverseSquaredFalloff( false);
	PointLightComponent->SetIntensity(60.f);
	PointLightComponent->MaxDrawDistance = 4000.f;
	PointLightComponent->MaxDistanceFadeRange = 2000.f;
}

ULightComponent* AOptimalPointLight::GetLightComponent()
{
	return PointLightComponent;
}
