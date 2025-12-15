#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LadyUmbrella/Assets/WidgetAssets.h"
#include "SubsectionTitle.generated.h"

UCLASS()
class USubsectionTitle : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (AllowPrivateAccess = "true"))
	FName TitleCode;
	
	UPROPERTY(BlueprintReadWrite, Category = "Custom Button|Widgets", meta = (BindWidget, AllowPrivateAccess = "true"))
	UTextBlock* Title;

public:

	FText GetTitle() const { return Title->GetText(); }
	void SetTitle(const FText& Text) const { Title->SetText(Text); }
	FName GetTitleCode() const { return TitleCode; }
	void SetTitleCode(const FName Code) { TitleCode = Code; }

	UFUNCTION() void OnLanguageChanged();
	void NativeOnInitialized() override;
};

inline void USubsectionTitle::OnLanguageChanged()
{
	int32 Index = 0;
	FLocalizedStringRow Row;
	FText NewText;

	if (TitleCode != "None")
	{
		Index = FWidgetAssets::LocalizationRowNames.Find(TitleCode);
		Row = FWidgetAssets::ProcessedLocalizationRows[FMath::Clamp(Index,0,FWidgetAssets::LocalizationRowNames.Num()-1)];
		NewText = FText::FromString(FWidgetAssets::FilterByLanguageCode(GetWorld(), Row));
		Title->SetText(NewText);
	}
}

inline void USubsectionTitle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	FWidgetAssets::OnLanguageChanged.AddUObject(this, &USubsectionTitle::OnLanguageChanged);
}
