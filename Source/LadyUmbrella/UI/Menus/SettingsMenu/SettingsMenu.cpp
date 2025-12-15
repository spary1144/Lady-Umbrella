// Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsMenu.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/UI/Basics/Input/Button/AnimatedBaseButton.h"
#include "LadyUmbrella/UI/HUD/MainHUD.h"
#include "LadyUmbrella/UI/Menus/PauseMenu/PauseMenu.h"
#include "LadyUmbrella/UI/Menus/SettingsMenu/Section/ControlSettings.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Controllers/MainMenuController.h"
#include "LadyUmbrella/UI/Basics/DescriptionBlock.h"
#include "LadyUmbrella/UI/Basics/SubsectionTitle.h"
#include "LadyUmbrella/UI/Basics/Tab.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/SelectorInput.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/SliderInput.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/ToggleInput.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "Section/AudioSettingsSection.h"
#include "Section/ControllerSettingsSection.h"
#include "Section/VideoSettingsSection.h"

void USettingsMenu::Show()
{
	if (!IsInViewport())
	{
		AddToViewport();
	}
	SetVisibility(ESlateVisibility::Visible);
	MenuSwitch->SetActiveWidgetIndex(0);
	CurrentMenu = Cast<UMenu>(MenuSwitch->GetActiveWidget());
	SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->ToggleTab();
	SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->UpdateTab();
	CurrentMenu->SetNavigationIndex(0);
	UInputElement* CurrentElement = Cast<UInputElement>(CurrentMenu->GetOptions()[0]);
	CurrentElement->SetFocus();
	CurrentElement->SetKeyboardFocus();
	CurrentElement->OnAddedToFocusPath(FFocusEvent());
	UpdateTitlesAndDescriptions();
	ShowCurrentDescription();
}

ENavigationInput USettingsMenu::IsNavigatingSomewhere(FKey Input) const
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

void USettingsMenu::UpdateTitlesAndDescriptions()
{
	OptionTitles.Empty();
	for (UUserWidget* Option : CurrentMenu->GetOptions())
	{
		if (UInputElement* ProcessedOption = Cast<UInputElement>(Option))
		{
			OptionTitles.Add(FText::FromString(ProcessedOption->GetSectionText()));
		}
	}
}

void USettingsMenu::ShowCurrentDescription()
{
	if (!DescriptionBlock->IsVisible())
	{
		return;
	}
	
	if (!IsValid(CurrentMenu))
	{
		return;
	}
	
	if (CurrentMenu->GetOptions().IsEmpty())
	{
		return;
	}
	
	UInputElement* CurrentElement = Cast<UInputElement>(CurrentMenu->GetOptions()[CurrentMenu->GetNavigationIndex()]);
	uint32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;

	if (CurrentElement->GetTitleText() != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(CurrentElement->GetTitleText());
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		DescriptionBlock->SetTitleText(NewText);
	}

	if (CurrentElement->GetDescription() != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(CurrentElement->GetDescription());
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		DescriptionBlock->SetDescriptionText(NewText);
	}
}

void USettingsMenu::SwitchTab(bool bLeft)
{
	const int32 PreviousIndex = MenuSwitch->GetActiveWidgetIndex();
	SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->ToggleTab();
	SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->UpdateTab();
	if (bLeft)
	{
		MenuSwitch->SetActiveWidgetIndex(FMath::Max(MenuSwitch->GetActiveWidgetIndex() - 1, 0));
	}
	else
	{
		MenuSwitch->SetActiveWidgetIndex(FMath::Min(MenuSwitch->GetActiveWidgetIndex() + 1, MenuSwitch->GetNumWidgets()));
	}
	CurrentMenu = Cast<UMenu>(MenuSwitch->GetActiveWidget());
	CurrentMenu->SetNavigationIndex(0);
	if (CurrentMenu->GetOptions().IsEmpty())
	{
		int Current = MenuSwitch->GetActiveWidgetIndex();
		if (Current >= MenuSwitch->GetNumWidgets() - 1 || Current <= 0)
		{
			SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->ToggleTab();
			SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->UpdateTab();
			SwitchTab(!bLeft);
			SynchronizeProperties();
			return;
		}
		SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->ToggleTab();
		SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->UpdateTab();
		SwitchTab(bLeft);
		SynchronizeProperties();
		return;
	}
	CurrentMenu->GetOptions()[0]->SetFocus();
	CurrentMenu->GetOptions()[0]->SetKeyboardFocus();
	SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->ToggleTab();
	SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->UpdateTab();
	if (PreviousIndex != MenuSwitch->GetActiveWidgetIndex())
	{
		SynchronizeProperties();
		UpdateTitlesAndDescriptions();
		ShowCurrentDescription();
	}
}

UUserWidget* USettingsMenu::CreateSlider(FInputConfig OptionConfig, UMenuSection*& MenuInstance)
{
	if (!IsValid(FWidgetAssets::InputElementTemplate))
	{
		return nullptr;
	}
	
	UInputElement* Option = Cast<UInputElement>(
		CreateWidgetInstance(
			*MenuInstance->GetContainer(),
			FWidgetAssets::InputElementTemplate,
			FName(OptionConfig.Title.ToString() + " Slider")
		)
	);
	
	if (!IsValid(Option))
	{
		return nullptr;
	}
	Option->SetTitleText(FName(OptionConfig.Title.ToString()));
	Option->SetDescription(FName(OptionConfig.Description.ToString()));
	Option->SetInputType(EInputElementType::SLIDER);
	
	Option->OnFocusChanged.AddLambda(
[this, OptionConfig](bool bGainedFocus)
	{
		if (bGainedFocus)
		{
			ShowCurrentDescription();
		}
	});

	TObjectPtr<UWidget> a = Option->GetInputWidget();
	USliderInput* Slider = Cast<USliderInput>(a);
	Slider->SetValue(OptionConfig.InitialState);
	
	FScriptDelegate Delegate;
	Delegate.BindUFunction(MenuInstance, OptionConfig.SetterForNewValue);
	if (Delegate.IsBound())
	{
		Slider->OnSliderChange.Add(Delegate);
	}
	Slider->UpdateText();
	
	return Option;
}

UUserWidget* USettingsMenu::CreateSelector(FInputConfig OptionConfig, UMenuSection*& MenuInstance, bool bButtonless)
{
	if (!IsValid(FWidgetAssets::InputElementTemplate))
	{
		return nullptr;
	}
	if (bButtonless && !IsValid(FWidgetAssets::InputSimpleSelectorTemplate))
	{
		return nullptr;
	}
	
	if (bButtonless)
	{
		USelectorInput* Option = Cast<USelectorInput>(
			CreateWidgetInstance(
				*MenuInstance->GetContainer(),
				FWidgetAssets::InputSimpleSelectorTemplate,
				FName(OptionConfig.Title.ToString() + " Selector")
			)
		);
		Option->SetIsFocusable(true);
		Option->SetVisibility(ESlateVisibility::Visible);
		
		FWidgetAssets::TabGenerationParams Return;
		UFunction* GetterFuncList = nullptr;
		GetterFuncList = MenuInstance->FindFunction(OptionConfig.GetterFunctionForNameList);
		MenuInstance->ProcessEvent(GetterFuncList, &Return);

		if (MenuInstance->IsA(UVideoSettingsSection::StaticClass()) && Return.OutList.IsEmpty())
		{
			Return.OutList = { "Low", "Medium", "High" };
		}
		
		if (IsValid(GetterFuncList))
		{
			Option->SetOptions(Return.OutList);
			Option->SetCurrentOptionIndex(StaticCast<uint8>(OptionConfig.InitialState));
		}
	
		FScriptDelegate Delegate;
		Delegate.BindUFunction(MenuInstance, OptionConfig.SetterForNewIndex);
		if (Delegate.IsBound())
		{
			Option->OnOptionUpdated.Add(Delegate);
		}
		Option->UpdateOptions();
	
		return Option;
	}
	
	UInputElement* Option = Cast<UInputElement>(
		CreateWidgetInstance(
			*MenuInstance->GetContainer(),
			FWidgetAssets::InputElementTemplate,
			FName(OptionConfig.Title.ToString() + " Selector")
		)
	);
	
	TArray<FString> ReturnList;
	UFunction* GetterFuncList = MenuInstance->FindFunction(OptionConfig.GetterFunctionForNameList);
	MenuInstance->ProcessEvent(GetterFuncList, &ReturnList);

	if (MenuInstance->IsA(UVideoSettingsSection::StaticClass()) && ReturnList.IsEmpty())
	{
		ReturnList = { "Low", "Medium", "High" };
	}

	Option->SetTitleText(FName(OptionConfig.Title.ToString()));
	Option->SetDescription(FName(OptionConfig.Description.ToString()));
	Option->SetInputType(EInputElementType::SELECTOR);

	Option->OnFocusChanged.AddLambda(
[this, OptionConfig](bool bGainedFocus)
	{
		if (bGainedFocus)
		{
			ShowCurrentDescription();
		}
	});

	USelectorInput* Selector = Cast<USelectorInput>(Option->GetInputWidget());
	
	if (IsValid(GetterFuncList))
	{
		Selector->SetOptions(ReturnList);
		Selector->SetCurrentOptionIndex(StaticCast<uint8>(OptionConfig.InitialState));
	}
	
	FScriptDelegate Delegate;
	Delegate.BindUFunction(MenuInstance, OptionConfig.SetterForNewIndex);
	if (Delegate.IsBound())
	{
		Selector->OnOptionUpdated.Add(Delegate);
	}
	Selector->UpdateOptions();
	
	return Option;
}

UUserWidget* USettingsMenu::CreateToggle(FInputConfig OptionConfig, UMenuSection*& MenuInstance)
{
	if (!IsValid(FWidgetAssets::InputElementTemplate))
	{
		return nullptr;
	}
	
	UInputElement* Option = Cast<UInputElement>(
		CreateWidgetInstance(
			*MenuInstance->GetContainer(),
			FWidgetAssets::InputElementTemplate,
			FName(OptionConfig.Title.ToString() + " Toggle")
		)
	);	

	Option->SetTitleText(FName(OptionConfig.Title.ToString()));
	Option->SetDescription(FName(OptionConfig.Description.ToString()));
	Option->SetInputType(EInputElementType::TOGGLABLE);

	Option->OnFocusChanged.AddLambda(
	[this, OptionConfig](bool bGainedFocus)
	{
		if (bGainedFocus)
		{
			ShowCurrentDescription();
		}
	});
	
	UToggleInput* Toggle = Cast<UToggleInput>(Option->GetInputWidget());
	Toggle->SetStatus(StaticCast<bool>(OptionConfig.InitialState));
	
	FScriptDelegate Delegate;
	Delegate.BindUFunction(MenuInstance, OptionConfig.SetterForActivation);
	if (Delegate.IsBound())
	{
		Toggle->OnToggleUpdated.Add(Delegate);
	}
	Toggle->UpdateOptions();

	return Option;
}

USubsectionTitle* USettingsMenu::CreateSubsectionTitle(FInputConfig OptionConfig, UMenuSection*& MenuInstance)
{
	if (!IsValid(FWidgetAssets::SubsectionTitleTemplate))
	{
		return nullptr;
	}
	USubsectionTitle* Option = Cast<USubsectionTitle>(
		CreateWidgetInstance(
			*MenuInstance->GetContainer(),
			FWidgetAssets::SubsectionTitleTemplate,
			FName(OptionConfig.Title.ToString() + " Title")
		)
	);
	Option->SetTitle(OptionConfig.Title);
	Option->SetTitleCode(FName(OptionConfig.Title.ToString()));
	return Option;
}


void USettingsMenu::PopulateSection(UMenuSection* MenuInstance, FName Key, bool bSingleButton)
{
	MenuInstance->GetContainer()->ClearChildren();
	MenuInstance->GetOptions().Empty();
	if (!OptionPopulationSections.Contains(Key))
	{
		return;
	}
	TArray<FInputConfig> InputConfigsForKey = OptionPopulationSections[Key].InputConfigs;
	for (int32 Index = 0; Index < InputConfigsForKey.Num(); Index++)
	{
		FInputConfig CurrentOptionConfig = InputConfigsForKey[Index];
		UUserWidget* Option = nullptr;
		switch (CurrentOptionConfig.InputType)
		{
		case EInputElementType::SELECTOR :
			Option = CreateSelector(CurrentOptionConfig, MenuInstance, Key == FName("Controls"));
			break;
		case EInputElementType::SLIDER :
			Option = CreateSlider(CurrentOptionConfig, MenuInstance);
			break;
		case EInputElementType::TOGGLABLE :
			Option = CreateToggle(CurrentOptionConfig, MenuInstance);
			break;
		case EInputElementType::OTHER:
			Option = CreateSubsectionTitle(CurrentOptionConfig, MenuInstance);
			break;
		}
		if (IsValid(Option))
		{
			UPanelSlot* CurrSlot = MenuInstance->GetContainer()->AddChild(Option);
			if (UOverlaySlot* ProcessedOlySlot = Cast<UOverlaySlot>(CurrSlot))
			{
				ProcessedOlySlot->SetPadding(FMargin(0));
				ProcessedOlySlot->SetHorizontalAlignment(HAlign_Fill);
				ProcessedOlySlot->SetVerticalAlignment(VAlign_Fill);
			}
			else if (UScrollBoxSlot* ProcessedScBxSlot = Cast<UScrollBoxSlot>(CurrSlot))
			{
				ProcessedScBxSlot->SetSize(FSlateChildSize(InputElementSizeRule));
				ProcessedScBxSlot->SetHorizontalAlignment(InputElementAlignH);
				ProcessedScBxSlot->SetVerticalAlignment(InputElementAlignV);
				ProcessedScBxSlot->SetPadding(InputElementPadding);
			}
			if (bSingleButton)
			{
				return;
			}
		}
	}
}

void USettingsMenu::PopulateTabs()
{
	for (int32 Index = 0; Index < MenuSwitch->GetNumWidgets(); Index++)
	{
		if (UMenuSection* SectionWidget = Cast<UMenuSection>(MenuSwitch->GetWidgetAtIndex(Index)))
		{
			FName Id = (Identifiers[Identifiers.Find(SectionWidget->GetIdentifier())]);
			if (Id == FName("Controls"))
			{
				PopulateSection(SectionWidget, Id, true);
				continue;
			}
			
			PopulateSection(SectionWidget, Id);
		}
	}
}

void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();
	MenuInputContainer->ClearChildren();
	SectionTabs.Empty();
	for (int32 Index = 0; Index < MenuSwitch->GetNumWidgets(); Index++)
	{
		if (UMenuSection* SectionWidget = Cast<UMenuSection>(MenuSwitch->GetWidgetAtIndex(Index)))
		{
			UTab* SectionTab = Cast<UTab>(CreateWidgetInstance(*MenuInputContainer, TabTemplate, FName(SectionWidget->GetIdentifier().ToString() + " Tab"))); 
			UHorizontalBoxSlot* CurrSlot = Cast<UHorizontalBoxSlot>(MenuInputContainer->AddChild(SectionTab));

			if (IsValid(CurrSlot))
			{
				CurrSlot->SetSize(FSlateChildSize(TabSizeRule));
				CurrSlot->SetHorizontalAlignment(TabAlignH);
				CurrSlot->SetVerticalAlignment(TabAlignV);
				CurrSlot->SetPadding(TabPadding);
			}
			
			SectionTab->GetTextWidget()->SetText(FText::FromString(SectionWidget->GetIdentifier().ToString()));
			SectionTab->SetTextCode(FName(SectionWidget->GetIdentifier().ToString()));
			if (SectionTab->IsActive())
			{
				SectionTab->ToggleTab();
				SectionTab->UpdateTab();
			}
			SectionTabs.Add(SectionTab);
			SectionWidget->OnNavigatedToOption.AddDynamic(this, &USettingsMenu::ShowCurrentDescription);
		}
	}

	FWidgetAssets::OnLanguageChanged.AddUObject(this, &USettingsMenu::ShowCurrentDescription);
}

FReply USettingsMenu::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
	
	switch (IsNavigatingSomewhere(InKeyEvent.GetKey()))
	{
	case ENavigationInput::NAVIGATE_UP :
		{
			const int32 PreviousIndex = CurrentMenu->GetNavigationIndex();
			CurrentMenu->MenuNavigation(false);
			if (PreviousIndex != CurrentMenu->GetNavigationIndex())
			{
				ShowCurrentDescription();
			}
			return FReply::Handled();
		}
	case ENavigationInput::NAVIGATE_DOWN :
		{
			const int32 PreviousIndex = CurrentMenu->GetNavigationIndex();
			CurrentMenu->MenuNavigation(true);
			if (PreviousIndex != CurrentMenu->GetNavigationIndex())
			{
				ShowCurrentDescription();
			}
			return FReply::Handled();
		}
	case ENavigationInput::INTERACT_RIGHT :
		if (CurrentMenu->GetNavigationIndex() < 0)
		{
			return FReply::Handled();
		}
		if (UInputElement* Input = Cast<UInputElement>(CurrentMenu->GetOptions()[CurrentMenu->GetNavigationIndex()]))
		{
			if (Input->HandleInput(false))
			{
				return FReply::Handled();
			}
		}
		return FReply::Unhandled();
	case ENavigationInput::INTERACT_LEFT :
		if (CurrentMenu->GetNavigationIndex() < 0)
		{
			return FReply::Handled();
		}
		if (UInputElement* Input = Cast<UInputElement>(CurrentMenu->GetOptions()[CurrentMenu->GetNavigationIndex()]))
		{
			if (Input->HandleInput(true))
			{
				return FReply::Handled();
			}
		}
		return FReply::Unhandled();
	case ENavigationInput::SWITCH_LEFT_TAB :
		{
			SwitchTab(true);
			return FReply::Handled();
		}
	case ENavigationInput::SWITCH_RIGHT_TAB :
		{
			SwitchTab(false);
			return FReply::Handled();
		}
			
	case ENavigationInput::EXIT :
		SaveGame();
		if (IsValid(FGlobalPointers::MainController))
		{
			SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->ToggleTab();
			SectionTabs[MenuSwitch->GetActiveWidgetIndex()]->UpdateTab();

			if (BackToMenu)
			{
				UGameplayStatics::SpawnSound2D(this, BackToMenu);
			}
			
			const int32 CurrentPauseMenuIndex = FGlobalPointers::MainController->GetMainHUD()->GetPauseMenuInstance()->GetNavigationIndex();

			FGlobalPointers::MainController->GetMainHUD()->GetSettingsMenuInstance()->SetVisibility(ESlateVisibility::Collapsed);
			FGlobalPointers::MainController->GetMainHUD()->GetPauseMenuInstance()->SetVisibility(ESlateVisibility::Visible);
			FGlobalPointers::MainController->GetMainHUD()->GetPauseMenuInstance()->GetOptions()[CurrentPauseMenuIndex]->SetFocus();
			FGlobalPointers::MainController->GetMainHUD()->GetPauseMenuInstance()->GetOptions()[CurrentPauseMenuIndex]->SetKeyboardFocus();
			return FReply::Handled();
		}
		
		if (AMainMenuController* Controller = Cast<AMainMenuController>(GetWorld()->GetFirstPlayerController()))
		{
			Controller->GetMainMenu()->Show();
			SetVisibility(ESlateVisibility::Collapsed);
			return FReply::Handled();
		}
		return FReply::Unhandled();
		
	default:
		return FReply::Unhandled();
	}
	
}

FReply USettingsMenu::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (CurrentMenu->GetNavigationIndex() >= 0)
	{
		if (UInputElement* Input = Cast<UInputElement>(CurrentMenu->GetOptions()[CurrentMenu->GetNavigationIndex()]))
		{
			if (Input->GetInputType() == EInputElementType::SLIDER)
			{
				USliderInput* Slider = Cast<USliderInput>(Input->GetInputWidget());
				Slider->SetStep(Slider->GetMaxStepGrowth().X);
			}
		}
	}
	
	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

bool USettingsMenu::ValidateSaveGameInstance()
{
	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
	if (!IsValid(GameInstance))
	{
		return false;
	}

	ULU_SaveGame* SaveGame = GameInstance->GetSaveGameFile();
	if (!IsValid(SaveGame))
	{
		return false;
	}
	return true;
}

void USettingsMenu::SaveSection(UMenuSection* MenuInstance, FName Key)
{
	if (!OptionPopulationSections.Contains(Key))
	{
		return;
	}
	
	FSectionConfig Config = OptionPopulationSections[Key];
	TArray<UUserWidget*> Options = MenuInstance->GetOptions();
	if (Options.IsEmpty())
	{
		return;
	}
	int32 OptionIndex = 0;
	for(int32 ElementIndex = 0; ElementIndex < Config.InputConfigs.Num(); ElementIndex++)
	{
		FInputConfig* CurrentOptionConfig = &OptionPopulationSections[Key].InputConfigs[ElementIndex];
		if (CurrentOptionConfig->InputType == EInputElementType::OTHER)
		{
			continue;
		}
		if (UInputElement* Element = Cast<UInputElement>(Options[OptionIndex])){
			switch (Element->GetInputType())
				{
				case EInputElementType::SELECTOR :
					CurrentOptionConfig->InitialState = StaticCast<float>(Cast<USelectorInput>(Element->GetInputWidget())->GetCurrentOptionIndex());
					break;
				case EInputElementType::SLIDER :
					CurrentOptionConfig->InitialState = Cast<USliderInput>(Element->GetInputWidget())->GetValue();
					break;
				case EInputElementType::TOGGLABLE :
					CurrentOptionConfig->InitialState = StaticCast<float>(Cast<UToggleInput>(Element->GetInputWidget())->GetStatus());
					break;
				default:
					break;
			}
			OptionIndex++;
		}
	}
}

void USettingsMenu::SaveGame()
{
	if (!ValidateSaveGameInstance())
	{
		return;
	}

	ULU_SaveGame* SaveGame = Cast<ULU_GameInstance>(GetGameInstance())->GetSaveGameFile();

	for (int32 Index = 0; Index < MenuSwitch->GetNumWidgets(); Index++)
	{
		if (UMenuSection* SectionWidget = Cast<UMenuSection>(MenuSwitch->GetWidgetAtIndex(Index)))
		{
			switch (Identifiers.Find(SectionWidget->GetIdentifier()))
			{
			case 0:
				SaveSection(SectionWidget, FName("Gameplay"));
				break;
			case 1:
				SaveSection(SectionWidget, FName("Video"));
				break;
			case 2:
				SaveSection(SectionWidget, FName("Audio"));
				break;
			case 3:
				SaveSection(SectionWidget, FName("Controls"));
				break;
			default:
				FLogger::ErrorLog("Unexpected Widget Identifier");
				break;
			}
		}
	}
	
	SaveGame->SetOptionPopulationSections(GetOptionPopulationSections());
}

void USettingsMenu::LoadGame()
{
	if (!ValidateSaveGameInstance())
	{
		return;
	}
	ULU_SaveGame* SavedGame = Cast<ULU_GameInstance>(GetGameInstance())->GetSaveGameFile();

	if (SavedGame->GetOptionPopulationSections().IsEmpty())
	{
		PopulateTabs();
		SaveGame();
		return;
	}
	
	if (SavedGame->GetOptionPopulationSections().Num() != GetOptionPopulationSections().Num())
	{
		PopulateTabs();
		SaveGame();
		return;
	}

	for (const TPair<FName, FSectionConfig>& PairLocal : GetOptionPopulationSections())
	{
		const FName& Key = PairLocal.Key;
		const FSectionConfig* SectionSaved = SavedGame->GetOptionPopulationSections().Find(Key);
		if (!SectionSaved)
		{
			PopulateTabs();
			SaveGame();
			return; 
		}

		const FSectionConfig& SectionLocal= PairLocal.Value;
		if (SectionLocal.InputConfigs.Num() != SectionSaved->InputConfigs.Num())
		{
			PopulateTabs();
			SaveGame();
			return; 
		}
		for (int32 Index = 0; Index < SectionLocal.InputConfigs.Num(); Index++)
		{
			if (!SectionLocal.InputConfigs[Index].Title.EqualTo(SectionSaved->InputConfigs[Index].Title))
			{
				PopulateTabs();
				SaveGame();
				return; 
			}
		}
	}
	
	SetOptionPopulationSections(SavedGame->GetOptionPopulationSections());
	// If main menu / main menu controller
	PopulateTabs();
}

void USettingsMenu::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (!IsValid(DescriptionBlock) || !IsValid(MenuSwitch))
	{
		return;
	}
	DescriptionBlock->SetVisibility(MenuSwitch->ActiveWidgetIndex > 2 ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void USettingsMenu::LoadDataCheckPoint()
{
	LoadGame();
}

void USettingsMenu::SaveDataCheckPoint()
{
	SaveGame();
}
