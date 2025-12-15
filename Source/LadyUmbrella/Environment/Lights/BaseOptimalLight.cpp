// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseOptimalLight.h"

#include "Components/LightComponent.h"

// Sets default values
ABaseOptimalLight::ABaseOptimalLight() :
	bStartWithFlickeringEnabled { false },
	LightIntensityChangeSpeed { 8.0f },
	MinFlickeringIntensityPercentage { 0.0f },
	MaxFlickeringIntensityPercentage { 0.8f },
	MinWaitBetweenFlickersTime { 0.3f },
	MaxWaitBetweenFlickersTime { 0.5f },
	bFlickeringEnabled { false },
	OriginalLightIntensity { 0.0f },
	CurrentWaitBetweenFlickersTime { 0.0f },
	CurrentTimeWaitedBetweenFlickersTime { 0.0f },
	CurrentFlickeringIntensityPercentage { 0.0f },
	CurrentTargetFlickeringIntensityPercentage { 0.0f },
	FlickeringState { EFlickeringState::WaitingForFlickering }
{
 	// Lights might need to tick if they Flicker.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(false);
}

void ABaseOptimalLight::BeginPlay()
{
	Super::BeginPlay();

	ULightComponent* LightComponent = GetLightComponent();
	if (IsValid(LightComponent))
	{
		OriginalLightIntensity = LightComponent->Intensity;
	}

	SetFlickeringEnabled(bStartWithFlickeringEnabled);
}

void ABaseOptimalLight::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (!bFlickeringEnabled)
	{
		return;
	}

	switch (FlickeringState)
	{
	case EFlickeringState::WaitingForFlickering:
		WaitingForFlickeringTickFunctionality(DeltaTime);
		break;
		
	case EFlickeringState::EnteringFlicker:
		EnteringFlickerTickFunctionality(DeltaTime);
		break;
		
	case EFlickeringState::ExitingFlicker:
		ExitingFlickerTickFunctionality(DeltaTime);
		break;
	}
}

void ABaseOptimalLight::SetLightIntensity(const float NewIntensity)
{
	ULightComponent* LightComponent = GetLightComponent();
	if (!IsValid(LightComponent))
	{
		return;
	}

	LightComponent->SetIntensity(NewIntensity);
}

void ABaseOptimalLight::WaitingForFlickeringTickFunctionality(const float DeltaTime)
{
	CurrentTimeWaitedBetweenFlickersTime += DeltaTime;

	// If we Waited long enough.
	if (CurrentTimeWaitedBetweenFlickersTime >= CurrentWaitBetweenFlickersTime)
	{
		FlickeringState = EFlickeringState::EnteringFlicker;
		CurrentTargetFlickeringIntensityPercentage = FMath::RandRange(MinFlickeringIntensityPercentage, MaxFlickeringIntensityPercentage);
	}
}

void ABaseOptimalLight::EnteringFlickerTickFunctionality(const float DeltaTime)
{
	CurrentFlickeringIntensityPercentage -= LightIntensityChangeSpeed * DeltaTime;
	CurrentFlickeringIntensityPercentage = FMath::Clamp(CurrentFlickeringIntensityPercentage, CurrentTargetFlickeringIntensityPercentage, 1.0f);
	SetLightIntensity(CurrentFlickeringIntensityPercentage * OriginalLightIntensity);
	
	// If we are at our lowest intensity.
	if (CurrentFlickeringIntensityPercentage <= CurrentTargetFlickeringIntensityPercentage)
	{
		FlickeringState = EFlickeringState::ExitingFlicker;
	}
}

void ABaseOptimalLight::ExitingFlickerTickFunctionality(const float DeltaTime)
{
	CurrentFlickeringIntensityPercentage += LightIntensityChangeSpeed * DeltaTime;
	CurrentFlickeringIntensityPercentage = FMath::Clamp(CurrentFlickeringIntensityPercentage, CurrentTargetFlickeringIntensityPercentage, 1.f);
	SetLightIntensity(OriginalLightIntensity * CurrentFlickeringIntensityPercentage);
	
	// If we are back at our regular intensity.
	if (CurrentFlickeringIntensityPercentage >= 1.0f)
	{
		FlickeringState = EFlickeringState::WaitingForFlickering;
		CurrentTimeWaitedBetweenFlickersTime = 0.0f;
		CurrentWaitBetweenFlickersTime = FMath::RandRange(MinWaitBetweenFlickersTime, MaxWaitBetweenFlickersTime);
	}
}

void ABaseOptimalLight::SetFlickeringEnabled(const bool bIsEnabled)
{
	PrimaryActorTick.SetTickFunctionEnable(bIsEnabled);
	bFlickeringEnabled = bIsEnabled;

	if (bFlickeringEnabled)
	{
		FlickeringState = EFlickeringState::WaitingForFlickering;
		CurrentTimeWaitedBetweenFlickersTime = 0.0f;
		CurrentWaitBetweenFlickersTime = FMath::RandRange(MinWaitBetweenFlickersTime, MaxWaitBetweenFlickersTime);

		ULightComponent* LightComponent = GetLightComponent();
		if (IsValid(LightComponent))
		{
			OriginalLightIntensity = LightComponent->Intensity;
		}
	}
	else
	{
		// Resetting our Original Light intensity.
		SetLightIntensity(OriginalLightIntensity);
	}
}

bool ABaseOptimalLight::IsFlickeringEnabled() const
{
	return bFlickeringEnabled;
}
