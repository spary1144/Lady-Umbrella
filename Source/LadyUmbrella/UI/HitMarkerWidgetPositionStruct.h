#pragma once

#include "CoreMinimal.h"
#include "HitMarkerWidgetPositionStruct.generated.h"

class UUserWidget;

USTRUCT(BlueprintType)
struct FHitMarkerData
{
    GENERATED_BODY()

    UPROPERTY()
    FVector WorldHitOrigin;

    UPROPERTY()
    UUserWidget* MarkerWidget;

    FHitMarkerData();
    FHitMarkerData(const FVector& Origin, UUserWidget* Widget);
};