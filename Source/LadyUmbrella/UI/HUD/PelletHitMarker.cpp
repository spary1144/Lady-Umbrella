// Fill out your copyright notice in the Description page of Project Settings.


#include "PelletHitMarker.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Components/CanvasPanelSlot.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

void UPelletHitMarker::NativeConstruct()
{
	Super::NativeConstruct();
	
	UPanelWidget* ParentPanel = GetParent();
	
	if (!IsValid(ParentPanel))
	{
		return;
	}
	
	if (UCanvasPanel* CanvasParent = Cast<UCanvasPanel>(ParentPanel))
	{
		HitMarkerPanel = CanvasParent;
	}
	
	BaseHitMarker = HitMarker->GetColorAndOpacity();
	HitMarker->SetVisibility(ESlateVisibility::Hidden);
}

void UPelletHitMarker::Show()
{
	FGlobalPointers::Umbrella->OnMaxPelletsPerShot.BindUObject(this, &UPelletHitMarker::UpdateHitMarkersArray);
	FGlobalPointers::Umbrella->OnPelletHit.BindUObject(this, &UPelletHitMarker::UpdateHitMarkers);
}

void UPelletHitMarker::Hide()
{
	FGlobalPointers::Umbrella->OnMaxPelletsPerShot.Unbind();
	FGlobalPointers::Umbrella->OnPelletHit.Unbind();
	SetVisibility(ESlateVisibility::Hidden);
}

void UPelletHitMarker::UpdateHitMarkersArray(const int32 MaxNrBulletPerShot)
{
	if (HitMarkerArray.Num() < MaxNrBulletPerShot)
	{
		const UCanvasPanelSlot* HitMarkerSlot = Cast<UCanvasPanelSlot>(this->Slot);
		for (int32 i = HitMarkerArray.Num(); i < MaxNrBulletPerShot; ++i)
		{
			UImage* HitMarkerCopy = DuplicateObject<UImage>(HitMarker, this);
			HitMarkerPanel->AddChild(HitMarkerCopy);
			HitMarkerCopy->SetVisibility(ESlateVisibility::Hidden);
			UCanvasPanelSlot* HitMarkerCopySlot = Cast<UCanvasPanelSlot>(HitMarkerCopy->Slot);
			HitMarkerCopySlot->SetSize(HitMarkerSlot->GetSize()); 
			HitMarkerCopySlot->SetAlignment(HitMarkerSlot->GetAlignment());
			HitMarkerCopySlot->SetPosition(HitMarkerSlot->GetPosition());
			HitMarkerArray.Add(HitMarkerCopy);
		}
	}
}

void UPelletHitMarker::UpdateHitMarkers(const bool bHasHitGenericCharacter, TSet<AGenericCharacter*> EnemiesHitByPellets) const
{
	if (!bHasHitGenericCharacter)
	{
		for (UImage* HitMarkerCopy : HitMarkerArray)
		{
			if (HitMarkerCopy->GetVisibility() != ESlateVisibility::Visible)
			{
				break;
			}
			HitMarkerCopy->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}
	for (int32 i = 0; i < EnemiesHitByPellets.Num(); ++i)
	{
		UImage* HitMarkerToShow = HitMarkerArray[i];
		AGenericCharacter* Enemy = EnemiesHitByPellets.Array()[i];
		HitMarkerToShow->SetVisibility(ESlateVisibility::Visible);
		HitMarkerToShow->SetColorAndOpacity(BaseHitMarker);
		
		if (Enemy->GetHealthComponent()->IsDead())
		{
			HitMarkerToShow->SetColorAndOpacity(HitMarkerKillColor);
		}
		FVector2D ViewportSize = FVector2D::ZeroVector;
		if (GEngine->GameViewport)
		{
			ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		}
		const FVector2D PanelSize = HitMarkerPanel->GetCachedGeometry().GetLocalSize();
		FVector2D ScreenPos;
		if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), Enemy->GetHitPositions().Last(), ScreenPos))
		{
			const FVector2D LocalPos = (ScreenPos / ViewportSize) * PanelSize;
			if (UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(HitMarkerToShow->Slot))
			{
				ImageSlot->SetPosition(LocalPos);
			}
		}
	}
}