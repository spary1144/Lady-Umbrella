// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimalRectLight.h"

#include "Components/RectLightComponent.h"

AOptimalRectLight::AOptimalRectLight()
{
	RectLightComponent = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLightComponent"));
	SetRootComponent(RectLightComponent);
	RectLightComponent->MaxDrawDistance = 5000.f;
	RectLightComponent->MaxDistanceFadeRange = 4000.f;
}

ULightComponent* AOptimalRectLight::GetLightComponent()
{
	return RectLightComponent;
}
