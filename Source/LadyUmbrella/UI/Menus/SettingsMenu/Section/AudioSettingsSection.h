// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/MenuSection.h"
#include "AudioSettingsSection.generated.h"

UCLASS()
class LADYUMBRELLA_API UAudioSettingsSection : public UMenuSection
{
	GENERATED_BODY()

	UPROPERTY()
	USoundClass* SFXBuffer;

public:
	
	UFUNCTION() void UpdateMaster(float Value);
	UFUNCTION() float GetMaster();
	
	UFUNCTION() void UpdateSFX(float Value);
	UFUNCTION() float GetSFX();
	
	UFUNCTION() void UpdateVoices(float Value);
	UFUNCTION() float GetVoices();
	
	UFUNCTION() void UpdateMusic(float Value);
	UFUNCTION() float GetMusic();

protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	
};