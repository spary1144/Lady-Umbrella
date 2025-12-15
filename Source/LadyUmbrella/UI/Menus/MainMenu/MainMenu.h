//  
// MainMenu.h
// 
// Main Menu interface for the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/UI/Menus/Menu.h"
#include "MainMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);

class UVerticalBox;

UCLASS()
class LADYUMBRELLA_API UMainMenu : public UMenu
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float FadeSpeed = 1.5f;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* Fade;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* NewGameButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ContinueButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ChaptersMenuButton;

	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* SettingsButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* CreditsButton;
	
	UPROPERTY(BlueprintReadWrite, Category = "Buttons", meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnimatedBaseButton* ExitGameButton;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnButtonClicked ButtonClickedDelegate;
	
	virtual void NativeOnInitialized() override;
	
	UFUNCTION(BlueprintCallable, Category = "Visibility")
	void Show();
	
	UFUNCTION(Blueprintable, BlueprintCallable, Category = "Functionality")
	void NewGame();

	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ShowChaptersMenu();
	
	UFUNCTION(BlueprintCallable, Category = "Functionality")
	void ExitDesktop();

	FORCEINLINE UAnimatedBaseButton* GetNewGameButton() const { return NewGameButton; }
	FORCEINLINE UAnimatedBaseButton* GetContinueButton() const { return ContinueButton; }
	FORCEINLINE UAnimatedBaseButton* GetChaptersMenuButton() const { return ChaptersMenuButton; }
	FORCEINLINE UAnimatedBaseButton* GetSettingsButton() const { return SettingsButton; }
	FORCEINLINE UAnimatedBaseButton* GetCreditsButton() const { return CreditsButton; }
	FORCEINLINE UAnimatedBaseButton* GetExitGameButton() const { return ExitGameButton; }

	FORCEINLINE void SetNewGameButton(UAnimatedBaseButton* Value) { NewGameButton = Value; }
	FORCEINLINE void SetContinueButton(UAnimatedBaseButton* Value) { NewGameButton = Value; }
	FORCEINLINE void SetChapterSelectorButton(UAnimatedBaseButton* Value) { NewGameButton = Value; }
	//FORCEINLINE void SetSettingsButton(UAnimatedButton* Value) { NewGameButton = Value; }
	FORCEINLINE void SetExitGameButton(UAnimatedBaseButton* Value) { ExitGameButton = Value; }
	
protected:
	UFUNCTION() void ContinueGame();
	UFUNCTION() void ShowCredits();
	UFUNCTION() void ShowSettings();
	
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
};