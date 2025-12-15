// 
// AnimatedButton.h
// 
// Animated Button for UI. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LadyUmbrella/UI/Basics/State/UIState.h"
#include "AnimatedBaseButton.generated.h"

class UButton;
class UImage;
class UOverlay;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonActivated);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFocusChanged, bool);

UCLASS()
class LADYUMBRELLA_API UAnimatedBaseButton : public UUserWidget
{
	GENERATED_BODY()
protected:

	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* Button;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* ButtonName;
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Customizable Button Values", meta = (AllowPrivateAccess = true))
	FText Text;
	
	UPROPERTY(EditAnywhere, Category="Customizable Button Values", meta = (AllowPrivateAccess = true))
	EUIState State = EUIState::Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customizable Button Values", meta = (AllowPrivateAccess = "true"))
	FName ButtonIdentifier;
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnButtonActivated OnButtonActivated;
	FOnFocusChanged OnFocusChanged;

	TObjectPtr<UButton> GetButton() const { return Button; };
	FORCEINLINE FName GetIdentifier() const { return ButtonIdentifier; };
	FORCEINLINE void SetIdentifier(FName Code) { ButtonIdentifier = Code; };
	FString GetText() const { return ButtonName->GetText().ToString(); }
	void SetButtonText(const FText& NewText) { ButtonName->SetText(NewText); }
	FORCEINLINE void SetTextVar(const FText& NewText) { Text = NewText; }
	FORCEINLINE EUIState GetState() const { return State; }
	FORCEINLINE void SetState(EUIState NewState) { State = NewState; }

protected:

	UPROPERTY()
	bool bBrushesSwapped = false;

	UFUNCTION() void OnLanguageChanged();
	virtual void NativeOnInitialized() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	
	UFUNCTION()
	virtual void ActivateButton();
};
