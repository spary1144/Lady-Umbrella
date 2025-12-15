// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/MenuSection.h"
#include "ControlSettings.generated.h"

class UAnimatedBaseButton;
class UTextBlock;
class APlayerCharacter;
class UInputElement;

UCLASS()
class LADYUMBRELLA_API UControlSettings : public UMenuSection
{
	GENERATED_BODY()

public:
	UFUNCTION() float GetSensibilityHorizontalStick();
	UFUNCTION() float GetSensibilityVerticalStick();
	UFUNCTION() float GetSensibilityHorizontalKBM();
	UFUNCTION() float GetSensibilityVerticalKBM();
	
	UFUNCTION() FORCEINLINE TArray<FString> GetPlatforms() { return {"PLAYSTATION", "XBOX"}; }
	UFUNCTION() FORCEINLINE int8 GetCurrentPlatformIndex() { return 0; }
	
	UFUNCTION() void UpdateSensibilityVerticalKBM(float Value);
	UFUNCTION() void UpdateSensibilityHorizontalKBM(float Value);
	UFUNCTION() void UpdateSensibilityVerticalStick(float Value);
	UFUNCTION() void UpdateSensibilityHorizontalStick(float Value);
	UFUNCTION() void UpdatePlatformName(uint8 Index);

	UFUNCTION() void UpdateUmbrella(bool bActivate);
	
protected:
	
	UFUNCTION() float GetProcessedSensibility(EPlatform Platform, bool bOX);
	UFUNCTION() void SetSensibility(float Percent, bool bHorizontal, bool bStick);
	UFUNCTION() TArray<FString> GetLanguages();
	UFUNCTION() void UpdateLanguage(uint8 Index) const;
};