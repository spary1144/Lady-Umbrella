// Fill out your copyright notice in the Description page of Project Settings.


#include "OptimalRectLightComponent.h"

#include "LadyUmbrella/Util/LoggerUtil.h"

UOptimalRectLightComponent::UOptimalRectLightComponent()
{
	FLogger::NoScreenMessageLog(FString("Constructing ") + GetName() + FString(" (UOptimalRectLightComponent)"));
	MaxDrawDistance = 5000.f;
	MaxDistanceFadeRange = 4000.f;
}
