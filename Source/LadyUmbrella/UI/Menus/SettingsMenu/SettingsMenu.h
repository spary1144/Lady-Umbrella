// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSection.h"
#include "LadyUmbrella/UI/Basics/SubsectionTitle.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "LadyUmbrella/UI/Basics/Input/InputElement.h"
#include "LadyUmbrella/UI/Basics/State/FInputConfig.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "SettingsMenu.generated.h"

class UWidgetSwitcher;
class UDescriptionBlock;
class UTab;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API USettingsMenu : public UUserWidget, public ISaveInterface
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Container", meta = (BindWidget, AllowPrivateAccess = "true"))
	UWidgetSwitcher* MenuSwitch;
	
	UPROPERTY(BlueprintReadWrite, Category = "Container", meta = (BindWidget, AllowPrivateAccess = "true"))
	UPanelWidget* MenuInputContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Container", meta = (BindWidget, AllowPrivateAccess = "true"))
	UDescriptionBlock* DescriptionBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UTab> TabTemplate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (AllowPrivateAccess = "true"))
	USoundBase* BackToMenu;
	
	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TMap<ENavigationInput, FKeyInputCombo> NavigationInputMap;
	
	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FSectionConfig> OptionPopulationSections;
	
	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TArray<FName> Identifiers;
	
	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|Tab", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESlateSizeRule::Type> TabSizeRule = ESlateSizeRule::Fill;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|Tab", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EHorizontalAlignment> TabAlignH = HAlign_Fill;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|Tab", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EVerticalAlignment> TabAlignV = VAlign_Fill;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|Tab", meta = (AllowPrivateAccess = "true"))
	FMargin TabPadding = FMargin(0.f, 15.f);

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|InputElement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESlateSizeRule::Type> InputElementSizeRule = ESlateSizeRule::Automatic;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|InputElement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EHorizontalAlignment> InputElementAlignH = HAlign_Fill;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|InputElement", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EVerticalAlignment> InputElementAlignV = VAlign_Fill;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "Creation|InputElement", meta = (AllowPrivateAccess = "true"))
	FMargin InputElementPadding = FMargin(0.f);
	
	UPROPERTY()
	UMenu* CurrentMenu;

	UPROPERTY()
	TArray<UTab*> SectionTabs;
	
	UPROPERTY()
	TArray<FText> OptionTitles;
	
	UPROPERTY()
	TArray<FText> OptionDescriptions;


public:

	UFUNCTION(BlueprintCallable) void Show();
	UFUNCTION() void ShowCurrentDescription();
	
	ENavigationInput IsNavigatingSomewhere(FKey Input) const;

	void UpdateTitlesAndDescriptions();
	void SwitchTab(bool bLeft);
	UUserWidget* CreateSlider(FInputConfig OptionConfig, UMenuSection*& MenuInstance);
	UUserWidget* CreateToggle(FInputConfig OptionConfig, UMenuSection*& MenuInstance);
	UUserWidget* CreateSelector(FInputConfig OptionConfig, UMenuSection*& MenuInstance, bool bButtonless = false);
	USubsectionTitle* CreateSubsectionTitle(FInputConfig OptionConfig, UMenuSection*& MenuInstance);
	void PopulateSection(UMenuSection* MenuInstance, FName Key, bool bSingleButton = false);
	void PopulateTabs();

	FORCEINLINE void SetOptionPopulationSections(const TMap<FName, FSectionConfig>& NewSettings){ OptionPopulationSections = NewSettings; }
	FORCEINLINE TMap<FName, FSectionConfig>& GetOptionPopulationSections() { return OptionPopulationSections; }
	
	virtual void LoadGame() override;
	virtual void SynchronizeProperties() override;
protected:
	uint8 SectionIndex;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	bool ValidateSaveGameInstance();
	void SaveSection(UMenuSection* MenuInstance, FName Key);

	virtual void SaveGame() override;
	virtual void LoadDataCheckPoint() override;
	virtual void SaveDataCheckPoint() override;
	
};
