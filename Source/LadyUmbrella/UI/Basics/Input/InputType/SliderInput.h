#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "SliderInput.generated.h"

class UOverlay;
class UImage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSliderChange, float, Value);

UCLASS()
class LADYUMBRELLA_API USliderInput : public UUserWidget
{
	GENERATED_BODY()
	
	// Bindings
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	USlider* SliderWidget;
		
	UPROPERTY(BlueprintReadWrite, Category = "Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* ValueText;

	// Variables
	UPROPERTY(EditAnywhere, Category="Customizable Variables", meta = (AllowPrivateAccess = true))
	FVector2D StepGrowthRange;
	
	UPROPERTY(EditAnywhere, Category = "Customizable Variables")
	USoundBase* SelectorSound;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnSliderChange OnSliderChange;
	
	FORCEINLINE TObjectPtr<UTextBlock> GetTextBlock() const { return ValueText; }
	FORCEINLINE FString GetText() const { return ValueText->GetText().ToString(); }
	FORCEINLINE void SetSectionText(const FText& NewText) const { ValueText->SetText(NewText); }
	FORCEINLINE float GetStep() const { return SliderWidget->GetStepSize(); }
	FORCEINLINE void SetStep(float NewValue) const { SliderWidget->SetStepSize(NewValue); }
	FORCEINLINE float GetValue() const { return SliderWidget->GetValue(); }
	FORCEINLINE void SetValue(float NewValue) const { SliderWidget->SetValue(NewValue); }
	FORCEINLINE float GetMinValue() const { return SliderWidget->GetMinValue(); }
	FORCEINLINE void SetMinValue(float NewValue) const { SliderWidget->SetMinValue(NewValue); }
	FORCEINLINE float GetMaxValue() const { return SliderWidget->GetMaxValue(); }
	FORCEINLINE void SetMaxValue(float NewValue) const { SliderWidget->SetMaxValue(NewValue); }
	FORCEINLINE FVector2D GetMaxStepGrowth() const { return StepGrowthRange; }
	FORCEINLINE void SetMaxStepGrowth(const FVector2D& NewValue) { StepGrowthRange = NewValue; }
	void UpdateText();
	void UpdateStep();
	void UpdateValue(bool bLeft);

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void SliderValueChanged(float Value);
};
