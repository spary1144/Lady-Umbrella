//  
// GenericProgressBar.cpp
// 
// Implementation of the GenericProgressBar class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "GenericProgressBar.h"
#include "Components/ProgressBar.h"

UProgressBar* UGenericProgressBar::GetGenericProgressBar() const
{
	return ProgressBar;
}

float UGenericProgressBar::GetMaximumValue() const
{
	return MaximumValue;
}

float UGenericProgressBar::GetPercentageValue() const
{
	return Percentage;
}

void UGenericProgressBar::SetGenericProgressBar(UProgressBar* NewProgressBar)
{
	ProgressBar = NewProgressBar;
}

void UGenericProgressBar::SetMaximumValue(const float Value)
{
	MaximumValue = Value;
}

void UGenericProgressBar::SetPercentageValue(const float Value)
{
	Percentage = Value;
}

void UGenericProgressBar::UpdateProgressBar(const float Value) 
{
	SetPercentageValue(Value / GetMaximumValue());
	GetGenericProgressBar()->SetPercent(GetPercentageValue());
}