// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuSection.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "LadyUmbrella/UI/HUD/MainHUD.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/SettingsMenu.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/UI/Basics/Input/InputElement.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

FReply UMenuSection::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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
		default:
			return FReply::Unhandled();
	}
}

void UMenuSection::Show()
{
	AddToViewport();
	SetNavigationIndex(0);
	TArray<UUserWidget*> Options = GetOptions();
	if (!Options.IsEmpty())
	{
		Options[GetNavigationIndex()]->SetFocus();
		Options[GetNavigationIndex()]->SetKeyboardFocus();
	}
	SetVisibility(ESlateVisibility::Visible);
}


