#include "SliderInput.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

void USliderInput::NativeConstruct()
{
	Super::NativeConstruct();
	
	SliderWidget->OnValueChanged.AddDynamic(this, &USliderInput::SliderValueChanged);
	SetValue(FMath::Clamp(GetValue(), GetMinValue(), GetMaxValue()));
	UpdateText();
}

void USliderInput::UpdateText()
{
	int MaxPrintWidth = FString::FromInt(StaticCast<int>(GetMaxValue()*100.f)).Len();
	
	//FString ProcessedText = FString::Printf(TEXT("%0*.0f"), MaxPrintWidth, GetValue()*100.f);
	FString ProcessedText = FString::Printf(TEXT("%d"), StaticCast<int32>(GetValue()*100.f));
	SetSectionText(FText::FromString(ProcessedText));
}

void USliderInput::UpdateStep()
{
	SetStep(FMath::Clamp(GetStep() * FWidgetAssets::STEP_GROWTH_RATE, StepGrowthRange.X, StepGrowthRange.Y));
}

void USliderInput::UpdateValue(bool bLeft)
{
	float Step = GetStep();
	Step *= bLeft ? -1.f : 1.f;
	SetValue(FMath::Clamp(GetValue() + Step, GetMinValue(), GetMaxValue()));
}

void USliderInput::SliderValueChanged(float Value)
{
	OnSliderChange.Broadcast(Value);
	UpdateText();
	
	if (IsValid(SelectorSound))
	{
		UGameplayStatics::SpawnSound2D(this, SelectorSound);
	}
}
