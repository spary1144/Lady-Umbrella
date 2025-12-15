// Fill out your copyright notice in the Description page of Project Settings.

#include "Credits.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void UCredits::NativeConstruct()
{
	Super::NativeConstruct();
	CurrentSlide     = 1;
	NameColumNumber  = 3;
	RowIndex         = 0;
	TitleColumNumber = 3;

	if (IsValid(CreditsTable))
	{
		CreditsTable->RowStruct = FCreditSlide::StaticStruct();
		CreditsTable->GetAllRows<FCreditSlide>(TEXT("Context"), AllRows);
	}
}

void UCredits::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	// UpdateColumns();
}

void UCredits::Show()
{
	if (AllRows.IsEmpty())
	{
		CreditsTable->GetAllRows<FCreditSlide>(TEXT("Context"), AllRows);
	}
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	ShowSlide();
}

void UCredits::Hide()
{
	PlayAnimation(FadeAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Forward, FadeSpeed);
	float Time = FadeAnim->GetEndTime();
	GetWorld()->GetTimerManager().SetTimer(
		SlideTimer,
		[this]()
		{
			SetVisibility(ESlateVisibility::Hidden);
			RemoveFromParent();
			UGameplayStatics::OpenLevel(this, TEXT("L_MainMenu"));
		},
		Time,false);
}

void UCredits::ShowSlide()
{
	FCreditSlide* Pointer = AllRows[RowIndex];
	
	UTextBlock* BlockNames = Cast<UTextBlock>(Names->GetChildAt(StaticCast<uint8>(Pointer->ColumnName)));
	if (IsValid(BlockNames))
	{
		BlockNames->SetText(Pointer->Names);
	}
	
	UTextBlock* BlockField = Cast<UTextBlock>(Titles->GetChildAt(StaticCast<uint8>(Pointer->ColumnField)));
	if (IsValid(BlockField))
	{
		BlockField->SetText(Pointer->Field);
	}

	TitleColumNumber = Pointer->SlideMaxFieldCols;
	NameColumNumber  = Pointer->SlideMaxNameCols;

	PlayAnimation(
		FadeAnim,
		FWidgetAssets::START_AT_BEGINNING,
		FWidgetAssets::SINGLE_PLAY_ANIMATION,
		EUMGSequencePlayMode::Reverse,
		FadeSpeed
	);

	const float TransitionTime = FadeAnim->GetEndTime();
	
	GetWorld()->GetTimerManager().SetTimer(
		SlideTimer,
		[this]()
		{
			if (RowIndex < MAX_SLIDE_NUM)
			{
				RowIndex++;
				TransitionToNext();
			}
			else
			{
				Hide();
			}
			
			UpdateColumns();
		},
		TransitionTime,
		false
	);
	
}

void UCredits::TransitionToNext()
{
	PlayAnimation(FadeAnim,
		FWidgetAssets::START_AT_BEGINNING,
		FWidgetAssets::SINGLE_PLAY_ANIMATION,
		EUMGSequencePlayMode::Forward,
		FadeSpeed
	);

	const float TransitionTime = FadeAnim->GetEndTime();
	
	GetWorld()->GetTimerManager().SetTimer(
		SlideTimer,
		[this]()
		{
			ShowSlide();
		},
		TransitionTime,
		false
	);
}

void UCredits::UpdateColumns() const
{
	if (!IsValid(Titles) || !IsValid(Names))
	{
		return;
	}
	
	uint8 Removed = Titles->GetChildrenCount();
	for (UWidget* Widget : Titles->GetAllChildren())
	{
		if (Removed > TitleColumNumber)
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			Removed = Removed > 0 ? Removed - 1 : 0;
			continue;
		}
		
		Widget->SetVisibility(ESlateVisibility::Visible);
	}

	Removed = Names->GetChildrenCount();
	for (UWidget* Widget : Names->GetAllChildren())
	{
		if (Removed > TitleColumNumber)
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			Removed = Removed > 0 ? Removed - 1 : 0;
			continue;
		}
		
		Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

