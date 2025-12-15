#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToggleInput.generated.h"

class UOverlay;
class UImage;
class UButton;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleUpdated, bool, Value);

UCLASS()
class LADYUMBRELLA_API UToggleInput : public UUserWidget
{
	GENERATED_BODY()
	
	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UButton* ToggleButton;
	
	// Animations
	UPROPERTY(BlueprintReadWrite, Category = "Animation", meta = (BindWidgetAnim, AllowPrivateAccess = "true"), Transient)
	UWidgetAnimation* ToggleOptionAnim;
	
	// Variables
	UPROPERTY(EditAnywhere, Category="Customizable Variables", meta = (AllowPrivateAccess = true, Units = "times", UIMin = 0.1f, UIMax = 10.f, ClampMin = 0.1f, ClampMax = 10.f))
	float AnimationSpeedMultiplier = 1.25f;

	UPROPERTY(EditAnywhere, Category = "Customizable Variables")
	USoundBase* SelectorSound;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnToggleUpdated OnToggleUpdated;
	
	FORCEINLINE TObjectPtr<UButton> GetOnTextBlock() { return ToggleButton; }
	FORCEINLINE bool GetStatus() { return bActive; }
	FORCEINLINE void SetStatus(bool Status) { bActive = Status; }
	FORCEINLINE void Toggle() { bActive = !bActive; }
	FORCEINLINE float GetAnimSpeed() { return AnimationSpeedMultiplier; }
	FORCEINLINE void SetAnimSpeed(float Speed) { AnimationSpeedMultiplier = Speed; }
	UFUNCTION() void UpdateOptions();

protected:
	bool bActive;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
};
