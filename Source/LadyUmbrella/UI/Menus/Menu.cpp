//  
// Menu.cpp
// 
// Implementation of the Menu class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Menu.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/UI/Basics/ModalWindow.h"
#include "LadyUmbrella/UI/Basics/SubsectionTitle.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "LadyUmbrella/UI/Basics/Input/InputElement.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/SliderInput.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/SelectorInput.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/ToggleInput.h"
#include "WorkbenchMenu/UpgradeSlot.h"

void UMenu::MenuNavigation(const bool bMoveForward)
{
	int32 NewIndex = GetNavigationIndex();

	KeyNavigation(bMoveForward, NewIndex);
	if (NewIndex != GetNavigationIndex())
	{
		SetNavigationIndex(NewIndex);
		TArray<UUserWidget*> Options = GetOptions();
		if (!Options.IsEmpty())
		{
			Options[GetNavigationIndex()]->SetFocus();
			Options[GetNavigationIndex()]->SetKeyboardFocus();
		}
		
		if (NavigationSound)
		{
			UGameplayStatics::SpawnSound2D(this, NavigationSound);
		}
	}
	OnNavigatedToOption.Broadcast();
}

UAnimatedBaseButton* UMenu::GetButtonOption(FName Identifier)
{
	for (UUserWidget* Option : GetOptions())
	{
		if (UAnimatedBaseButton* Button = Cast<UAnimatedBaseButton>(Option))
		{
			if (Identifier == Button->GetIdentifier())
			{
				return Button;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Input Element %s was unable to be accessed"), *Identifier.ToString());
	return nullptr;
}

UModalWindow* UMenu::GetModalWindow(FName Identifier)
{
	for (UUserWidget* Option : GetOptions())
	{
		if (UModalWindow* Window = Cast<UModalWindow>(Option))
		{
			if (Identifier == Window->GetIdentifier())
			{
				return Window;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Input Element %s was unable to be accessed"), *Identifier.ToString());
	return nullptr;
}

UInputElement* UMenu::GetInputOption(FName Identifier)
{
	for (UUserWidget* Option : GetOptions())
	{
		if (UInputElement* Result = Cast<UInputElement>(Option))
		{
			if (Identifier == Result->GetIdentifier())
			{
				return Result;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Input Element %s was unable to be accessed"), *Identifier.ToString());
	return nullptr;
}

USliderInput* UMenu::GetSlider(FName Identifier)
{
	UInputElement* Option = GetInputOption(Identifier);
	if (IsValid(Option) && Option->GetInputWidget()->IsA(USliderInput::StaticClass()))
	{
		return Cast<USliderInput>(Option->GetInputWidget());
	}
	UE_LOG(LogTemp, Error, TEXT("Slider %s was unable to be accessed"), *Identifier.ToString());
	return nullptr;
}

USelectorInput* UMenu::GetSelector(FName Identifier)
{
	UInputElement* Option = GetInputOption(Identifier);
	if (IsValid(Option) && Option->GetInputWidget()->IsA(USelectorInput::StaticClass()))
	{
		return Cast<USelectorInput>(Option->GetInputWidget());
	}
	UE_LOG(LogTemp, Error, TEXT("Selector %s was unable to be accessed"), *Identifier.ToString());
	return nullptr;
}

UToggleInput* UMenu::GetToggle(FName Identifier)
{
	UInputElement* Option = GetInputOption(Identifier);
	if (IsValid(Option) && Option->GetInputWidget()->IsA(UToggleInput::StaticClass()))
	{
		return Cast<UToggleInput>(Option->GetInputWidget());
	}
	UE_LOG(LogTemp, Error, TEXT("Toggle %s was unable to be accessed"), *Identifier.ToString());
	return nullptr;
}

TArray<UUserWidget*>& UMenu::GetOptions()
{
	if (!Buttons.IsEmpty())
	{
		return Buttons;
	}
	
	TArray<UUserWidget*> ExtractedOptions;
	for (UWidget* Child : MenuInputContainer->GetAllChildren())
	{
		
		if (!Child->IsA(USubsectionTitle::StaticClass()) && Child->IsA(UUserWidget::StaticClass()))
		{
			UUserWidget* Element = Cast<UUserWidget>(Child);
			ExtractedOptions.Add(Element);
		}
	}
	
	SetOptions(ExtractedOptions);
	return Buttons;
}

TArray<UUserWidget*> UMenu::GetButtonsFrom(UPanelWidget* Source)
{
	if (!IsValid(Source))
	{
		TArray<UUserWidget*> EmptyArray;
		EmptyArray.Empty();
		UE_LOG(LogTemp, Error, TEXT("Source %s was unable to be accessed"), *Source->GetName());
		return EmptyArray;
	}
	
	TArray<UUserWidget*> ExtractedOptions;
	for (UWidget* Child : Source->GetAllChildren())
	{
		if (!Child->IsA(USubsectionTitle::StaticClass()) && Child->IsA(UUserWidget::StaticClass()))
		{
			UUserWidget* Element = Cast<UUserWidget>(Child);
			ExtractedOptions.Add(Element);
		}
	}
	SetOptions(ExtractedOptions);
	return Buttons;
}

int UMenu::GetNextProbable(bool bPrevious, int& OriginalTarget, int& PredictedIndex)
{
	if (bPrevious)
	{
		PredictedIndex = PredictedIndex == 0 ? GetOptions().Num() - 1 : PredictedIndex - 1;
	}
	else
	{
		PredictedIndex = PredictedIndex == GetOptions().Num() - 1 ? 0 : PredictedIndex + 1;
	}
	PredictedIndex = FMath::Clamp(PredictedIndex, 0, GetOptions().Num() - 1);
	if (UUserWidget* Result = GetOptions()[PredictedIndex])
	{
		if (Result->GetIsEnabled() && Result->IsFocusable() || PredictedIndex == OriginalTarget)
		{
			return PredictedIndex;
		}

		OriginalTarget = OriginalTarget > 0 ? OriginalTarget : PredictedIndex;
		
		return GetNextProbable(bPrevious, OriginalTarget, PredictedIndex);
	}
	OriginalTarget = OriginalTarget > 0 ? OriginalTarget : PredictedIndex;
	return OriginalTarget;
}

void UMenu::KeyNavigation(const bool bMoveForward, int32& NewIndex)
{
	if (GetNavigationIndex() < 0.f)
	{
		NewIndex = 0;
		return;
	}
	int32 OptionIndex = GetNavigationIndex();
	int32 OriginalTarget = -1;
	NewIndex = GetNextProbable(!bMoveForward, OriginalTarget, OptionIndex);
}

ENavigationInput UMenu::IsNavigatingSomewhere(FKey Input) const
{
	for (TPair<ENavigationInput, FKeyInputCombo> Pair : NavigationInputMap)
	{
		if (Pair.Value.KeyMap.Contains(Input))
		{
			return Pair.Key;
		}
	}
	return ENavigationInput::NONE;
}

void UMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetJoyStickDeadZone(0.75f);
	SetCanNavigate(true);
	SetIsFocusable(true);
}

FReply UMenu::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D Delta =  InMouseEvent.GetCursorDelta();
	if (Delta.IsNearlyZero())
	{
		return FReply::Handled();
	}
	
	if (!GetOptions().IsEmpty() && GetNavigationIndex() >= 0)
	{
		if (UAnimatedBaseButton* Button = Cast<UAnimatedBaseButton>(GetOptions()[GetNavigationIndex()]))
		{
			const bool bPlayingAnimation = Button->IsPlayingAnimation();
			EUIState CurrentState = Button->GetState();
			if (bPlayingAnimation && CurrentState == EUIState::Pressed)
			{
				return FReply::Handled();
			}
		}
		// To be expanded if needed for alternative types
	}
	
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	int32 ProbingIndex;
	for (ProbingIndex = 0; ProbingIndex < GetOptions().Num(); ++ProbingIndex)
	{
		UUserWidget* Option = GetOptions()[ProbingIndex];

		if (!Option || !Option->IsVisible() || !Option->GetIsEnabled())
		{
			continue;
		}

		FGeometry OptionGeometry = Option->GetCachedGeometry();

		if (OptionGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
		{
			// Update index
			if (GetNavigationIndex() != ProbingIndex)
			{
				SetNavigationIndex(ProbingIndex);
			}

			Option->SetFocus();
			Option->SetKeyboardFocus();
			return FReply::Handled();
		}
	}
	if (ProbingIndex == GetOptions().Num())
	{
		SetFocus();
		SetKeyboardFocus();
		SetNavigationIndex(-1);
	}

	return FReply::Handled();
}

FReply UMenu::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	Super::NativeOnAnalogValueChanged(InGeometry, InAnalogEvent);
	
	float Value = InAnalogEvent.GetAnalogValue();
	ENavigationInput Input = IsNavigatingSomewhere(InAnalogEvent.GetKey());
	bool bNavigate = (Input == ENavigationInput::NAVIGATE_DOWN || Input == ENavigationInput::NAVIGATE_UP);
	bool bInteract = (Input == ENavigationInput::INTERACT_LEFT || Input == ENavigationInput::INTERACT_RIGHT);

	if (bNavigate)
	{
		if (FMath::Abs(Value) < GetJoyStickDeadZone())
		{
			bStickNeutralY = true;
		}
		else if (bStickNeutralY)
		{
			bStickNeutralY = false;
			MenuNavigation(Value < 0);
		}
	}
	else if (bInteract)
	{
		if (FMath::Abs(Value) < GetJoyStickDeadZone())
		{
			bStickNeutralX = true;
			bHandlingSlider = false;
		}
		else if (bStickNeutralX)
		{
			if (GetNavigationIndex() < 0)
			{
				return FReply::Handled();
			}
			bStickNeutralX = false;
			if (UInputElement* InputOption = Cast<UInputElement>(GetOptions()[GetNavigationIndex()]))
			{
				bHandlingSlider = InputOption->HandleInput(Value < 0) && InputOption->GetInputWidget()->IsA(USliderInput::StaticClass());
			}
		}
		else if (bHandlingSlider)
		{
			if (UInputElement* InputOption = Cast<UInputElement>(GetOptions()[GetNavigationIndex()]))
			{
				InputOption->HandleInput(Value < 0);
			}
		}
	}
	
	return FReply::Handled();
}