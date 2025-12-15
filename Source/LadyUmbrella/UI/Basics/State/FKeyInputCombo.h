#pragma once

#include "CoreMinimal.h"
#include "FKeyInputCombo.generated.h"

USTRUCT(BlueprintType)
struct FKeyInputCombo // Created this so that it can be hashable in editor with TMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FKey> KeyMap;

	bool operator==(const FKeyInputCombo& Other) const
	{
		return KeyMap == Other.KeyMap;
	}
};
