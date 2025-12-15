// 
// VoiceLineDatatableRow.h
// 
// Defines the structure of the voice line datatable.  
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineState.h"

#include "VoiceLineDatatableRow.generated.h"

USTRUCT()
struct FVoiceLineDatatableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "State")
	EVoiceLineState VoiceLineState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Duration")
	float Duration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "English (United States)")
	FString EN_US;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "German")
	FString DE_DE;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Spanish (Spain)")
	FString ES_ES;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Swedish")
	FString SV_SE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Basque")
	FString EU_ES;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Catalan")
	FString CA_ES;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "French (Switzerland)")
	FString FR_CH;

	FVoiceLineDatatableRow()
	{
		VoiceLineState = EVoiceLineState::OTHER;
		Duration = 0.f;
		
		EN_US.Empty();
		DE_DE.Empty();
		ES_ES.Empty();
		SV_SE.Empty();
		EU_ES.Empty();
		CA_ES.Empty();
		FR_CH.Empty();
	}
};