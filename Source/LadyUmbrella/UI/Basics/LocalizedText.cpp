#include "LocalizedText.h"

#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "LadyUmbrella/Util/Localization/LocalizedStringRow.h"

void ULocalizedText::OnLanguageChanged()
{
	int32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;

	if (TextCode != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(TextCode);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index, 0, FWidgetAssets::LocalizationRowNames.Num() - 1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		SetText(NewText);
	}
}

TSharedRef<SWidget> ULocalizedText::RebuildWidget()
{
	// Call parent to build the Slate widget first
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	// Bind once when constructed
	if (!FWidgetAssets::OnLanguageChanged.IsBoundToObject(this))
	{
		FWidgetAssets::OnLanguageChanged.AddUObject(this, &ULocalizedText::OnLanguageChanged);
	}

	// Initialize text immediately
	OnLanguageChanged();

	return Widget;
}

void ULocalizedText::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	FWidgetAssets::OnLanguageChanged.RemoveAll(this);
}
