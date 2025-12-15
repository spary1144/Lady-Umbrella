#include "GrapplingCustom.h"

AGrapplingCustom::AGrapplingCustom()
{
	SpeedOfRotationPitch = 0.f;
	SpeedOfRotationYaw	 = 0.f;
	SpeedOfRotationRoll	 = 0.f;
	DegreesOfRotation	 = 0.f;
	RotationAxis		 = FRotator(0, 0, 0);
	InterpolateRotation  = true;
}

void AGrapplingCustom::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGrapplingCustom::ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit)
{
	GetWorld()->GetTimerManager().ClearTimer(RotateTimer);

	if (IsValid(ComponentHit))
	{
		GetWorld()->GetTimerManager().SetTimer(RotateTimer, this, &AGrapplingCustom::RotateElement, AnimationTimerCallingRate, true);
	}
}

IHookInterface::GrabType AGrapplingCustom::GetGrabType()
{
	return PULLWITHCOMPONENT;
}

void AGrapplingCustom::RotateElement()
{
	auto* Root = GetRootComponent();
	if (!IsValid(Root))
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimer);
		return;
	}
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	const FRotator CurrentRotation	  = Root->GetComponentRotation();
	// Aqui hay un fallo. Habría que obtener el eje que no esté afectado por la mascara
	// tal vez el nombre no sea el mejor. Debería ser RotationToAdd en lugar de FinalRotation o MaskOfRotation
	const FRotator FinalRotation	  = DegreesOfRotation * RotationAxis;

	const float MaxDeltaPitch	= SpeedOfRotationPitch * DeltaSeconds;
	const float MaxDeltaYaw		= SpeedOfRotationYaw   * DeltaSeconds;
	const float MaxDeltaRoll		= SpeedOfRotationRoll  * DeltaSeconds;

	const float TargetPitch = RotationAxis.Pitch != 0 ? FinalRotation.Pitch : CurrentRotation.Pitch;
	const float TargetYaw	= RotationAxis.Yaw   != 0 ? FinalRotation.Yaw	  : CurrentRotation.Yaw;
	const float TargetRoll	= RotationAxis.Roll  != 0 ? FinalRotation.Roll  : CurrentRotation.Roll;
	
	const float NewPitch	= FMath::FixedTurn(CurrentRotation.Pitch, TargetPitch, MaxDeltaPitch);
	const float NewYaw		= FMath::FixedTurn(CurrentRotation.Yaw  , TargetYaw  , MaxDeltaYaw);
	const float NewRoll 	= FMath::FixedTurn(CurrentRotation.Roll , TargetRoll , MaxDeltaRoll);
	
	const FRotator NewRotation = FRotator(NewPitch, NewYaw, NewRoll);

	Root->SetRelativeRotation(NewRotation);

	const bool bDone =
			FMath::IsNearlyEqual(TargetPitch, CurrentRotation.Pitch, 1.0f) &&
			FMath::IsNearlyEqual(TargetYaw,   CurrentRotation.Yaw,   1.0f) &&
			FMath::IsNearlyEqual(TargetRoll,  CurrentRotation.Roll,  1.0f);
	
	if (bDone)
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimer);
		Root->SetRelativeRotation(NewRotation);
	}
}

