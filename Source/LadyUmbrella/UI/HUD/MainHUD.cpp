// 
// MainHUD.cpp
// 
// Implementation of the MainHUD class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MainHUD.h"
#include "PelletHitMarker.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "LadyUmbrella/UI/HUD/AmmoCounter.h"
#include "LadyUmbrella/UI/HUD/Crosshair.h"
#include "LadyUmbrella/UI/HUD/Subtitles.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/UI/Menus/CheatMenu/CheatMenu.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/SettingsMenu.h"
#include "LadyUmbrella/UI/HUD/GrenadeIndicator.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Animation/WidgetAnimation.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	
	SetPauseMenuInstance(GetPauseMenuWidget());
	SetCheatMenuInstance(GetCheatMenuWidget());
	SetSettingsMenuInstance(GetSettingsMenuWidget());
	SetSubtitleInstance(GetSubtitlesWidget());
	SetHUDInstance(GetHUDWidget());

	if (IsValid(GetHUDInstance()))
	{
		GrenadeIndicatorInstance = FindWidgetOfClass<UGrenadeIndicator>(GetHUDInstance());
		GrenadeIndicatorInstance->SetVisibility(ESlateVisibility::Hidden);
		GetHUDInstance()->AddToViewport();
	}
	
	GetSubtitlesInstance()->AddToViewport();
}

void AMainHUD::OnUmbrellaSpawned()
{
	SetHUDInstance(GetHUDWidget());

	if (!IsValid(GetHUDInstance()))
	{
		return;
	}
	
	AmmoCounterInstance = FindWidgetOfClass<UAmmoCounter>(GetHUDInstance());
	PelletHitMarkerInstance = FindWidgetOfClass<UPelletHitMarker>(GetHUDInstance());
	TArray<UCrosshair*> CrosshairWidgets;
	FindAllWidgetsOfClass<UCrosshair>(GetHUDInstance(), CrosshairWidgets);
	
	for (UCrosshair* Crosshair : CrosshairWidgets)
	{
		if (Crosshair->GetIsLeftBorder())
		{
			CrosshairLeftBorderInstance = Crosshair;
			UWidgetAnimation* LeftAnim = FindAnimationByName(HUDInstance, TEXT("ShootingCadenceLeft"));
			if (IsValid(LeftAnim))
			{
				CrosshairLeftBorderInstance->SetAnimationOwner(HUDInstance, LeftAnim);
			}
		}
		else if (Crosshair->GetIsRightBorder())
		{
			CrosshairRightBorderInstance = Crosshair;
			UWidgetAnimation* RightAnim = FindAnimationByName(HUDInstance, TEXT("ShootingCadenceRight"));
			if (IsValid(RightAnim))
			{
				CrosshairRightBorderInstance->SetAnimationOwner(HUDInstance, RightAnim);
			}
		}
		else
		{
			CrosshairDot = Crosshair;
		}
	}
	
	TogglePlayerHUD();
}

void AMainHUD::HideEverythingExceptSubtitle() const
{
	GetAmmoCounterInstance()->SetVisibility(ESlateVisibility::Hidden);
	CrosshairLeftBorderInstance->SetVisibility(ESlateVisibility::Hidden);
	CrosshairDot->SetVisibility(ESlateVisibility::Hidden);
	CrosshairRightBorderInstance->SetVisibility(ESlateVisibility::Hidden);
}

void AMainHUD::ShowEverythingExceptMenus() const
{
	GetAmmoCounterInstance()->SetVisibility(ESlateVisibility::Visible);
	CrosshairLeftBorderInstance->SetVisibility(ESlateVisibility::Visible);
	CrosshairDot->SetVisibility(ESlateVisibility::Visible);
	CrosshairRightBorderInstance->SetVisibility(ESlateVisibility::Visible);
	GetSubtitlesInstance()->SetVisibility(ESlateVisibility::Visible);
}

void AMainHUD::TogglePauseMenu() const
{
	if (IsValid(GetPauseMenuInstance()))
	{
		GetPauseMenuInstance()->Show();
	}
}

void AMainHUD::ToggleCheatMenu()
{
	if (IsValid(GetCheatMenuInstance()) && bCheatMenuCanToggle)
	{
		if (GetCheatMenuInstance()->IsVisible())
		{
			GetCheatMenuInstance()->Hide();
		}
		else
		{
			GetCheatMenuInstance()->Show();
		}

		bCheatMenuCanToggle = false;
	}
}

void AMainHUD::EnableToggleCheatMenu()
{
	bCheatMenuCanToggle = true;
}

void AMainHUD::ToggleSettingsMenu() const
{
	if (IsValid(GetSettingsMenuInstance()))
	{
		GetSettingsMenuInstance()->Show();
	}
}

void AMainHUD::TogglePlayerHUD()
{
	if (IsValid(GetAmmoCounterInstance()) && IsValid(PelletHitMarkerInstance) && IsValid(CrosshairLeftBorderInstance)
		&& IsValid(CrosshairDot) && IsValid(CrosshairRightBorderInstance))
	{
		GetAmmoCounterInstance()->Show();
		PelletHitMarkerInstance->Show();
		CrosshairLeftBorderInstance->Show();
		CrosshairDot->Show();
		CrosshairRightBorderInstance->Show();
	}
	
	SetPlayerHUDState(true);
}

void AMainHUD::UnTogglePlayerHUD()
{
	if (IsValid(GetAmmoCounterInstance()) && IsValid(PelletHitMarkerInstance) && IsValid(CrosshairLeftBorderInstance)
		&& IsValid(CrosshairDot) && IsValid(CrosshairRightBorderInstance))
	{
		GetAmmoCounterInstance()->Hide();
		PelletHitMarkerInstance->Hide();
		CrosshairLeftBorderInstance->Hide();
		CrosshairDot->Hide();
		CrosshairRightBorderInstance->Hide();
	}

	SetPlayerHUDState(false);
}


void AMainHUD::ToggleGrenadeIndicator(AGrenade* Grenade) const
{
	if (IsValid(GetGrenadeIndicatorInstance()))
	{
		GetGrenadeIndicatorInstance()->Show(Grenade);
	}
}

void AMainHUD::UnToggleGrenadeIndicator() const
{
	if (IsValid(GetGrenadeIndicatorInstance()))
	{
		GetGrenadeIndicatorInstance()->Hide();
	}
}

void AMainHUD::AddSubtitle(const FString& CharacterName, const FString& Subtitle,const float& Duration) const
{
	GetSubtitlesInstance()->AddSubtitle(CharacterName, Subtitle, Duration);
}

void AMainHUD::RemoveSubtitle(const FString& CharacterName) const
{
	GetSubtitlesInstance()->RemoveSubtitle(CharacterName);
}

void AMainHUD::HidePlayerHUDOnTimer()
{
	GetWorldTimerManager().SetTimer(
		HidePlayerHUDTimerHandle,
		this,
		&AMainHUD::UnTogglePlayerHUD,
		HidePlayerHUDTime,
		false,
		HidePlayerHUDTime
	);
}

void AMainHUD::ResetHidePlayerHUDTimer()
{
	if (GetWorldTimerManager().IsTimerActive(HidePlayerHUDTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(HidePlayerHUDTimerHandle);
	}
}

UPauseMenu* AMainHUD::GetPauseMenuInstance() const 
{
	return PauseMenuInstance;
}

UCheatMenu* AMainHUD::GetCheatMenuInstance() const 
{
	return CheatMenuInstance;
}

TSubclassOf<UCheatMenu> AMainHUD::GetCheatMenuWidget() const
{
	return CheatMenuWidget;
}

UAmmoCounter* AMainHUD::GetAmmoCounterInstance() const
{
	return AmmoCounterInstance;
}

TSubclassOf<UPauseMenu> AMainHUD::GetPauseMenuWidget() const
{
	return PauseMenuWidget;
}

USettingsMenu* AMainHUD::GetSettingsMenuInstance() const
{
	return SettingsMenuInstance;
}

TSubclassOf<USettingsMenu>  AMainHUD::GetSettingsMenuWidget() const
{
	return SettingsMenuWidget;
}

USubtitles* AMainHUD::GetSubtitlesInstance() const
{
	return SubtitlesInstance;
}

TSubclassOf<USubtitles> AMainHUD::GetSubtitlesWidget() const
{
	return SubtitlesWidget;
}

UGrenadeIndicator* AMainHUD::GetGrenadeIndicatorInstance() const
{
	return GrenadeIndicatorInstance;
}

UUserWidget* AMainHUD::GetHUDInstance() const
{
	return HUDInstance;
}

TSubclassOf<UUserWidget> AMainHUD::GetHUDWidget() const
{
	return HUDWidget;
}

bool AMainHUD::GetPlayerHUDState() const
{
	return bIsOn;
}

void AMainHUD::SetPauseMenuInstance(const TSubclassOf<UPauseMenu>& NewPauseMenuWidget)
{
	if (!IsValid(GetPauseMenuInstance()) && IsValid(NewPauseMenuWidget))
	{
		PauseMenuInstance = CreateWidget<UPauseMenu>(GetWorld(), NewPauseMenuWidget);
	}
}

void AMainHUD::SetCheatMenuInstance(const TSubclassOf<UCheatMenu>& NewCheatMenuWidget)
{
	if (!IsValid(GetCheatMenuInstance()) && IsValid(NewCheatMenuWidget))
	{
		CheatMenuInstance = CreateWidget<UCheatMenu>(GetWorld(), NewCheatMenuWidget);
	}
}

void AMainHUD::SetSettingsMenuInstance(const TSubclassOf<USettingsMenu>& NewSettingsMenuWidget)
{
	if (!IsValid(GetSettingsMenuInstance()) && IsValid(NewSettingsMenuWidget))
	{
		SettingsMenuInstance = CreateWidget<USettingsMenu>(GetWorld(), NewSettingsMenuWidget);
		if (IsValid(SettingsMenuInstance))
		{
			SettingsMenuInstance->LoadGame();
			// If we want to ALWAYS force default XBOX Icons on game start (even if they were configured before)
			// vvv comment everything bellow to not enforce anything vvv
			/**/ TArray<FInputConfig> GameplayConfig = SettingsMenuInstance->GetOptionPopulationSections()["Gameplay"].InputConfigs;
			/**/ for (int Index = 0; Index < GameplayConfig.Num(); Index++)
			/**/ {
			/**/ 	if(FText::FromString("Language_Title").EqualTo(GameplayConfig[Index].Title))
			/**/ 	{
			/**/ 		SettingsMenuInstance->GetOptionPopulationSections()["Gameplay"].InputConfigs[Index].InitialState = 1.f;
			/**/ 	}
			/**/ }
		}
	}
}

void AMainHUD::SetSubtitleInstance(const TSubclassOf<USubtitles>& NewSubtitleWidget)
{
	if (!IsValid(GetSubtitlesInstance()) && IsValid(NewSubtitleWidget))
	{
		SubtitlesInstance = CreateWidget<USubtitles>(GetWorld(), NewSubtitleWidget);
	}
}

void AMainHUD::SetHUDInstance(const TSubclassOf<UUserWidget>& NewHUDWidget)
{
	if (!IsValid(GetHUDInstance()) && IsValid(NewHUDWidget))
	{
		HUDInstance = CreateWidget<UUserWidget>(GetWorld(), NewHUDWidget);
	}
}

void AMainHUD::SetPlayerHUDState(const bool State)
{
	bIsOn = State;
}

template <typename T>
T* AMainHUD::FindWidgetOfClass(UUserWidget* RootWidget)
{
	if (!RootWidget)
		return nullptr;

	TArray<UWidget*> AllWidgets;
	RootWidget->WidgetTree->GetAllWidgets(AllWidgets);

	for (UWidget* Widget : AllWidgets)
	{
		if (T* Found = Cast<T>(Widget))
		{
			return Found;
		}
	}

	return nullptr;
}

template <typename T>
void AMainHUD::FindAllWidgetsOfClass(UUserWidget* RootWidget, TArray<T*>& OutWidgets)
{
	if (!RootWidget)
		return;

	TArray<UWidget*> AllWidgets;
	RootWidget->WidgetTree->GetAllWidgets(AllWidgets);

	for (UWidget* Widget : AllWidgets)
	{
		if (T* Found = Cast<T>(Widget))
		{
			OutWidgets.Add(Found);
		}
	}
}

UWidgetAnimation* AMainHUD::FindAnimationByName(const UUserWidget* Widget, const FName& AnimName)
{
	if (!IsValid(Widget))
	{
		return nullptr;
	}
	
	UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(Widget->GetClass());
	
	if (!WidgetClass)
	{
		return nullptr;
	}	
	
	for (UWidgetAnimation* Anim : WidgetClass->Animations)
	{
		FString AnimNameStr = Anim->GetName();
		
		if (AnimNameStr.EndsWith(TEXT("_INST")))
		{
			AnimNameStr.RemoveFromEnd(TEXT("_INST"));
		}
		
		if (AnimNameStr.Equals(AnimName.ToString()) )
		{
			return Anim;
		}
	}

	return nullptr;
}