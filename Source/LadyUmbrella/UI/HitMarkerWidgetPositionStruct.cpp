#include "HitMarkerWidgetPositionStruct.h"
#include "Blueprint/UserWidget.h"

FHitMarkerData::FHitMarkerData()
    : WorldHitOrigin(FVector::ZeroVector)
    , MarkerWidget(nullptr)
{
}

FHitMarkerData::FHitMarkerData(const FVector& Origin, UUserWidget* Widget)
    : WorldHitOrigin(Origin)
    , MarkerWidget(Widget)
{
}