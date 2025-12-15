#include "HitReactionComponent.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "TimerManager.h"

UHitReactionComponent::UHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	HitMarkerWidgetSlotName = TEXT("GroupSlot");
	HitBorderImageName = TEXT("Image");
	RotationOffsetDegrees = -90.0f;
	MinSaturation = 0.05f;
	MaxSaturation = 1.0f;
	MaxBorderOpacity = 0.75f;
	BorderFadeSpeed = 1.0f;
	BorderFadeRate = 2.f;
	CurrentBorderOpacity = 0.0f;
	TargetBorderOpacity = 0.0f;
	CachedHealth = -1;
	OpacityThreshold = 0.001f;
	CameraRotationDegreeTolerance = 1.0f;

	ConstructorHelpers::FClassFinder<UUserWidget> BPHitMarkerWidgetClass(TEXT("/Game/Widgets/WidgetBlueprints/HUD/WBP_HitMarker"));
	if (BPHitMarkerWidgetClass.Succeeded())
	{
		HitMarkerWidgetClass = BPHitMarkerWidgetClass.Class;
	}
	ConstructorHelpers::FClassFinder<UUserWidget> BPHitBorderWidgetClass(TEXT("/Game/Widgets/WidgetBlueprints/HUD/WBP_HitBorder"));
	if (BPHitBorderWidgetClass.Succeeded())
	{
		HitBorderWidgetClass = BPHitBorderWidgetClass.Class;
	}
	ConstructorHelpers::FObjectFinder<UCurveFloat> SaturationCurveObject(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_UIDesaturationRate.C_UIDesaturationRate'"));
	if (SaturationCurveObject.Succeeded())
	{
		SaturationCurve = SaturationCurveObject.Object;
	}
	ConstructorHelpers::FObjectFinder<UMaterialInterface> SaturationMatFinder(TEXT("/Game/Materials/PostProcessMaterials/MM_SaturationEffect.MM_SaturationEffect"));
	if (SaturationMatFinder.Succeeded())
	{
		SaturationMaterialBase = SaturationMatFinder.Object;
	}
}

void UHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		HealthComponent = Owner->FindComponentByClass<UHealthComponent>();
		CachedCameraComponent = Owner->FindComponentByClass<UCameraComponent>();
	}

	if (IsValid(CachedCameraComponent) && IsValid(SaturationMaterialBase))
	{
		SaturationMID = UMaterialInstanceDynamic::Create(SaturationMaterialBase, this);
		if (IsValid(SaturationMID))
		{
			CachedCameraComponent->AddOrUpdateBlendable(SaturationMID);
		}
	}

	if (IsValid(HitBorderWidgetClass))
	{
		InitializeHitBorderWidget();
	}
}

void UHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ActiveHitMarkers.Num() > 0)
	{
		UpdateHitMarkerWidgets();
	}
	//TODO: See if we can remove or make those less dependant on the tick @PALEXUTAD
	UpdateCameraSaturation();
	if (IsValid(HitBorderImage) && !FMath::IsNearlyEqual(CurrentBorderOpacity, TargetBorderOpacity, OpacityThreshold))
	{
		CurrentBorderOpacity = FMath::FInterpTo(CurrentBorderOpacity, TargetBorderOpacity, DeltaTime, BorderFadeSpeed);
		FLinearColor NewColor = HitBorderImage->GetColorAndOpacity();
		NewColor.A = CurrentBorderOpacity;
		HitBorderImage->SetColorAndOpacity(NewColor);
	}
}

void UHitReactionComponent::OnPlayerHit()
{
	SetComponentTickEnabled(true);

	TargetBorderOpacity = MaxBorderOpacity;

	if (IsValid(HitBorderImage))
	{
		FLinearColor NewColor = HitBorderImage->GetColorAndOpacity();
		NewColor.A = TargetBorderOpacity;
		HitBorderImage->SetColorAndOpacity(NewColor);
		CurrentBorderOpacity = TargetBorderOpacity;
	}

	FTimerHandle FadeOutTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, [this]() {
		TargetBorderOpacity = 0.0f;
	}, BorderFadeRate, false);
}

void UHitReactionComponent::ShowHitMarker(const FVector& Origin)
{
	if (!IsValid(HitMarkerWidgetClass)) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		return;
	}

	OnPlayerHit();

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Recalculate camera direction cache if needed
	if (!CameraRotation.Equals(LastCameraRotation, CameraRotationDegreeTolerance))
	{
		FVector CameraForward = CameraRotation.Vector();
		FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
		CachedCameraForward2D = FVector2D(CameraForward.X, CameraForward.Y).GetSafeNormal();
		CachedCameraRight2D = FVector2D(CameraRight.X, CameraRight.Y).GetSafeNormal();
		LastCameraRotation = CameraRotation;
	}

	FVector2D FinalPosition;
	float AngleDegrees;
	CalculateHitMarkerTransform(Origin, CameraLocation, CachedCameraForward2D, CachedCameraRight2D, FinalPosition, AngleDegrees);

	if (UUserWidget* HitMarker = CreateWidget<UUserWidget>(GetWorld(), HitMarkerWidgetClass))
	{
		HitMarker->AddToViewport();
		ActiveHitMarkers.Add(FHitMarkerData(Origin, HitMarker));

		if (UObject* GroupSlotObj = HitMarker->GetWidgetFromName(HitMarkerWidgetSlotName))
		{
			if (UCanvasPanel* GroupSlot = Cast<UCanvasPanel>(GroupSlotObj))
			{
				FWidgetTransform Transform;
				Transform.Angle = AngleDegrees;
				Transform.Translation = FinalPosition;
				GroupSlot->SetRenderTransform(Transform);
			}
		}
	}
}

void UHitReactionComponent::UpdateHitMarkerWidgets()
{
	if (!IsValid(CachedCameraComponent)) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraForward = CameraRotation.Vector();
	FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	FVector2D Forward2D(CameraForward.X, CameraForward.Y);
	FVector2D Right2D(CameraRight.X, CameraRight.Y);
	for (int32 i = ActiveHitMarkers.Num() - 1; i >= 0; --i)
	{

		FHitMarkerData& MarkerData = ActiveHitMarkers[i];
		if (!IsValid(MarkerData.MarkerWidget) || !MarkerData.MarkerWidget->IsInViewport())
		{
			ActiveHitMarkers.RemoveAt(i);
			continue;
		}

		FVector2D FinalPosition;
		float AngleDegrees;
		CalculateHitMarkerTransform(MarkerData.WorldHitOrigin, CameraLocation, Forward2D, Right2D, FinalPosition, AngleDegrees);

		if (UObject* GroupSlotObj = MarkerData.MarkerWidget->GetWidgetFromName(HitMarkerWidgetSlotName))
		{
			if (UCanvasPanel* GroupSlot = Cast<UCanvasPanel>(GroupSlotObj))
			{
				FWidgetTransform Transform;
				Transform.Angle = AngleDegrees;
				Transform.Translation = FinalPosition;
				GroupSlot->SetRenderTransform(Transform);
			}
		}
	}
}

void UHitReactionComponent::UpdateCameraSaturation()
{
	if (!IsValid(HealthComponent) || !IsValid(SaturationCurve)) return;

	int32 CurrentHealth = HealthComponent->GetHealth();
	if (CurrentHealth == CachedHealth) return;

	CachedHealth = CurrentHealth;

	float Ratio = FMath::Clamp((float)CurrentHealth / (float)HealthComponent->GetMaxHealth(), 0.f, 1.f);
	float CurveValue = SaturationCurve->GetFloatValue(Ratio);
	float FinalSaturation = FMath::Lerp(MinSaturation, MaxSaturation, CurveValue);

	if (IsValid(SaturationMID))
	{
		SaturationMID->SetScalarParameterValue(TEXT("DesaturationAmount"), 1.0f - FinalSaturation);
	}
}

void UHitReactionComponent::InitializeHitBorderWidget()
{
	if (!IsValid(HitBorderWidgetClass)) return;

	HitBorderWidget = CreateWidget<UUserWidget>(GetWorld(), HitBorderWidgetClass);
	if (IsValid(HitBorderWidget))
	{
		HitBorderWidget->AddToViewport();
		SetHitBorderColorAndOpacity();
	}
}

void UHitReactionComponent::SetHitBorderColorAndOpacity()
{
	if (!IsValid(HitBorderWidget)) return;

	if (UWidget* ImageWidget = HitBorderWidget->GetWidgetFromName(HitBorderImageName))
	{
		HitBorderImage = Cast<UImage>(ImageWidget);
		if (IsValid(HitBorderImage))
		{
			FLinearColor InitialColor = HitBorderImage->GetColorAndOpacity();
			InitialColor.A = 0.0f;
			HitBorderImage->SetColorAndOpacity(InitialColor);
		}
	}
}

void UHitReactionComponent::CalculateHitMarkerTransform(const FVector& Origin, const FVector& CameraLocation,
	const FVector2D& Forward2D, const FVector2D& Right2D, FVector2D& OutPos, float& OutAngle) const
{
	FVector2D ToEnemy2D = FVector2D((Origin - CameraLocation).GetSafeNormal2D());

	float RightDot = FVector2D::DotProduct(ToEnemy2D, Right2D);
	float ForwardDot = FVector2D::DotProduct(ToEnemy2D, Forward2D);
	FVector2D ToTarget(RightDot, -ForwardDot);

	ToTarget = ToTarget.IsNearlyZero() ? FVector2D(0.f, -1.f) : ToTarget.GetSafeNormal();

	OutPos = ScreenCenter + ToTarget * WidgetCanvasSize;
	OutPos.X = FMath::Clamp(OutPos.X, -WidgetCanvasSize.X / 2.f, WidgetCanvasSize.X / 2.f);
	OutPos.Y = FMath::Clamp(OutPos.Y, -WidgetCanvasSize.Y / 2.f, WidgetCanvasSize.Y / 2.f);

	FVector2D ToCenter = (ScreenCenter - OutPos).GetSafeNormal();
	OutAngle = FMath::RadiansToDegrees(FMath::Atan2(ToCenter.Y, ToCenter.X)) + RotationOffsetDegrees;
}
