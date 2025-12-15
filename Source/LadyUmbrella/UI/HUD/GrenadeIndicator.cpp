// 
// GrenadeIndicator.cpp
// 
// Implementation of the GrenadeIndicator class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "GrenadeIndicator.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "LadyUmbrella/Weapons/Grenade/Grenade.h"
#include "Kismet/GameplayStatics.h"

void UGrenadeIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	UPanelWidget* ParentPanel = GetParent();
	if (!IsValid(ParentPanel))
	{
		return;
	}
	
	if (UCanvasPanel* CanvasParent = Cast<UCanvasPanel>(ParentPanel))
	{
		GrenadePanel = CanvasParent;
	}
	
}

void UGrenadeIndicator::Show(AGrenade* Grenade)
{
	if (!IsValid(Grenade) || BoundGrenade.IsValid())
	{
		return;
	}

	BoundGrenade = Grenade;

	Grenade->OnGrenadeThrown.BindUObject(this, &UGrenadeIndicator::UpdateGrenadeIndicator);
}

void UGrenadeIndicator::Hide()
{
	if (BoundGrenade.IsValid())
	{
		BoundGrenade->OnGrenadeThrown.Unbind();
		BoundGrenade.Reset();
	}
	SetVisibility(ESlateVisibility::Hidden);
}

void UGrenadeIndicator::UpdateGrenadeIndicator(const FVector& GrenadeLocation)
{
	if (!BoundGrenade.IsValid())
	{
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		bool IsGrenadeOnScreen = false;
		const FVector2D PanelSize = GrenadePanel->GetCachedGeometry().GetLocalSize();
		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		FVector2D GrenadeScreenLocation;
		if (PlayerController->ProjectWorldLocationToScreen(GrenadeLocation, GrenadeScreenLocation))
		{
			if (GrenadeScreenLocation.X >= 0.f && GrenadeScreenLocation.X <= ViewportSize.X &&
				GrenadeScreenLocation.Y >= 0.f && GrenadeScreenLocation.Y <= ViewportSize.Y)
			{
				IsGrenadeOnScreen = true;
			}
		}
		if (IsGrenadeOnScreen)
		{
			ShowOnScreen(GrenadeScreenLocation, ViewportSize, PanelSize);
		}
		else
		{
			ShowOffScreen(GrenadeLocation, PlayerController, PanelSize);
		}
	}
	SetVisibility(ESlateVisibility::Visible);
}

void UGrenadeIndicator::ShowOnScreen(const FVector2D Grenade2D, const FVector2D ViewportSize, const FVector2D CanvasPanelSize) const
{
	const FVector2D ScreenPosition = (Grenade2D / ViewportSize) * CanvasPanelSize;
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		CanvasSlot->SetPosition(ScreenPosition);
		CanvasSlot->SetAlignment(FVector2D(ALIGN_CENTER, ALIGN_ABOVE_GRENADE));
	}
	Indicator->SetRenderTransformAngle(RESET_ANGLE);
	GrenadeImage->SetRenderTransformAngle(RESET_ANGLE);
	Indicator->SetBrushColor(FLinearColor::Transparent);
}

void UGrenadeIndicator::ShowOffScreen(const FVector& Grenade3D, const APlayerController* PlayerController, const FVector2D CanvasPanelSize) const
{
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	const FVector Direction3D = (Grenade3D - CameraLocation).GetSafeNormal();
	const FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	const FVector Up = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);
	const float X = FVector::DotProduct(Direction3D, Right);
	const float Y = FVector::DotProduct(Direction3D, Up);
	const FVector2D Direction2D = FVector2D(X, -Y).GetSafeNormal();
	const float AngleInRadians = FMath::Atan2(Direction2D.Y, Direction2D.X);
	const float AngleInDegrees = FMath::UnwindDegrees(FMath::RadiansToDegrees(AngleInRadians) + DEGREE_TO_MATCH_IMAGE);
	const FVector2D HalfCanvas = CanvasPanelSize / 2.0f;
	float ScaleX = FLT_MAX;
	float ScaleY = FLT_MAX;
	if (Direction2D.X != 0.0f)
	{
		ScaleX = HalfCanvas.X / FMath::Abs(Direction2D.X);
	}
	if (Direction2D.Y != 0.0f)
	{
		ScaleY = HalfCanvas.Y / FMath::Abs(Direction2D.Y);
	}
	const float FinalScale = FMath::Min(ScaleX, ScaleY);
	FVector2D ScreenPosition = HalfCanvas + Direction2D * FinalScale;
	ScreenPosition.X = FMath::Clamp(ScreenPosition.X, 0.0f, CanvasPanelSize.X);
	ScreenPosition.Y = FMath::Clamp(ScreenPosition.Y, 0.0f, CanvasPanelSize.Y);
	const float AlignInX = FMath::Clamp(ALIGN_CENTER + FMath::Cos(AngleInRadians), CLAMP_MIN_OFF_SCREEN, CLAMP_MAX_OFF_SCREEN);
	const float AlignInY = FMath::Clamp(ALIGN_CENTER + FMath::Sin(AngleInRadians), CLAMP_MIN_OFF_SCREEN, CLAMP_MAX_OFF_SCREEN);
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		CanvasSlot->SetPosition(ScreenPosition);
		CanvasSlot->SetAlignment(FVector2D(AlignInX, AlignInY));
	}
	Indicator->SetRenderTransformAngle(AngleInDegrees);
	GrenadeImage->SetRenderTransformAngle(-AngleInDegrees);
	Indicator->SetBrushColor(FLinearColor::White);
}

