// 
// Crosshair.cpp
// 
// Implementation of the Crosshair class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "Crosshair.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Components/CanvasPanelSlot.h"
#include "Animation/WidgetAnimation.h"

void UCrosshair::NativeConstruct()
{
	Super::NativeConstruct();
	
	UPanelWidget* ParentPanel = GetParent();
	
	if (!IsValid(ParentPanel))
	{
		return;
	}
	
	if (UCanvasPanel* CanvasParent = Cast<UCanvasPanel>(ParentPanel))
	{
		CrosshairPanel = CanvasParent;
	}
	
	BaseCrosshairColor = Crosshair->GetColorAndOpacity();
}

void UCrosshair::Show()
{
	if (bIsLeftBorder || bIsRightBorder)
	{
		FGlobalPointers::Umbrella->OnUmbrellaRangeAndSpread.AddUObject(this, &UCrosshair::UpdateCrosshairSize);
		FGlobalPointers::Umbrella->OnCanShoot.AddUObject(this, &UCrosshair::UpdateCrosshairAnimation);
		FGlobalPointers::Umbrella->OnShootingCadence.AddUObject(this, &UCrosshair::UpdateCrosshairCadence);
	}
	
	FGlobalPointers::Umbrella->OnUmbrellaStateChanged.AddUObject(this, &UCrosshair::UpdateCrosshairState);
}

void UCrosshair::Hide()
{
	if (bIsLeftBorder || bIsRightBorder)
	{
		FGlobalPointers::Umbrella->OnUmbrellaRangeAndSpread.RemoveAll(this);
		FGlobalPointers::Umbrella->OnCanShoot.RemoveAll(this);
		FGlobalPointers::Umbrella->OnShootingCadence.RemoveAll(this);
	}
	
	FGlobalPointers::Umbrella->OnUmbrellaStateChanged.RemoveAll(this);
	SetVisibility(ESlateVisibility::Hidden);
}

void UCrosshair::UpdateCrosshairSize(const float BulletRange, const float SpreadAngleX, const float SpreadAngleZ) const
{
	const float OffsetX = FMath::Tan(FMath::DegreesToRadians(SpreadAngleX)) * BulletRange + 20.f;
	const float SizeY = ANGLE_TO_FULL_SPAN * FMath::Tan(FMath::DegreesToRadians(SpreadAngleZ)) * BulletRange;
	const float SizeX = SizeY;

	if (bIsLeftBorder && !bIsRightBorder)
	{
		if (UCanvasPanelSlot* LeftSlot = Cast<UCanvasPanelSlot>(this->Slot))
		{
			LeftSlot->SetPosition(FVector2D(-OffsetX, CROSSHAIR_VERTICAL_CENTER));
			Crosshair->SetRenderScale(FVector2D(SizeX / 100,SizeY / 100));
		}
	}

	if (!bIsLeftBorder && bIsRightBorder)
	{
		if (UCanvasPanelSlot* RightSlot = Cast<UCanvasPanelSlot>(this->Slot))
		{
			RightSlot->SetPosition(FVector2D(OffsetX, CROSSHAIR_VERTICAL_CENTER));
			Crosshair->SetRenderScale(FVector2D(SizeX / 100,SizeY / 100));
		}
	}
}

void UCrosshair::UpdateCrosshairAnimation(const bool bIsShooting) const
{
	if (!OwnerWidget->IsAnimationPlaying(ShootingCadenceAnim) && bIsShooting)
	{
		OwnerWidget->PlayAnimationReverse(ShootingCadenceAnim);
	} 
	else
	{
		if (OwnerWidget->IsAnimationPlaying(ShootingCadenceAnim) && !bIsShooting)
		{
			OwnerWidget->StopAnimation(ShootingCadenceAnim);
		}
	}
}

void UCrosshair::UpdateCrosshairCadence(const FTimerHandle ShootingTimer) const
{
	const float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(ShootingTimer);
	const float Duration = GetWorld()->GetTimerManager().GetTimerRate(ShootingTimer);

	if (Duration <= 0.0f)
	{
		return;
	}
	
	const float Alpha = Elapsed / Duration;
	
	const float TargetTime = FMath::Lerp(
	ShootingCadenceAnim->GetEndTime(),
	ShootingCadenceAnim->GetStartTime(),
	Alpha
	);
	
	OwnerWidget->SetAnimationCurrentTime(ShootingCadenceAnim, TargetTime);
}

void UCrosshair::UpdateCrosshairState(const bool IsAiming, const bool IsShieldOpen, const bool IsOnEnemy, const bool IsOnHookable)
{
	if (IsAiming && !IsShieldOpen)
	{
		SetVisibility(ESlateVisibility::Visible);
		
		if (IsOnEnemy)
		{
			Crosshair->SetColorAndOpacity(OnEnemyCrosshairColor);
			return;
		}

		if (IsOnHookable)
		{
			Crosshair->SetColorAndOpacity(FLinearColor::Blue);
			return;
		}

		Crosshair->SetColorAndOpacity(BaseCrosshairColor);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCrosshair::SetAnimationOwner(UUserWidget* InOwner, UWidgetAnimation* InAnim)
{
	OwnerWidget = InOwner;
	ShootingCadenceAnim = InAnim;
}