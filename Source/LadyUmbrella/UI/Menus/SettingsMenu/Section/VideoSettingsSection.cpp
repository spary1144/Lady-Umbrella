// Fill out your copyright notice in the Description page of Project Settings.

#include "VideoSettingsSection.h"

#include "GameFramework/GameUserSettings.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

void UVideoSettingsSection::NativeConstruct()
{
	Super::NativeConstruct();
}

void UVideoSettingsSection::UpdateAntialiasingQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetAntiAliasingQuality(Index);
		Settings->ApplySettings(false);
	}
}

void UVideoSettingsSection::UpdateReflectionQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetReflectionQuality(Index);
		Settings->ApplySettings(false);
	}
}

void UVideoSettingsSection::UpdateShaderQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetShadingQuality(Index);
		Settings->ApplySettings(false);
	}
}

void UVideoSettingsSection::UpdateShadowQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetShadingQuality(Index);
		Settings->ApplySettings(false);
	}
}

/*
void UVideoSettingsSection::UpdateSteamDeckCompatibilityMode(bool Value)
{
	bSteamDeckCompatibilityMode = Value;

	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		if (bSteamDeckCompatibilityMode)
		{
			Settings->SetOverallScalabilityLevel(0);
		}
		else
		{
			Settings->SetOverallScalabilityLevel(PCCompatibilityIndex);
		}

		Settings->ApplySettings(false);
	}
}
*/

void UVideoSettingsSection::UpdateTextureQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetTextureQuality(Index);
		Settings->ApplySettings(false);
	}
}

void UVideoSettingsSection::UpdateViewDistanceQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetViewDistanceQuality(Index);
		Settings->ApplySettings(false);
	}
}

void UVideoSettingsSection::UpdateVisualEffectQuality(uint8 Index)
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();

	if (Settings)
	{
		Settings->SetVisualEffectQuality(Index);
		Settings->ApplySettings(false);
	}
}

void UVideoSettingsSection::UpdateResolution(uint8 Index)
{
	FLogger::InfoLog("[TODO] Update Resolution Delegate");
}
