// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlSettings.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/SliderInput.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

float UControlSettings::GetProcessedSensibility(EPlatform Platform, bool bOX)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("No FGlobalPointers::PlayerCharacter on UControlSettings.GetProcessedSensibility"));
		return 0;
	}
	if (!IsValid(PlayerCharacter) || !IsValid(PlayerCharacter->GetCameraSystemComponent()))
	{
		UE_LOG(LogTemp, Error, TEXT("No CameraSystemComponent on UControlSettings.GetProcessedSensibility"));
		return 0;
	}
	
	FVector2D Range = PlayerCharacter->GetCameraSystemComponent()->GetSensibilityRange(FWidgetAssets::PlatformsKeys[Platform]);
	float Sensibility = PlayerCharacter->GetCameraSystemComponent()->GetCurrentSensibility(FWidgetAssets::PlatformsKeys[Platform], bOX);
	return FMath::Clamp((Sensibility - Range.X) / (Range.Y - Range.X), FWidgetAssets::MINIMUM_SLIDER, FWidgetAssets::MAXIMUM_SLIDER);
}

float UControlSettings::GetSensibilityHorizontalStick()
{
	return GetProcessedSensibility(EPlatform::XBOX, true);
}

float UControlSettings::GetSensibilityVerticalStick()
{
	return GetProcessedSensibility(EPlatform::XBOX, false);
}

float UControlSettings::GetSensibilityHorizontalKBM()
{
	return GetProcessedSensibility(EPlatform::PC, true);
}

float UControlSettings::GetSensibilityVerticalKBM()
{
	return GetProcessedSensibility(EPlatform::PC, false);
}

void UControlSettings::UpdateSensibilityVerticalKBM(float Value)
{
	SetSensibility(Value, false, false);
}

void UControlSettings::UpdateSensibilityHorizontalKBM(float Value)
{
	SetSensibility(Value, true, false);
}

void UControlSettings::UpdateSensibilityVerticalStick(float Value)
{
	SetSensibility(Value, false, true);
}

void UControlSettings::UpdateSensibilityHorizontalStick(float Value)
{
	SetSensibility(Value, true, true);
}

void UControlSettings::UpdateUmbrella(bool bActivate)
{
	if (bActivate)
	{
		FLogger::DebugLog("Activate UpdateUmbrella");
		return;
	}
	FLogger::DebugLog("Deactivate UpdateUmbrella");
}

void UControlSettings::UpdatePlatformName(uint8 Index)
{
    if(!IsValid(FGlobalPointers::MainController))
    {
    	return;
    }
	EPlatform CurrPlatform;
	if (FGlobalPointers::MainController->GetInputDevice().IsEmpty())
	{
		CurrPlatform = EPlatform::PC;
	}
	else
	{
		CurrPlatform = *FWidgetAssets::PlatformsKeys.FindKey(*FGlobalPointers::MainController->GetInputDevice());
	}

	if (CurrPlatform == EPlatform::PC)
	{
		return;
	}

	if (ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		if (Index > 0)
		{
			GameInstance->SetPlatformName(EPlatform::XBOX);
		}
		else
		{
			GameInstance->SetPlatformName(EPlatform::PS);
		}
	}
}

void UControlSettings::SetSensibility(float Percent, bool bHorizontal, bool bStick)
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		UE_LOG(LogTemp, Error, TEXT("No CameraSystemComponent on UControlSettings.SetSensibility"));
		return;
	}
	float UpdatedValue = 0;
	FVector2D Range = FVector2D::ZeroVector;
	EPlatform Platform = bStick ? EPlatform::XBOX : EPlatform::PC;

	Range = FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->GetSensibilityRange(FWidgetAssets::PlatformsKeys[Platform]);
	UpdatedValue = Percent * (Range.Y - Range.X) + Range.X;
	FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->UpdateCurrentControlSensitivity(FWidgetAssets::PlatformsKeys[Platform].ToString(), UpdatedValue, bHorizontal);
}

TArray<FString> UControlSettings::GetLanguages()
{
	TArray<FString> Languages;

	
	const UEnum* EnumPtr = StaticEnum<ELanguageCode>();

	if (!IsValid(EnumPtr))
	{
		return Languages;
	} 

	for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
	{
		//FText DisplayName = EnumPtr->GetDisplayNameTextByIndex(i);
		int32 Temp = EnumPtr->GetValueByIndex(i);
		/*if (Temp == StaticCast<int32>(ELanguageCode::DE_DE))
		{
			continue;	
		}*/
		FString DisplayName;
		switch (static_cast<ELanguageCode>(Temp))
		{
			case ELanguageCode::DE_DE:
				continue;
			case ELanguageCode::EN_US:
				DisplayName = "English";
				break;
			case ELanguageCode::ES_ES:
				DisplayName = "Spanish";
				break;
			case ELanguageCode::EU_ES:
				DisplayName = "Basque";
				break;
			case ELanguageCode::CA_ES:
				DisplayName = "Catalan";
				break;
			case ELanguageCode::FR_CH:
				DisplayName = "French";
				break;
			case ELanguageCode::SV_SE:
				DisplayName = "Swedish";
				break;
		}
		Languages.Add(DisplayName);
	}
	
	return Languages;
}

void UControlSettings::UpdateLanguage(const uint8 Index) const
{
	const UEnum* EnumPtr = StaticEnum<ELanguageCode>();
	uint8 ToAdd = 0;
	
	if (!IsValid(EnumPtr))
	{
		return;
	}
	
	if (Index >= StaticCast<uint8>(ELanguageCode::DE_DE))
	{
		ToAdd = 1;
	}
	
	if (Index + ToAdd >= EnumPtr->NumEnums() - 1)
	{
		return;
	}
	
	const ELanguageCode SelectedLanguage = static_cast<ELanguageCode>(EnumPtr->GetValueByIndex(Index + ToAdd));
	
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		return;
	}
	
	GameInstance->SetLanguageCode(SelectedLanguage);

	FWidgetAssets::OnLanguageChanged.Broadcast();
}