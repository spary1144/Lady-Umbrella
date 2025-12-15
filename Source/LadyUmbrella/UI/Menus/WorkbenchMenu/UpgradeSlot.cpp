// Fill out your copyright notice in the Description page of Project Settings.

#include "UpgradeSlot.h"

#include "Components/AudioComponent.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeComponent.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "LadyUmbrella/Util/LoggerUtil.h"


void UUpgradeSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button->OnPressed.AddDynamic(this, &UUpgradeSlot::StartHold);
	Button->OnReleased.AddDynamic(this, &UUpgradeSlot::StopHold);

	if (IsValid(UpgradeSound))
	{
		AudioComp = UGameplayStatics::SpawnSound2D(this, UpgradeSound);
		AudioComp->RegisterComponent();
		AudioComp->Stop();
	}

	SetIsFocusable(true);
	SetIsEnabled(true);
}

FReply UUpgradeSlot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
	Button->OnHovered.Broadcast();
	return FReply::Handled();
}

void UUpgradeSlot::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	Button->OnUnhovered.Broadcast();
	StopHold();
}

void UUpgradeSlot::SetPriceText(const FText& Price)
{
	TextUpgradePrice->SetText(Price);
}

void UUpgradeSlot::UpdateLevel(FUpgrade& Upgrade)
{
	if (!Upgrade.IsAvailable())
	{
		bIsUnavailable = true;
		if (Upgrade.IsMaxedOut())
		{
			IconUpgradePrice->SetVisibility(ESlateVisibility::Hidden);
			TextUpgradePrice->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		bIsUnavailable = false;
	}
}

void UUpgradeSlot::SetupUpgradeSlots(const FUpgrade& UpgradeToShow)
{
	SetTextVar(UpgradeToShow.GetName());
	SetIdentifier(FName(UpgradeToShow.GetName().ToString()));
	SetButtonText(UpgradeToShow.GetName());
	SetDescription(FName(UpgradeToShow.GetSummary().ToString()));
	OnLanguageChanged();
	if (UpgradeToShow.IsMaxedOut() || UpgradeToShow.GetAllPrices().IsEmpty())
	{
		return;
	}
	SetPriceText(FText::FromString(FString::FromInt(UpgradeToShow.GetPrice())));
}

void UUpgradeSlot::StartHold()
{
	if (bIsUnavailable)
	{
		if (UnavailableAnim)
		{
			PlayAnimation(UnavailableAnim, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Forward, 1.f / HoldTime, true);
		}
		return;
	}
	
	if (!IconUpgradePrice->IsVisible() || !TextUpgradePrice->IsVisible() || !GetIsEnabled())
	{
		return;
	}

	if (GetWorld()->GetTimerManager().GetTimerRate(TimerHandleHeldButton) > 0)
	{
		return;
	}
	
	if (GetAnimationCurrentTime(HoldAnim) > 0)
	{
		ReverseAnimation(HoldAnim);
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandleHeldButton,
			this,
			&UUpgradeSlot::FinishHold,
			HoldTime - GetAnimationCurrentTime(HoldAnim),
			false);

		return;
	}
	
	if (AudioComp)
	{
		AudioComp->Play();
	}
	
	PlayAnimation(HoldAnim, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Forward, 1.f / HoldTime);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandleHeldButton,
		this,
		&UUpgradeSlot::FinishHold,
		HoldTime,
		false);
}

void UUpgradeSlot::StopHold()
{
	if (GetWorld()->GetTimerManager().GetTimerRate(TimerHandleHeldButton) > 0)
	{
		ReverseAnimation(HoldAnim);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleHeldButton);
	}
	
	if (AudioComp)
	{
		AudioComp->Stop();
	}
}

void UUpgradeSlot::FinishHold()
{
	if (ProgressEndedDelegate.ExecuteIfBound(GetUpgradeId()))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleHeldButton);
		PlayAnimation(HoldAnim, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Reverse, 2.f / HoldTime);
	}
}
