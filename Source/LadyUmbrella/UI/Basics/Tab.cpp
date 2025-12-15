#pragma once

#include "Tab.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"

FText UTab::GetText() const { return Text->GetText(); }
void UTab::SetText(FText NewText) { Text->SetText(NewText); }
void UTab::UpdateTab() const { Switcher->SetActiveWidgetIndex(bActive ? 1 : 0); }

void UTab::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	FWidgetAssets::OnLanguageChanged.AddUObject(this, &UTab::OnLanguageChanged);
	OnLanguageChanged();
}

void UTab::OnLanguageChanged()
{
	int32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;

	if (TextCode != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(TextCode);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index, 0, FWidgetAssets::LocalizationRowNames.Num() - 1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		Text->SetText(NewText);
	}
}