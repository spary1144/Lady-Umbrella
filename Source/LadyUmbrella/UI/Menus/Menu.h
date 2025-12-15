//  
// Menu.h
// 
// Generic Menu with functionalities for all menus. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Basics/ModalWindow.h"
#include "LadyUmbrella/UI/Basics/State/ENavigationInput.h"
#include "LadyUmbrella/UI/Basics/State/FKeyInputCombo.h"
#include "LadyUmbrella/Util/FunctionLibrary/FrameFreezeLibrary.h"
#include "Menu.generated.h"

class UAnimatedBaseButton;
class UInputElement;
class USliderInput;
class USelectorInput;
class UToggleInput;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNavigatedTo);

UCLASS()
class LADYUMBRELLA_API UMenu : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Container", meta = (BindWidget, AllowPrivateAccess = "true"))
	UPanelWidget* MenuInputContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (AllowPrivateAccess = "true"))
	TMap<ENavigationInput, FKeyInputCombo> NavigationInputMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container", meta = (AllowPrivateAccess= "true"))
	USoundBase* NavigationSound;
	
public:
	FOnNavigatedTo OnNavigatedToOption;
	//GamepadTesting
	void MenuNavigation(const bool bMoveForward);

	UAnimatedBaseButton* GetButtonOption(FName Identifier);
	UModalWindow* GetModalWindow(FName Identifier);
	UInputElement* GetInputOption(FName Identifier);
	USliderInput* GetSlider(FName Identifier);
	USelectorInput* GetSelector(FName Identifier);
	UToggleInput* GetToggle(FName Identifier);
	TArray<UUserWidget*>& GetOptions();
	TArray<UUserWidget*> GetButtonsFrom(UPanelWidget* Source);
	
	FORCEINLINE UPanelWidget*& GetContainer() { return MenuInputContainer; }
	FORCEINLINE int32 GetNavigationIndex() const { return NavigationIndex; }
	FORCEINLINE float GetJoyStickDeadZone() const { return JoyStickDeadZone; }
	FORCEINLINE bool GetCanNavigate() const { return bCanNavigate; }
	FORCEINLINE FTimerHandle& GetStickTimer() { return ResetStickInput; }
	FORCEINLINE bool GetStickNeutralOX() const { return bStickNeutralX; }
	FORCEINLINE void SetStickNeutralOX(bool IsNeutral) { bStickNeutralX = IsNeutral; }
	FORCEINLINE void SetOptions(const TArray<UUserWidget*>& NewButtons) { Buttons = NewButtons; }
	FORCEINLINE void SetNavigationIndex(const int32 NewIndex) { NavigationIndex = NewIndex; }
	FORCEINLINE void SetJoyStickDeadZone(const float NewThreshold) { JoyStickDeadZone = NewThreshold; }
	FORCEINLINE void SetCanNavigate(const bool NewCanNavigate) { bCanNavigate = NewCanNavigate; }

	ENavigationInput IsNavigatingSomewhere(FKey Input) const;
	
	FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent);
	
private:
	
	UPROPERTY() TArray<UUserWidget*> Buttons;

	int32 NavigationIndex;
	float JoyStickDeadZone;
	bool bCanNavigate;
	FTimerHandle ResetStickInput;
	
protected:
	bool bStickNeutralX = false;
	bool bStickNeutralY = false;
	bool bHandlingSlider = false;
	
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void KeyNavigation(const bool bMoveForward, int32& NewIndex);
	int32 GetNextProbable(bool bPrevious, int32& OriginalTarget, int32& PredictedIndex);
};
