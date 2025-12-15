#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LadyUmbrella/UI/Basics/State/EOption.h"
#include "SelectorInput.generated.h"

class UOverlay;
class UImage;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionUpdated, uint8, Value);

UCLASS()
class LADYUMBRELLA_API USelectorInput : public UUserWidget
{
	GENERATED_BODY()
	
	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* CurrOptionText;

	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* PrevOption;
	
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* NextOption;
	
	// Animations
	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* SwitchOptionAnim;
	
	// Variables
	UPROPERTY(EditAnywhere, Category="Customizable Variables", meta = (AllowPrivateAccess = true))
	TArray<FString> Options;
	
	UPROPERTY(EditAnywhere, Category="Customizable Variables", meta = (AllowPrivateAccess = true))
	uint8 CurrOptionIndex;
	
	UPROPERTY(EditAnywhere, Category="Customizable Variables", meta = (AllowPrivateAccess = true, Units = "times", UIMin = 0.1f, UIMax = 10.f, ClampMin = 0.1f, ClampMax = 10.f))
	float AnimationSpeedMultiplier = 1.25f;
	
	UPROPERTY(EditAnywhere, Category = "Customizable Variables")
	USoundBase* SelectorSound;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnOptionUpdated OnOptionUpdated;

	TObjectPtr<UTextBlock> GetOptionTextBlock(EOption Target);
	FString GetOptionText(EOption Target);
	void SetOptionText(EOption Target, FString Text);
	FORCEINLINE TArray<FString> GetSelectorOptions() { return Options; }
	FORCEINLINE void SetOptions(const TArray<FString>& NewOptions) { Options = NewOptions; }
	FORCEINLINE uint8 GetCurrentOptionIndex() const { return CurrOptionIndex; }
	FORCEINLINE void SetCurrentOptionIndex(uint8 NewStart) { CurrOptionIndex = NewStart; }
	FORCEINLINE float GetAnimSpeed() { return AnimationSpeedMultiplier; }
	FORCEINLINE void SetAnimSpeed(float Speed) { AnimationSpeedMultiplier = Speed; }
	void UpdateOptions();
	UFUNCTION() void SwitchPrev();
	UFUNCTION() void SwitchNext();
	
protected: 
	virtual void NativeOnInitialized() override;
};
