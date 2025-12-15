#include "HintBase.h"

#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

void UHintBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	bIsShowing = false;
	SetVisibility(ESlateVisibility::Hidden);
	FWidgetAssets::OnLanguageChanged.AddUObject(this, &UHintBase::OnLanguageChanged);	
	OnLanguageChanged();
}


void UHintBase::OnLanguageChanged()
{
	int32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;

	if (TitleCode != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(TitleCode);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		HintTitle->SetText(NewText);
	}
	
	if (DescriptionCode != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(DescriptionCode);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		HintDescription->SetText(NewText);
	}
}

void UHintBase::SynchronizeProperties()	// UMG Designer exclusive function
{
	Super::SynchronizeProperties();
	if (!IsValid(InputType) || !IsValid(Separator) || !IsValid(HintTitle))
	{
		return;
	}
	InputType->SetVisibility(bShowInputOnHint ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	Separator->SetVisibility(bShowTitle ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	HintTitle->SetVisibility(bShowTitle ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
		
void UHintBase::ToggleHint()
{
	if (!IsValid(this))
	{
		return;
	}
	bIsShowing = !bIsShowing;
	
	if (bIsShowing)
	{
		SetVisibility(ESlateVisibility::Visible);
		float AnimTime = 0.f;
		if (bScaleInOut && IsValid(ScaleIn))
		{
			PlayAnimation(ScaleIn, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Forward, AnimSpeed);
			AnimTime = ScaleIn->GetEndTime();
		}
		else if (IsValid(FadeIn))
		{
			PlayAnimation(FadeIn, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Forward, AnimSpeed);
			AnimTime = FadeIn->GetEndTime();
		}
		
		if (bTimed)
		{
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]()
				{
					ToggleHint();
				},
				DisplayDuration + AnimTime,
				false
			);
		}
		
		if(OnShowChangeTo.IsBound())
		{
			OnShowChangeTo.Broadcast(true);
		}
	}
	else
	{
		UUMGSequencePlayer* AnimationPlayer;
		if (bScaleInOut)
		{
			float StartingPos = IsPlayingAnimation() ? ScaleIn->GetEndTime() - GetAnimationCurrentTime(ScaleIn) : FWidgetAssets::START_AT_BEGINNING;
			AnimationPlayer = PlayAnimation(ScaleIn, StartingPos, 1, EUMGSequencePlayMode::Reverse, AnimSpeed);
		}
		else
		{
			float StartingPos = IsPlayingAnimation() ? FadeIn->GetEndTime() - GetAnimationCurrentTime(FadeIn) : FWidgetAssets::START_AT_BEGINNING;
			AnimationPlayer = PlayAnimation(FadeIn, FWidgetAssets::START_AT_BEGINNING, 1, EUMGSequencePlayMode::Reverse, AnimSpeed);
		}
		if(IsValid(AnimationPlayer))
		{
			AnimationPlayer->OnSequenceFinishedPlaying().AddLambda([this](UUMGSequencePlayer&)
			{
				SetVisibility(ESlateVisibility::Visible);
				if(OnShowChangeTo.IsBound())
				{
					OnShowChangeTo.Broadcast(false);
				}
				
			});
		}
	}
}

FText UHintBase::GetDescriptionText()
{
	return HintDescription->GetText();
}

FText UHintBase::GetTitleText()
{
	return HintTitle->GetText();
}

void UHintBase::SetDescriptionText(FText Description)
{
	HintDescription->SetText(Description);
}

void UHintBase::SetTitleText(FText Title)
{
	HintTitle->SetText(Title);
}

void UHintBase::SetHintAlignment(EScreenPosition Alingment, FVector2D Pad)
{
	UVerticalBoxSlot* HSlot = Cast<UVerticalBoxSlot>(HintContainer->Slot);
	switch (Alingment)
	{
		case EScreenPosition::BOTTOM:
			HSlot->SetHorizontalAlignment(HAlign_Center);
			HSlot->SetVerticalAlignment(VAlign_Bottom);
			break;
		case EScreenPosition::BOTTOM_LEFT:
			HSlot->SetHorizontalAlignment(HAlign_Left);
			HSlot->SetVerticalAlignment(VAlign_Bottom);
			break;
		case EScreenPosition::BOTTOM_RIGHT:
			HSlot->SetHorizontalAlignment(HAlign_Right);
			HSlot->SetVerticalAlignment(VAlign_Bottom);
			break;
		case EScreenPosition::TOP:
			HSlot->SetHorizontalAlignment(HAlign_Center);
			HSlot->SetVerticalAlignment(VAlign_Top);
			break;
		case EScreenPosition::TOP_LEFT:
			HSlot->SetHorizontalAlignment(HAlign_Left);
			HSlot->SetVerticalAlignment(VAlign_Top);
			break;
		case EScreenPosition::TOP_RIGHT:
			HSlot->SetHorizontalAlignment(HAlign_Right);
			HSlot->SetVerticalAlignment(VAlign_Top);
			break;
		case EScreenPosition::LEFT:
			HSlot->SetHorizontalAlignment(HAlign_Left);
			HSlot->SetVerticalAlignment(VAlign_Center);
			break;
		case EScreenPosition::RIGHT:
			HSlot->SetHorizontalAlignment(HAlign_Right);
			HSlot->SetVerticalAlignment(VAlign_Center);
			break;	
		case EScreenPosition::CENTER:
			HSlot->SetHorizontalAlignment(HAlign_Center);
			HSlot->SetVerticalAlignment(VAlign_Center);
			break;	
	}
	
	FMargin ProcessedPadding(
		Pad.X < 0 ? FMath::Abs(Pad.X) : 0.f,
		Pad.Y > 0 ? Pad.Y : 0.f,
		Pad.X > 0 ? Pad.X : 0.f,
		Pad.Y < 0 ? FMath::Abs(Pad.Y) : 0.f
		);
	
	HSlot->SetPadding(ProcessedPadding);
}
