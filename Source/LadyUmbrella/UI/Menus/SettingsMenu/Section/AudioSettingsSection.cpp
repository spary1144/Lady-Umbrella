// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioSettingsSection.h"

#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"

void UAudioSettingsSection::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAudioSettingsSection::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SFXBuffer = LoadObject<USoundClass>(nullptr, TEXT("/Script/Engine.SoundClass'/Engine/EngineSounds/SFX.SFX'"));
}

float UAudioSettingsSection::GetMaster()
{
	return FmodUtils::GetBusCurrentVolume(FmodUtils::BusNames[0]);
}

float UAudioSettingsSection::GetVoices()
{
	return FmodUtils::GetBusCurrentVolume(FmodUtils::BusNames[1]);
}	

float UAudioSettingsSection::GetSFX()
{
	return FmodUtils::GetBusCurrentVolume(FmodUtils::BusNames[2]);
}

float UAudioSettingsSection::GetMusic()
{
	return FmodUtils::GetBusCurrentVolume(FmodUtils::BusNames[3]);
}

void UAudioSettingsSection::UpdateMaster(float Value)
{
	FmodUtils::SetBusVolume(FmodUtils::BusNames[0], Value);
}

void UAudioSettingsSection::UpdateVoices(float Value)
{
	FmodUtils::SetBusVolume(FmodUtils::BusNames[1], Value);
}

void UAudioSettingsSection::UpdateSFX(float Value)
{
	FmodUtils::SetBusVolume(FmodUtils::BusNames[2], Value);
	if(IsValid(SFXBuffer))
	{
		SFXBuffer->Properties.Volume = Value;
	}
}

void UAudioSettingsSection::UpdateMusic(float Value)
{
	FmodUtils::SetBusVolume(FmodUtils::BusNames[3], Value);
}