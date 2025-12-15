// Fill out your copyright notice in the Description page of Project Settings.


#include "SkipButtom.h"
#include "LevelSequencePlayer.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Icon/PlatformDependantWidget.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Controllers/MainController.h"

void USkipButtom::NativeConstruct()
{
	Super::NativeOnInitialized();
	
	SkipHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	
	SkipButton->OnClicked.AddDynamic(this, &USkipButtom::OnClickedSkipButton);
}

FReply USkipButtom::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);

	if (SkipImages->GetPlatformName() != EPlatform::PC && InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Bottom && SkipHorizontalBox->IsVisible())
	{
		SkipButton->OnClicked.Broadcast();
	}
	else if (SkipImages->GetPlatformName() == EPlatform::PC && InKeyEvent.GetKey() == EKeys::SpaceBar && SkipHorizontalBox->IsVisible())
	{
		SkipButton->OnClicked.Broadcast();
	}

	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->ParseInputDevice();
	}

	if (!SkipHorizontalBox->IsVisible())
	{
		SkipHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		return FReply::Handled();
	}

	return FReply::Handled();
}

FReply USkipButtom::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	SetKeyboardFocus();
	SetFocus();
	
	if (!SkipHorizontalBox->IsVisible())
	{
		SkipHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		return FReply::Handled();
	}

	return FReply::Handled();
}

void USkipButtom::OnClickedSkipButton()
{
	SkipButtonClicked.Broadcast();
	SequencePlayer->Stop();
}