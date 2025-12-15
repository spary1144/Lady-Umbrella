//  
// PauseMenu.cpp
// 
// Implementation of the PauseMenu class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "PauseMenu.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/SettingsMenu.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/UI/Basics/Input/InputElement.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

void UPauseMenu::Show()
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in PauseMenu.Show()");
		return;
	}
	
	UGameplayStatics::SpawnSound2D(this, PauseOff);
	
	FGlobalPointers::MainController->SetInputMode(FInputModeUIOnly());
	FGlobalPointers::MainController->bShowMouseCursor = true;
	FGlobalPointers::MainController->Pause();

	if (!IsInViewport())
	{
		AddToViewport();
	}
	
	SetNavigationIndex(0);
	TArray<UUserWidget*> Options = GetOptions();
	if (!Options.IsEmpty())
	{
		Options[GetNavigationIndex()]->SetFocus();
		Options[GetNavigationIndex()]->SetKeyboardFocus();
		Options[GetNavigationIndex()]->OnAddedToFocusPath(FFocusEvent());
	}
	SetVisibility(ESlateVisibility::Visible);
	FVector2D Viewport = FVector2D::ZeroVector;
	GEngine->GameViewport->GetViewportSize(Viewport);
	FGlobalPointers::MainController->SetMouseLocation(Viewport.X/2.f, Viewport.Y/2.f);

	FWidgetAssets::OnLanguageChanged.Broadcast();
}

void UPauseMenu::Resume()
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in PauseMenu.Resume()");
		return;
	}

	UGameplayStatics::SpawnSound2D(this, PauseOff);
	
	FGlobalPointers::MainController->SetInputMode(FInputModeGameOnly());
	FGlobalPointers::MainController->bShowMouseCursor = false;
	FGlobalPointers::MainController->SetPause(false);
	SetVisibility(ESlateVisibility::Hidden);
	FGlobalPointers::MainController->TogglePlayerHUD();
}

void UPauseMenu::Load()
{
	if (!IsValid(FGlobalPointers::GameInstance))
	{
		return;
	}
	FGlobalPointers::GameInstance->LoadCheckpoint();
	
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return;
	}
	FGlobalPointers::PlayerCharacter->CheckpointCameraReset();
	Resume();
}

void UPauseMenu::Settings()
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		FLogger::ErrorLog("Invalid Global pointers in PauseMenu.Settings()");
		return;
	}
	
	FGlobalPointers::MainController->ToggleSettingsMenu();
}

void UPauseMenu::ExitMainMenu()
{
	AActor* Actor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	AController* Controller = Cast<AController>(Actor);
	AMainController* MainController = Cast<AMainController>(Controller);
	
	if (IsValid(MainController))
	{
		MainController->SetInputMode(FInputModeUIOnly());
		MainController->SetShowMouseCursor(true);
	}
	UGameplayStatics::OpenLevel(this, TEXT("L_MainMenu"));
}

void UPauseMenu::ExitDesktop()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetResumeButton()->OnButtonActivated.AddDynamic(this, &UPauseMenu::Resume);
	GetLoadButton()->OnButtonActivated.AddDynamic(this, &UPauseMenu::Load);
	GetSettingsButton()->OnButtonActivated.AddDynamic(this, &UPauseMenu::Settings);
	GetExitMainMenuButton()->OnButtonActivated.AddDynamic(this, &UPauseMenu::ExitMainMenu);
	GetExitDesktopButton()->OnButtonActivated.AddDynamic(this, &UPauseMenu::ExitDesktop);
	FWidgetAssets::OnLanguageChanged.Broadcast();

}

FReply UPauseMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);

	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (IsValid(Controller))
	{
		Controller->SetInputMode(FInputModeUIOnly());
		//Controller->SetShowMouseCursor(true);
	}
	
	switch (IsNavigatingSomewhere(InKeyEvent.GetKey()))
	{
		case ENavigationInput::NAVIGATE_UP :

			MenuNavigation(false);
			return FReply::Handled();
			
		case ENavigationInput::NAVIGATE_DOWN :
			MenuNavigation(true);
			return FReply::Handled();
				
		case ENavigationInput::INTERACT :
		{
			const int32 CurrenIndex = GetNavigationIndex();
			if (CurrenIndex < 0)
			{
				return FReply::Handled();
			}
			
			if (UAnimatedBaseButton* Button = Cast<UAnimatedBaseButton>(GetOptions()[CurrenIndex]))
			{
				Button->GetButton()->OnClicked.Broadcast();
			}
			return FReply::Unhandled();
		}	
		case ENavigationInput::EXIT :
			Resume();
			return FReply::Handled();
		
		default:
			return FReply::Unhandled();
	}
}