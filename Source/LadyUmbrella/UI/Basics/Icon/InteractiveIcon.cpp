// 
// InteractiveIcon.cpp
// 
// Implementation of the InteractiveIcon class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "PlatformDependantWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/GenericInteractable.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Controllers/MainController.h"

void UInteractiveIcon::NativeConstruct()
{
	Super::NativeConstruct();

	GetCircleImage()->SetVisibility(ESlateVisibility::Hidden);
	GetPlatformKey()->SetVisibility(ESlateVisibility::Hidden);
	GetProhibitedImage()->SetVisibility(ESlateVisibility::Hidden);
	GetArrowImage()->SetVisibility(ESlateVisibility::Hidden);
}

bool UInteractiveIcon::Update(const AActor* Owner, const float OuterSphereSize, const float OuterSphereTriggerRadius,
	const float InnerSphereSize, const float InnerSphereTriggerRadius, const float AnimationSpeed,
	const bool bCanInteract, const FVector RootOffset)
{
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (!IsValid(PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid Player pointer in InteractiveIcon.Update()");
		return false;
	}

	GetCircleImage()->SetVisibility(ESlateVisibility::Hidden);
	GetPlatformKey()->SetVisibility(ESlateVisibility::Hidden);
	GetProhibitedImage()->SetVisibility(ESlateVisibility::Hidden);
	GetArrowImage()->SetVisibility(ESlateVisibility::Hidden);

	if (!bShouldShow)
	{
		return false;
	}

	if (PlayerCharacter->IsDead())
	{
		return false;
	}
	
	const float DistanceToActor = FVector::Dist(PlayerCharacter->GetActorLocation(), RootOffset);
		
	if (DistanceToActor <= InnerSphereTriggerRadius)
	{
		return ShowInnerIcon(Owner, PlayerCharacter, InnerSphereSize, AnimationSpeed, bCanInteract);
	}

	if (DistanceToActor <= OuterSphereTriggerRadius)
	{
		return ShowOuterIcon(Owner, PlayerCharacter, OuterSphereSize, AnimationSpeed);
	}
	
	return false;
}

bool UInteractiveIcon::ShowInnerIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter,
	const float InnerSphereSize, const float AnimationSpeed, const bool bCanInteract)
{
	if (CheckCollisionBeforeCamera(Owner, PlayerCharacter))
	{
		return false;
	}	
	
	TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return false;
	}
	
	const FVector OwnerWorldLocation = Cast<AGenericInteractable>(Owner)->GetWidgetComponent()->GetComponentLocation();

	if (IsIconOnScreen(PlayerController, OwnerWorldLocation) && bIsManagedByGameViewportSubsystem)
	{
		RemoveFromParent();
	}

	GetPlatformKey()->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if (!bCanInteract)
	{
		GetProhibitedImage()->SetVisibility(ESlateVisibility::Visible);
	}

	return AnimateIcon(Owner, PlayerCharacter, InnerSphereSize, AnimationSpeed);
}

bool UInteractiveIcon::ShowOuterIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter,
	const float OuterSphereSize, const float AnimationSpeed)
{
	if (CheckCollisionBeforeCamera(Owner, PlayerCharacter))
	{
		return false;
	}

	TObjectPtr<APlayerController> PlayerController = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		return false;
	}

	if (bIsManagedByGameViewportSubsystem)
	{
		RemoveFromParent();
		GetCircleImage()->SetRenderScale(FVector2D(OuterSphereSize));
	}

	const FVector OwnerWorldLocation = Cast<AGenericInteractable>(Owner)->GetWidgetComponent()->GetComponentLocation();

	if (!IsIconOnScreen(PlayerController, OwnerWorldLocation))
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
		const FVector Direction3D = (OwnerWorldLocation - CameraLocation).GetSafeNormal();
		const FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
		const FVector Up = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);
		const FVector2D Direction2D = FVector2D(FVector::DotProduct(Direction3D, Right),-FVector::DotProduct(Direction3D, Up)).GetSafeNormal();
		const float AngleInRadians = FMath::Atan2(Direction2D.Y, Direction2D.X);
		const float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians) + 90.0f;
		const FVector2D PanelSize = InteractiveIconPanel->GetCachedGeometry().GetLocalSize();
		const FVector2D HalfCanvas = PanelSize / 2.0f;
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
		FVector2D ScreenPos = HalfCanvas + Direction2D * FinalScale;
		ScreenPos.X = FMath::Clamp(ScreenPos.X, 0.0f, PanelSize.X);
		ScreenPos.Y = FMath::Clamp(ScreenPos.Y, 0.0f, PanelSize.Y);
		const float AlignX = FMath::Clamp(0.5f + FMath::Cos(AngleInRadians), MinimumOffset, MaximumOffset);
		const float AlignY = FMath::Clamp(0.5f + FMath::Sin(AngleInRadians), MinimumOffset, MaximumOffset);
		if (UCanvasPanelSlot* ArrowSlot = Cast<UCanvasPanelSlot>(GetArrowImage()->Slot))
		{
			ArrowSlot->SetPosition(ScreenPos);
			ArrowSlot->SetAlignment(FVector2D(AlignX, AlignY));
		}
		AddToViewport();
		GetArrowImage()->SetVisibility(ESlateVisibility::Visible);
		GetArrowImage()->SetRenderTransformAngle(AngleInDegrees);
		return true;
	}
	
	return AnimateIcon(Owner, PlayerCharacter, OuterSphereSize, AnimationSpeed);
}

bool UInteractiveIcon::AnimateIcon(const AActor* Owner, const APlayerCharacter* PlayerCharacter,
	const float DesiredSize, const float AnimationSpeed)
{
	GetCircleImage()->SetVisibility(ESlateVisibility::Visible);
	
	InterpolateSize = FMath::Lerp(InterpolateSize, DesiredSize, AnimationSpeed);
	GetCircleImage()->SetRenderScale(FVector2D(InterpolateSize));
	GetProhibitedImage()->SetRenderScale(FVector2D(InterpolateSize));
	GetPlatformKey()->SetRenderScale(FVector2D(InterpolateSize));

	return true;
}

bool UInteractiveIcon::IsOutsideFrustum(const AActor* Owner, const float IconFrustumMargin)
{
	const AMainController* MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	FVector2D ScreenLocation;
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	MainController->ProjectWorldLocationToScreen(Owner->GetActorLocation(), ScreenLocation);
	
	if ((ScreenLocation.X > IconFrustumMargin) &&
		(ScreenLocation.Y > IconFrustumMargin) &&
		(ScreenLocation.X < ViewportSize.X - IconFrustumMargin) &&
		(ScreenLocation.Y < ViewportSize.Y - IconFrustumMargin))
	{
		return false;
	}

	return true;
}

bool UInteractiveIcon::CheckCollisionBeforeCamera(const AActor* Owner, const APlayerCharacter* PlayerCharacter) const
{
	if (!IsValid(Owner) || !IsValid(PlayerCharacter))
	{
		FLogger::ErrorLog("Invalid pointers in UInteractiveIcon.CheckCollisionBeforeCamera()");
		return true;
	}
	
	TArray<FHitResult> Obstacles;
	FCollisionQueryParams CollisionQueryParams;

	CollisionQueryParams.AddIgnoredActor(Owner);
	CollisionQueryParams.AddIgnoredActors(Owner->Children);
	CollisionQueryParams.AddIgnoredActor(PlayerCharacter);

	FVector StartLocation = Owner->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f);

	if (bShouldOffset)
		StartLocation = TraceOffset;
	
	bool Hit = GetWorld()->LineTraceMultiByChannel(
		Obstacles,
		StartLocation,
		PlayerCharacter->GetCameraSystemComponent()->GetFollowCamera()->GetComponentLocation(),
		ECC_Camera,
		CollisionQueryParams
	);

	for (FHitResult ObstacleEntry : Obstacles)
	{
		if (!IsValid(ObstacleEntry.GetActor()))
		{
			continue;
		}

		if (ObstacleEntry.GetActor()->IsA(AGenericCharacter::StaticClass()))
		{
			return true;
		}
	}

	return Hit;
}

UImage* UInteractiveIcon::GetCircleImage() const
{
	return CircleImage;
}

UPlatformDependantWidget* UInteractiveIcon::GetPlatformKey() const
{
	return PlatformKey;
}

UImage* UInteractiveIcon::GetProhibitedImage() const
{
	return ProhibitedImage;
}

UImage* UInteractiveIcon::GetArrowImage() const
{
	return ArrowImage;
}

bool UInteractiveIcon::IsIconOnScreen(APlayerController* PlayerController, FVector WidgetWorldPos)
{

	PlayerController->GetViewportSize(ViewportWidth,ViewportHeight);
	PlayerController->ProjectWorldLocationToScreen(WidgetWorldPos, ScreenPosition);
	
	bool bIsOnScreenX = static_cast<int>(ScreenPosition.X) > 0 && ScreenPosition.X < ViewportWidth;
	bool bIsOnScreenY = static_cast<int>(ScreenPosition.Y) > 0 && ScreenPosition.Y < ViewportHeight;
	
	return (bIsOnScreenY && bIsOnScreenX);
}


