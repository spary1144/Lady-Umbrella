//  
// PauseMenu.h
// 
// Pause menu interface for the main player. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "PauseMenu.generated.h"

class USettingsMenu;
class UVerticalBox;

UCLASS()
class LADYUMBRELLA_API UPauseMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ResumeButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* LoadButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* SettingsButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ExitMainMenuButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ExitDesktopButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	USoundBase* PauseOn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	USoundBase* PauseOff;
	
public:

	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Resume();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Load();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void Settings();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ExitMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ExitDesktop();
	
	FORCEINLINE UAnimatedBaseButton* GetResumeButton() const { return ResumeButton; }
	FORCEINLINE UAnimatedBaseButton* GetLoadButton() const { return LoadButton; }
	FORCEINLINE UAnimatedBaseButton* GetSettingsButton() const { return SettingsButton; }
	FORCEINLINE UAnimatedBaseButton* GetExitMainMenuButton() const { return ExitMainMenuButton; }
	FORCEINLINE UAnimatedBaseButton* GetExitDesktopButton() const { return ExitDesktopButton; }

	FORCEINLINE void SetResumeButton(UAnimatedBaseButton* NewResumeButton) { ResumeButton = NewResumeButton; }
	FORCEINLINE void SetLoadButton(UAnimatedBaseButton* NewLoadButton) { LoadButton = NewLoadButton; }
	FORCEINLINE void SetSettingsButton(UAnimatedBaseButton* NewSettingsButton) { LoadButton = NewSettingsButton; }
	FORCEINLINE void SetExitMainMenuButton(UAnimatedBaseButton* NewExitMainMenuButton) { ExitMainMenuButton = NewExitMainMenuButton; }
	FORCEINLINE void SetExitDesktopButton(UAnimatedBaseButton* NewExitDesktopButton) { ExitDesktopButton = NewExitDesktopButton; }
	
protected:

	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};