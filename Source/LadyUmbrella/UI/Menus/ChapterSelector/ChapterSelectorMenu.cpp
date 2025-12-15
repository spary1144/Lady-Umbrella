// Fill out your copyright notice in the Description page of Project Settings.


#include "ChapterSelectorMenu.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Controllers/MainMenuController.h"
#include "LadyUmbrella/UI/Basics/DescriptionBlock.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/SaveSystem/FChapterNames.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "LadyUmbrella/UI/Menus/MainMenu/MainMenu.h"

void UChapterSelectorMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	Chapters = { ChapterOne, ChapterTwo, ChapterThree, ChapterFour, ChapterFive };
	TArray<UUserWidget*> WidgetChapters;
	WidgetChapters.Reserve(Chapters.Num());

	for (UAnimatedBaseButton* Chapter : Chapters)
	{
		Chapter->SetIsFocusable(true);
		WidgetChapters.Add(Chapter);
	}

	SetOptions(WidgetChapters);

	const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		return;
	}

	const ULU_SaveGame* SaveGame = GameInstance->GetSaveGameFile();
	if (!IsValid(SaveGame))
	{
		return;
	}
	
	for (int32 Chapter = 1; Chapter < ChapterNames::ChapterList.Num() - 1; ++Chapter)
	{
		if (const FChaptersProgress* Progress = SaveGame->GetChaptersProgress().Find(FName(*ChapterNames::ChapterList[Chapter])))
		{
			if (!Progress->bChapterUnlocked)
			{
				Chapters[Chapter - 1]->SetIsEnabled(false);
				Chapters[Chapter - 1]->SetButtonText(FText::FromString(""));
			}
		}
	}
	
	ChapterOne->OnButtonActivated.AddDynamic(this, &UChapterSelectorMenu::LoadChapterOne);
	ChapterTwo->OnButtonActivated.AddDynamic(this, &UChapterSelectorMenu::LoadChapterTwo);
	ChapterThree->OnButtonActivated.AddDynamic(this, &UChapterSelectorMenu::LoadChapterThree);
	ChapterFour->OnButtonActivated.AddDynamic(this, &UChapterSelectorMenu::LoadChapterFour);
	ChapterFive->OnButtonActivated.AddDynamic(this, &UChapterSelectorMenu::LoadChapterFive);

	ChapterOne->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::UpdateChapterOneDescription);
	ChapterTwo->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::UpdateChapterTwoDescription);
	ChapterThree->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::UpdateChapterThreeDescription);
	ChapterFour->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::UpdateChapterFourDescription);
	ChapterFive->GetButton()->OnHovered.AddDynamic(this, &UChapterSelectorMenu::UpdateChapterFiveDescription);
}

FReply UChapterSelectorMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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
		if (UAnimatedBaseButton* Button = Cast<UAnimatedBaseButton>(GetOptions()[GetNavigationIndex()]))
		{
			Button->GetButton()->OnClicked.Broadcast();
		}
		return FReply::Handled();
				
	case ENavigationInput::EXIT :
		SetVisibility(ESlateVisibility::Collapsed);
		if (const APlayerController* PlayerController = GetOwningPlayer())
		{
			if (const AMainMenuController* Controller = Cast<AMainMenuController>(PlayerController))
			{
				int32 Index = Controller->GetMainMenu()->GetNavigationIndex();
				if (Index > 0 && Index < Controller->GetMainMenu()->GetOptions().Num())
				{
					Controller->GetMainMenu()->SetVisibility(ESlateVisibility::Visible);
					Controller->GetMainMenu()->SetNavigationIndex(Index);
					Controller->GetMainMenu()->GetOptions()[Index]->SetFocus();
					Controller->GetMainMenu()->GetOptions()[Index]->SetKeyboardFocus();
				}
			}
		}
		return FReply::Handled();
			
	default:
		return FReply::Handled();
	}
}

void UChapterSelectorMenu::HandleLoadChapters(const int32 Chapter) const
{
	if (ChapterNames::ChapterList.IsValidIndex(Chapter))
	{
		if(APlayerController* PlayerController = GetOwningPlayer<APlayerController>())
		{
			const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
			if (!IsValid(GameInstance))
			{
				return;
			}

			const ULU_SaveGame* SaveGame = GameInstance->GetSaveGameFile();
			if (!IsValid(SaveGame))
			{
				return;
			}

			GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMaxAmmo = 4;
			GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMagAmmo = 4;
			GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaInventoryAmmo = 20;
			GameInstance->GetSaveGameFile()->GetPlayerData().CurrentUpgradePieces = 0;
			for (TPair<EUpgradeType, FUpgrade>& UpgradePair : GameInstance->GetSaveGameFile()->GetUpgradesMap())
			{
				UpgradePair.Value.SetLevel(0);
			}
			
			UGameplayStatics::OpenLevel(GetWorld(), FName(*ChapterNames::ChapterList[Chapter]));
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
		}
	}
}

void UChapterSelectorMenu::HandleChapterDescription(const int32 Chapter)
{
	if (ChaptersImage.IsValidIndex(Chapter) && ChaptersDescription.IsValidIndex(Chapter))
	{
		if (Chapters[Chapter]->GetText().Equals(""))
		{
			Description->GetImage()->SetVisibility(ESlateVisibility::Hidden);
			Description->SetTitleText(FText::FromString("???"));
			Description->SetDescriptionText(FText::FromString("???"));
			Description->GetDescription()->SetJustification(ETextJustify::Center);
		}
		else
		{
			if (!Description->GetImage()->IsVisible())
			{
				Description->GetImage()->SetVisibility(ESlateVisibility::Visible);
				Description->GetDescription()->SetJustification(ETextJustify::Left);
			}
			Description->GetImage()->SetBrushFromTexture(ChaptersImage[Chapter]);
			Description->SetTitleText(FText::FromString(Chapters[Chapter]->GetText()));
			Description->SetDescriptionText(FText::FromString(ChaptersDescription[Chapter]));
		}
	}
}

void UChapterSelectorMenu::UpdateChapterOneDescription()
{
	HandleChapterDescription(0);
}

void UChapterSelectorMenu::UpdateChapterTwoDescription()
{
	HandleChapterDescription(1);
}

void UChapterSelectorMenu::UpdateChapterThreeDescription()
{
	HandleChapterDescription(2);
}

void UChapterSelectorMenu::UpdateChapterFourDescription()
{
	HandleChapterDescription(3);
}

void UChapterSelectorMenu::UpdateChapterFiveDescription()
{
	HandleChapterDescription(4);
}

void UChapterSelectorMenu::LoadChapterOne()
{
	HandleLoadChapters(1);
}

void UChapterSelectorMenu::LoadChapterTwo()
{
	HandleLoadChapters(2);
}

void UChapterSelectorMenu::LoadChapterThree()
{
	HandleLoadChapters(3);
}

void UChapterSelectorMenu::LoadChapterFour()
{
	HandleLoadChapters(4);
}

void UChapterSelectorMenu::LoadChapterFive()
{
	HandleLoadChapters(5);
}

void UChapterSelectorMenu::Show()
{
	SetVisibility(ESlateVisibility::Visible);
	SetNavigationIndex(0);
	GetOptions()[0]->SetFocus();
	GetOptions()[0]->SetKeyboardFocus();
	GetOptions()[0]->OnAddedToFocusPath(FFocusEvent());
	HandleChapterDescription(0);
}

