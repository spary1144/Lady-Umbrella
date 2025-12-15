// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimalSpotLightComponent.h"

#include "LadyUmbrella/Util/LoggerUtil.h"

UOptimalSpotLightComponent::UOptimalSpotLightComponent()
{
	FLogger::NoScreenMessageLog(FString("Constructing ") + GetName() + FString(" (UOptimalSpotLightComponent)"));
	MaxDrawDistance = 7000.f;
	MaxDistanceFadeRange = 4000.f;
	SetUseInverseSquaredFalloff( false);
}
