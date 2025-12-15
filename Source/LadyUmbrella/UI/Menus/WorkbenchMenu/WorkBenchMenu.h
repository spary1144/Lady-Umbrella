
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Components/UpgradeComponent/Upgrade.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeType.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "LadyUmbrella/UI/Menus/WorkbenchMenu/UpgradeSlot.h"
#include "WorkBenchMenu.generated.h"

class UWidgetSwitcher;
class UImage;
class UTextBlock;
class UUpgradeComponent;
class UDescriptionBlock;
class UScrollBox;
class UUmbrellaUpgradeInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorkBenchExit, EUmbrellaLocation, UmbrellaLocation, bool, bIsImmediate, bool, bPlayAnim);

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API UWorkBenchMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* AmountOfPlayerPieces;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UScrollBox* VB_Upgrades_Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UScrollBox* VB_Upgrades_Shotgun;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UDescriptionBlock* DescriptionBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	UImage* Shotgun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUpgradeSlot> UpgradeSlotClass;

	int32 PiecesOfThePlayer;

	UPROPERTY()
	UUpgradeComponent* UmbrellaUpgradeComponent;
	
	UPROPERTY()
	TArray<UUpgradeSlot*> UpgradeSlotArray;
	
	// IMPORTANT: If we have to add more upgrades, this value will MOST LIKELY change
	// This is used to avoid having 3 maps. This way we know how many upgrades are
	// assigned to the Shotgun and assigned to the Shield functionality
	const int32 Upgrades_Separator = 4;
	
public:
	
	void CloseMenu();
	void ReadUmbrellaUpgrades(const TMap<EUpgradeType, FUpgrade>& Upgrades);
	void SetPiecesElement(const int32 NewPieces);
	void ToggleMenuOption();
	void UpdatePrices();
	void SetUpInitialFocus();

	//void ReadUmbrellaUpgrades(const UUpgradeComponent& CurrentUpgrades);

	FORCEINLINE void SetPiecesOfThePlayer(const int32 NewPieces) { PiecesOfThePlayer = NewPieces; };
	FORCEINLINE int32 GetPiecesOfThePlayer () const { return PiecesOfThePlayer; };

	UFUNCTION() void OnUpgradePressed(const int32 UpgradePressed);

	static FOnWorkBenchExit OnWorkBenchExit;
protected:
	void ShowCurrentDescription();
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
