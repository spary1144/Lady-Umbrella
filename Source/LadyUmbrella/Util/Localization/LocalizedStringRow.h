
#pragma once

#include "LocalizedStringRow.generated.h"

USTRUCT()
struct FLocalizedStringRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

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
	
	FLocalizedStringRow()
	{
		EN_US.Empty();
		DE_DE.Empty();
		ES_ES.Empty();
		SV_SE.Empty();
		EU_ES.Empty();
		CA_ES.Empty();
		FR_CH.Empty();
	}
};