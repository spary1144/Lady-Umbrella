// 
// MainMenuController.h
// 
// Controller for the main menu. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LadyUmbrella/UI/Menus/MainMenu/MainMenu.h"
#include "MainMenuController.generated.h"

class UMainMenu;
class USettingsMenu;
class UChapterSelectorMenu;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AMainMenuController : public APlayerController
{
	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMainMenu> MainMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UChapterSelectorMenu> ChapterSelectorMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USettingsMenu> SettingsMenuWidget;
	
	UPROPERTY()
	UMainMenu* MainMenuInstance;

	UPROPERTY()
	UChapterSelectorMenu* ChapterSelectorMenuInstance;
	
	UPROPERTY()
	USettingsMenu* SettingsMenuInstance;

public:
	
	FORCEINLINE UMainMenu* GetMainMenu() const { return MainMenuInstance; }
	
	FORCEINLINE UChapterSelectorMenu* GetChapterSelectorMenu() const { return ChapterSelectorMenuInstance; }
	FORCEINLINE USettingsMenu* GetSettingsMenu() const { return SettingsMenuInstance; }

protected:
	
	virtual void BeginPlay() override;
	
};
