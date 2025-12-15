#include "SelectorInput.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void USelectorInput::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PrevOption->OnClicked.AddDynamic(this, &USelectorInput::SwitchPrev);
	NextOption->OnClicked.AddDynamic(this, &USelectorInput::SwitchNext);
}

TObjectPtr<UTextBlock> USelectorInput::GetOptionTextBlock(EOption Target)
{
	UOverlay* Container;
	switch (Target)
	{
		case EOption::PREVIOUS:
			Container = Cast<UOverlay>(PrevOption->GetChildAt(0));
			return Cast<UTextBlock>(Container->GetChildAt(0));;
		case EOption::CURRENT:
			return CurrOptionText;
		case EOption::NEXT:
			Container = Cast<UOverlay>(NextOption->GetChildAt(0));
			return Cast<UTextBlock>(Container->GetChildAt(0));;
		default:
			UE_LOG(LogTemp, Warning, TEXT("USelectorInput::GetOptionTextBlock: Invalid Option Type"));
			return CurrOptionText;
	}
}

FString USelectorInput::GetOptionText(EOption Target)
{
	return GetOptionTextBlock(Target)->GetText().ToString();
}

void USelectorInput::SetOptionText(EOption Target, FString Text)
{
	UTextBlock* TextBlock = Cast<UTextBlock>(GetOptionTextBlock(Target));
	if(!IsValid(TextBlock))
	{
		UE_LOG(LogTemp, Warning, TEXT("USelectorInput::SetOptionText: Invalid Option Type"));
		return;
	}	
	TextBlock->SetText(FText::FromString(Text));
}


void USelectorInput::UpdateOptions()
{
	int Index = FMath::Clamp(GetCurrentOptionIndex(),0,Options.Num()-1);
	FString PrevText = Index > 0 ? Options[Index - 1] : "";
	FString NextText = Index < Options.Num() - 1 ? Options[Index + 1] : "";

	SetCurrentOptionIndex(Index);
	
	if (Options.Num() > 0)
	{
		SetOptionText(EOption::PREVIOUS, PrevText);
		SetOptionText(EOption::CURRENT, Options[Index]);
		SetOptionText(EOption::NEXT, NextText);
	}
	
	if (IsValid(SelectorSound))
	{
		UGameplayStatics::SpawnSound2D(this, SelectorSound);
	}
}

void USelectorInput::SwitchPrev()
{
	int PrevIndex = GetCurrentOptionIndex() - 1;
	PrevIndex = PrevIndex < 0 ? GetSelectorOptions().Num() - 1 : PrevIndex;
	
	PlayAnimation(SwitchOptionAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Forward, AnimationSpeedMultiplier);
	SetCurrentOptionIndex(PrevIndex);
	UpdateOptions();
	PlayAnimation(SwitchOptionAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Reverse, AnimationSpeedMultiplier);
	OnOptionUpdated.Broadcast(CurrOptionIndex);
}

void USelectorInput::SwitchNext()
{
	int NextIndex = GetCurrentOptionIndex() + 1;
	NextIndex = NextIndex >= Options.Num() ? 0 : NextIndex;
	
	PlayAnimation(SwitchOptionAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Forward, AnimationSpeedMultiplier);
	SetCurrentOptionIndex(NextIndex);
	UpdateOptions();
	PlayAnimation(SwitchOptionAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Reverse, AnimationSpeedMultiplier);
	OnOptionUpdated.Broadcast(CurrOptionIndex);
}
