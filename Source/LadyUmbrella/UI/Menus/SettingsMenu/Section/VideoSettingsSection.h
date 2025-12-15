// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/MenuSection.h"
#include "VideoSettingsSection.generated.h"

class UAnimatedBaseButton;
class UTextBlock;
class APlayerCharacter;
class UInputElement;

UCLASS()
class LADYUMBRELLA_API UVideoSettingsSection : public UMenuSection
{
	GENERATED_BODY()

	// NOTE: This cant be in constructor.
	int8 PCCompatibilityIndex = 2; 
	// bool bSteamDeckCompatibilityMode = false;

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION() FORCEINLINE TArray<FString> GetQualityOptions() { return { "Low", "Middle", "High"}; }
	
	UFUNCTION() FORCEINLINE int8 GetAntialiasingIndex() const { return 1; }
	UFUNCTION() FORCEINLINE int8 GetReflectionIndex() const { return 1; }
	UFUNCTION() FORCEINLINE int8 GetShaderIndex() const { return 1; }
	UFUNCTION() FORCEINLINE int8 GetShadowIndex() const { return 1; }
	UFUNCTION() FORCEINLINE int8 GetTextureIndex() const { return 1; }
	UFUNCTION() FORCEINLINE int8 GetViewDistanceIndex() const { return 1; }
	UFUNCTION() FORCEINLINE int8 GetVisualEffectIndex() const { return 1; }
	
	// UFUNCTION() FORCEINLINE int8 IsSteamDeckCompatibilityMode() const { return bSteamDeckCompatibilityMode; }
	
	UFUNCTION() void UpdateAntialiasingQuality(uint8 Index);
	UFUNCTION() void UpdateReflectionQuality(uint8 Index);
	UFUNCTION() void UpdateShaderQuality(uint8 Index);
	UFUNCTION() void UpdateShadowQuality(uint8 Index);
	// UFUNCTION() void UpdateSteamDeckCompatibilityMode(bool Value);
	UFUNCTION() void UpdateTextureQuality(uint8 Index);
	UFUNCTION() void UpdateViewDistanceQuality(uint8 Index);
	UFUNCTION() void UpdateVisualEffectQuality(uint8 Index);
	UFUNCTION() void UpdateResolution(uint8 Index);

};