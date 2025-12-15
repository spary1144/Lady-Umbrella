// 
// MainMenuController.cpp
// 
// Implementation of the MainMenuController class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "MainMenuController.h"
#include "LadyUmbrella/UI/Menus/MainMenu/MainMenu.h"
#include "LadyUmbrella/UI/Menus/ChapterSelector/ChapterSelectorMenu.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/SettingsMenu.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MainMenuWidget))
	{
		MainMenuInstance = CreateWidget<UMainMenu>(GetWorld(), MainMenuWidget);
		MainMenuInstance->AddToViewport();
		MainMenuInstance->Show();
	}

	if (IsValid(ChapterSelectorMenuWidget))
	{
		ChapterSelectorMenuInstance = CreateWidget<UChapterSelectorMenu>(GetWorld(), ChapterSelectorMenuWidget);
		ChapterSelectorMenuInstance->AddToViewport();
		ChapterSelectorMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (IsValid(SettingsMenuWidget))
	{
		SettingsMenuInstance = CreateWidget<USettingsMenu>(GetWorld(), SettingsMenuWidget);
		SettingsMenuInstance->AddToViewport();
		SettingsMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
	}
}

