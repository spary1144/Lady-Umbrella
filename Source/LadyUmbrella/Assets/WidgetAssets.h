// 
// WidgetAssets.h
// 
// Collection of pre-cached widget uassets.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NoExportTypes.h"
#include "LadyUmbrella/UI/Basics/Input/InputElement.h"
#include "LadyUmbrella/UI/Basics/Input/InputType/SelectorInput.h"
#include "LadyUmbrella/UI/Basics/State/EPlatform.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"
#include "LadyUmbrella/Util/Localization/LocalizedStringRow.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

DECLARE_MULTICAST_DELEGATE(FOnLanguageChanged);

namespace FWidgetAssets
{
	struct TabGenerationParams
	{
		TArray<FString> OutList;
	};
	
	
	inline TSubclassOf<UInputElement> InputElementTemplate;
	inline TSubclassOf<USelectorInput> InputSimpleSelectorTemplate;
	inline TSubclassOf<UUserWidget> SubsectionTitleTemplate;
	inline TSubclassOf<UUserWidget> CursorInvisible;
	
	inline UUserWidget* CursorInstance;
	
	inline UDataTable* LocalizationTable;
	inline TArray<FName> LocalizationRowNames;
	inline TArray<FLocalizedStringRow> ProcessedLocalizationRows;

	inline FOnLanguageChanged OnLanguageChanged;
	
	static TMap<EPlatform, FName> PlatformsKeys =
	{
		{EPlatform::PC, FName("KBM")},
		{EPlatform::XBOX, FName("XInputController")}
	};
	
	static const float START_AT_BEGINNING = 0.0f;
	static const int LOOP_ANIMATION = 0;
	static const int SINGLE_PLAY_ANIMATION = 1;
	static const FString EMPTY_LIST = "Empty";
	static const float STEP_GROWTH_RATE = 1.01f;
	
	static const float MINIMUM_SLIDER = 0.01f;
	static const float MAXIMUM_SLIDER = 1.f;
	
	inline void InitializeWidgetAssets()
	{
		if (!IsValid(InputElementTemplate))
			InputElementTemplate = AssetUtils::FindClass<UInputElement>(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Menu/Input/WBP_SettingsInputOption.WBP_SettingsInputOption_C'"));

		if (!IsValid(InputSimpleSelectorTemplate))
			InputSimpleSelectorTemplate = AssetUtils::FindClass<USelectorInput>(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Menu/Input/InputTypes/WBP_SelectorInput.WBP_SelectorInput_C'"));

		if (!IsValid(SubsectionTitleTemplate))
			SubsectionTitleTemplate = AssetUtils::FindClass<UUserWidget>(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Menu/Input/WBP_SubsectionTitle.WBP_SubsectionTitle_C'"));
	
		if (!IsValid(LocalizationTable))
			LocalizationTable = AssetUtils::FindObject<UDataTable>(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DT_LocalizedStrings.DT_LocalizedStrings'"));

		/*if (!IsValid(CursorInvisible))
			CursorInvisible = AssetUtils::FindClass<UUserWidget>("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/HUD/WBPHUDWidgets/WBP_InvisCursor.WBP_InvisCursor'/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/HUD/WBPHUDWidgets/WBP_InvisCursor.WBP_InvisCursor_C'");*/

		// Processed 
		if (IsValid(LocalizationTable))
		{
			LocalizationRowNames.Empty();
			ProcessedLocalizationRows.Empty();
			TArray<FLocalizedStringRow*> LocalizationRows;
			
			LocalizationTable->GetAllRows<FLocalizedStringRow>("", LocalizationRows);
			LocalizationRowNames = LocalizationTable->GetRowNames();

			for (const FLocalizedStringRow* Row : LocalizationRows)
			{
				ProcessedLocalizationRows.Add(*Row);
			}
		}

		/*
		if (IsValid(CursorInvisible))
		{
			// ???
		}*/
	}
	
	inline FString FilterByLanguageCode(const UWorld* World, const FLocalizedStringRow& Row)
	{
		if (IsValid(World))
		{
			const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(World->GetGameInstance());
			if (!IsValid(GameInstance)) return TEXT("Localized In-Game");
			
			switch (GameInstance->GetLanguageCode())
			{
			case ELanguageCode::EN_US:
				return Row.EN_US;
			case ELanguageCode::ES_ES:
				return Row.ES_ES;
			case ELanguageCode::DE_DE:
				return Row.DE_DE;
			case ELanguageCode::SV_SE:
				return Row.SV_SE;
			case ELanguageCode::EU_ES:
				return Row.EU_ES;
			case ELanguageCode::CA_ES:
				return Row.CA_ES;
			case ELanguageCode::FR_CH:
				return Row.FR_CH;
			}
		}

		return Row.EN_US;
	}
	
};
