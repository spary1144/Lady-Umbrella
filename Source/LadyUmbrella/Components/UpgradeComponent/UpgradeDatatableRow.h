
#pragma once

#include "UpgradeType.h"

#include "UpgradeDatatableRow.generated.h"

USTRUCT()
struct FUpgradeDatatableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Type")
	EUpgradeType UpgradeType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "First")
	float First;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Second")
	float Second;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Third")
	float Third;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Fourth")
	float Fourth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Fifth")
	float Fifth;
	
	FUpgradeDatatableRow()
	{
		UpgradeType  = EUpgradeType::Other;

		First  = 1.0f;
		Second = 2.0f;
		Third  = 3.0f;
		Fourth = 4.0f;
		Fifth  = 5.0f;
	}
};
