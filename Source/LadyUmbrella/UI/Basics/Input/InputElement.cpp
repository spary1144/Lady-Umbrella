#include "InputElement.h"
#include "InputType/SelectorInput.h"
#include "InputType/SliderInput.h"
#include "InputType/ToggleInput.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void UInputElement::NativeConstruct()
{
	Super::NativeConstruct();
	OnLanguageChanged();
}

void UInputElement::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!IsValid(GetInputWidget()))
	{
		return;
	}
	
	OnLanguageChanged();
	if (!SelectionList.IsEmpty() && InputType == EInputElementType::SELECTOR && GetInputWidget()->IsA(USelectorInput::StaticClass()))
	{
		USelectorInput* Widget = Cast<USelectorInput>(GetInputWidget());
		Widget->SetOptions(SelectionList);
		Widget->UpdateOptions();
	}

	FWidgetAssets::OnLanguageChanged.AddUObject(this, &UInputElement::OnLanguageChanged);	
}

void UInputElement::OnLanguageChanged()
{
	int32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;

	if (TextCode != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(TextCode);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		Title->SetText(NewText);
	}
}

FReply UInputElement::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	PlayAnimation(FocusAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Forward);
	OnFocusChanged.Broadcast(true);
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UInputElement::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	PlayAnimation(FocusAnim, FWidgetAssets::START_AT_BEGINNING, FWidgetAssets::SINGLE_PLAY_ANIMATION, EUMGSequencePlayMode::Reverse);
	OnFocusChanged.Broadcast(false);
}

void UInputElement::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (InputWidget)
	{
		uint8 InputIndex = StaticCast<uint8>(InputType);
		if (InputIndex < InputWidget->GetChildrenCount())
		{
			InputWidget->SetActiveWidgetIndex(InputIndex);
		}
	}
}

void UInputElement::BeginDestroy()
{
	Super::BeginDestroy();
	OnFocusChanged.Clear();
}

bool UInputElement::HandleInput(bool bInteractLeft)
{
	if (!HasKeyboardFocus())
	{
		return false;
	}
	
	switch (InputType)
	{
		case EInputElementType::SELECTOR:
			if (bInteractLeft)
			{
				Cast<USelectorInput>(GetInputWidget())->SwitchPrev();
				return true;
			}
		
			Cast<USelectorInput>(GetInputWidget())->SwitchNext();
			return true;
		
		case EInputElementType::SLIDER:
		{
			USliderInput* Slider = Cast<USliderInput>(GetInputWidget());
			
			Slider->UpdateValue(bInteractLeft);
			Slider->UpdateText();
			Slider->UpdateStep();
			return true;
		}
		case EInputElementType::TOGGLABLE:
		{
			UToggleInput* Toggle = Cast<UToggleInput>(GetInputWidget());

			if ((bInteractLeft && !Toggle->GetStatus()) || (!bInteractLeft && Toggle->GetStatus()))
			{
				Toggle->UpdateOptions();
				return true;
			}
			break;
		}
		default:
			return false;
	}
	return false;
}
