// 
// NumericProgressBar.cpp
// 
// Implementation of the NumericProgressBar class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "LadyUmbrella/UI/Basics/ProgressBar/NumericProgressBar.h"
#include "Components/TextBlock.h"

void UNumericProgressBar::UpdateProgressBar(const float Value)
{
	Super::UpdateProgressBar(Value);

	if (bShowPercentage)
	{
		const int32 DisplayedPercent = FMath::RoundToInt(GetPercentageValue() * 100.0f);

		bool bIsUnits = DisplayedPercent < 10;
		bool bIsTens = !bIsUnits && DisplayedPercent < 100;
		bool bIsHundreds = !bIsUnits && !bIsTens;
		
		if (bIsUnits)
		{
			GetNumericValue()->SetText(FText::FromString(FString::Printf(TEXT("00%d %%"), DisplayedPercent)));
		}

		if (bIsTens)
		{
			GetNumericValue()->SetText(FText::FromString(FString::Printf(TEXT("0%d %%"), DisplayedPercent)));
		}
		
		if (bIsHundreds)
		{
			GetNumericValue()->SetText(FText::FromString(FString::Printf(TEXT("%d %%"), DisplayedPercent)));
		}
	}
	else
	{
		GetNumericValue()->SetVisibility(ESlateVisibility::Hidden);
		GetNumericValue()->SetText(FText::FromString(""));
	}
}

UTextBlock* UNumericProgressBar::GetNumericValue() const
{
	return NumericPercentValue;
}

void UNumericProgressBar::SetNumericValue(UTextBlock* NewNumericValue)
{
	NumericPercentValue = NewNumericValue;
}