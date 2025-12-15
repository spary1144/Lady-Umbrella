
#include "HookableInterpolation.h"

#include "FMODEvent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

AHookableInterpolation::AHookableInterpolation()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(GetRootComponent());

	static auto Curve = AssetUtils::FindObject<UCurveFloat>("/Script/LadyUmbrella.RotationCurveFloat'/Game/DataStructures/Curves/Camera/C_Rotation.C_Rotation'");
	if (IsValid(Curve))
	{
		RotationCurve = Curve;
	}

	UFMODEvent* RotateEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/HOOKABLE_INTERP.HOOKABLE_INTERP'");
	FmodUtils::TryAddEvent(RotateEvent, GetFmodSoundComponentInterface());
	
}

void AHookableInterpolation::RotateFromChildDynamic(const float DegreesOfRotation, const float SpeedOfRotationChild, const FRotator& RotatorForParent)
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(RotationTimer);

	if (RotatorForParent.IsZero())
	{
		return;
	}
	
	const FQuat RotationQuat = FQuat(RotatorForParent);
	const FQuat CurrentQuat  = GetActorQuat();
	FinalRotation			 = RotationQuat * CurrentQuat;

	const FRotator CurrentRelRot = GetRootComponent()->GetRelativeRotation();
	InitialQuat					 = CurrentRelRot.Quaternion();
	const float AngleDelta		 = FMath::RadiansToDegrees(InitialQuat.AngularDistance(FinalRotation));
	
	RotationDuration = AngleDelta / SpeedOfRotationChild;
	RotationElapsed  = 0.f;
	
	if (GetFmodSoundComponentInterface())
	{
		GetFmodSoundComponentInterface()->PlayEvent();
	}
	
	TimerManager.SetTimer(
		RotationTimer,
		this,
		&AHookableInterpolation::RotateActor,
		RotateActorDynamicRate,
		true
	);
}

void AHookableInterpolation::RotateActor()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	const float DeltaTime	= World->GetDeltaSeconds();
	RotationElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(RotationElapsed / RotationDuration, 0.f, 1.f);
	float CurveValue  = Alpha;

	if (IsValid(RotationCurve))
	{
		CurveValue	= RotationCurve->GetFloatValue(Alpha);
	}
	
	const FQuat CurrentQuat = GetActorQuat();
	const FQuat NewQuat		= FQuat::Slerp(CurrentQuat, FinalRotation, CurveValue);

	RootComponent->SetRelativeRotation(NewQuat);
	if (NewQuat.Equals(FinalRotation, 0.001f))
	{
		RootComponent->SetRelativeRotation(NewQuat);
		World->GetTimerManager().ClearTimer(RotationTimer);
	}
}
