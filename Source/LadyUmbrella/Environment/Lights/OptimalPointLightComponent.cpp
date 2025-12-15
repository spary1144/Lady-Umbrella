// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimalPointLightComponent.h"

#include "LadyUmbrella/Util/LoggerUtil.h"

UOptimalPointLightComponent::UOptimalPointLightComponent()
{
	FLogger::NoScreenMessageLog(FString("Constructing ") + GetName() + FString(" (UOptimalPointLightComponent)"));
	SetUseInverseSquaredFalloff( false);
	SetIntensity(60.f);
	MaxDrawDistance = 4000.f;
	MaxDistanceFadeRange = 2000.f;
}
