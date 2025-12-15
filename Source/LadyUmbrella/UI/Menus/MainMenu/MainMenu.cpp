//  
// MainMenu.cpp
// 
// Implementation of the MainMenu class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MainMenu.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Controllers/MainMenuController.h"
#include "LadyUmbrella/Environment/TriggerLevelChange/EChapterNames.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "LadyUmbrella/UI/Menus/ChapterSelector/ChapterSelectorMenu.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/SettingsMenu.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/SaveGameSlotNames.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetNewGameButton()->OnButtonActivated.AddDynamic(this, &UMainMenu::NewGame);
	GetContinueButton()->OnButtonActivated.AddDynamic(this, &UMainMenu::ContinueGame);
	GetChaptersMenuButton()->OnButtonActivated.AddDynamic(this, &UMainMenu::ShowChaptersMenu);
	GetSettingsButton()->OnButtonActivated.AddDynamic(this, &UMainMenu::ShowSettings);
	GetCreditsButton()->OnButtonActivated.AddDynamic(this, &UMainMenu::ShowCredits);
	FWidgetAssets::InitializeWidgetAssets();
}

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	GetExitGameButton()->OnButtonActivated.AddDynamic(this, &UMainMenu::ExitDesktop);
	// We retrieve the state of the first level to know if we set continue to idle or blocked
	GetContinueButton()->SetIsEnabled(false);
	GetChaptersMenuButton()->SetIsEnabled(false);
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance) && GameInstance->GetSaveGameFlag())
	{
		FChaptersProgress ProgressFirst = GameInstance->GetSaveGameFile()->GetChaptersProgress().begin().Value();
		if (ProgressFirst.bChapterUnlocked)
		{
			GetContinueButton()->SetIsEnabled(true);
		}
		
		FChaptersProgress ProgressSecond = GameInstance->GetSaveGameFile()->GetChaptersProgress().Array()[1].Value;
		if (ProgressSecond.bChapterUnlocked)
		{
			GetChaptersMenuButton()->SetIsEnabled(true);
		}
	}
}

void UMainMenu::ContinueGame()
{
	/*
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in UMainMenu.ContinueGame()");
		return;
	}
	*/

	ButtonClickedDelegate.Broadcast();

	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());

	if (!IsValid(GameInstance))
	{
		return;
	}
	check(GameInstance->ReadSaveGameFile());
	GameInstance->OpenLevelFromLoadGame();
	
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	RemoveFromParent();
	PlayAnimation(Fade, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Forward, FadeSpeed);
}

void UMainMenu::ShowCredits()
{
	if (IsValid(FGlobalPointers::GameInstance))
	{
		FGlobalPointers::GameInstance->ShowCredits();
	}
}

void UMainMenu::ShowSettings()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (const APlayerController* PlayerController = GetOwningPlayer())
	{
		if (const AMainMenuController* Controller = Cast<AMainMenuController>(PlayerController))
		{
			Controller->GetSettingsMenu()->LoadGame();
			Controller->GetSettingsMenu()->Show();
		}
	}
}

void UMainMenu::Show()
{
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	//FGlobalPointers::MainController->SetInputMode(FInputModeUIOnly());
	//FGlobalPointers::MainController->bShowMouseCursor = true;

	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->bShowMouseCursor = false;
	
	SetNavigationIndex(0);
	TArray<UUserWidget*> Options = GetOptions();
	if (!Options.IsEmpty())
	{
		Options[GetNavigationIndex()]->SetFocus();
		Options[GetNavigationIndex()]->SetKeyboardFocus();
		Options[GetNavigationIndex()]->OnAddedToFocusPath(FFocusEvent());
	}
	SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::NewGame()
{
	ButtonClickedDelegate.Broadcast();
	
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		GameInstance->SetCurrentSaveSlot(SaveGameSlots::SaveSlot1);
	}
	
	APlayerController* PlayerController = GetOwningPlayer<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	RemoveFromParent();
	UGameplayStatics::OpenLevel(this, ToName(EChapterNames::Chapter0));
	// We set the first chapter to true so that continue button appears Idle and we save game
	if (GameInstance->UnlockLevelProgress(ToName(EChapterNames::Chapter0)))
	{
		if (IsValid(GameInstance->GetSaveGameFile()))
		{
			GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMaxAmmo = 4;
			GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMagAmmo = 4;
			GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaInventoryAmmo = 20;
			GameInstance->GetSaveGameFile()->GetPlayerData().CurrentUpgradePieces = 0;
			for (TPair<EUpgradeType, FUpgrade>& UpgradePair : GameInstance->GetSaveGameFile()->GetUpgradesMap())
			{
				UpgradePair.Value.SetLevel(0);
			}
		}
		GameInstance->SaveGame();
	}
}

void UMainMenu::ShowChaptersMenu()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (const APlayerController* PlayerController = GetOwningPlayer())
	{
		if (const AMainMenuController* Controller = Cast<AMainMenuController>(PlayerController))
		{
			Controller->GetChapterSelectorMenu()->Show();
		}
	}
}

void UMainMenu::ExitDesktop()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

FReply UMainMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	switch (IsNavigatingSomewhere(InKeyEvent.GetKey()))
	{
		case ENavigationInput::NAVIGATE_UP :
			MenuNavigation(false);
			return FReply::Handled();
				
		case ENavigationInput::NAVIGATE_DOWN :
			MenuNavigation(true);
			return FReply::Handled();
					
		case ENavigationInput::INTERACT :
			if (GetNavigationIndex() < 0)
			{
				return FReply::Handled();
			}
			if (UAnimatedBaseButton* Button = Cast<UAnimatedBaseButton>(GetOptions()[GetNavigationIndex()]))
			{
				Button->GetButton()->OnClicked.Broadcast();
			}
			return FReply::Handled();
				
		case ENavigationInput::EXIT :
			return FReply::Handled();
			
		default:
			return FReply::Handled();
	}
}