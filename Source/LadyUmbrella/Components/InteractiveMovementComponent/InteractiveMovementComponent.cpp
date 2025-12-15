// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractiveMovementComponent.h"

#include "EngineUtils.h"
#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Interfaces/InteractiveMovement.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/UmbrellaLocation.h"

// Sets default values for this component's properties
UInteractiveMovementComponent::UInteractiveMovementComponent()
	: MantleSphereTraceOffset(85.f)
	, StickToWallDistance(150.f)
	, MantleDistance(50.f)
	, MantleAnimPlayRate(1)
	, MantleCameraVerticalMovement(240.f)
	, MantleTraceFanMinAngle(-45.f)
	, MantleTraceFanMaxAngle(45.f)
	, MantleTraceFanStep(15.f)
	, CheckVaultDistance(100.f)
	, MinVaultableLength(25.f)
	, VaultOverHeight(100.f)
	, VaultAnimPlayRate(1)
	, ShortVaultAnimPlayRate(1)
	, LongVaultAnimPlayRate(0.4f)
	, MaxVaultDistance(100.f)
	, LongVaultDistance(300)
	, ShortVaultDistance(100.f)
	, VaultLandTraceLenght(400.1f)
	, SurfaceNormalZValue(1)
	, KickedBoxTraceExtent(200, 200.f, 100.f)
	, StepAnimPlayRate(1)
	, StepCheckDistance(150)
	, StepLandVerticalOffset(10.f)
	, SwingAnimationPlayRate(1.f)
	, CheckSwingDistance(300.f)
	, SwingTraceVerticalOffset(-80.f)
	, FallInitialHeight(0)
	, MinHeightToRoll(800.f)
	, RollMontagePlayRate(1.f)
	, VerticalHookMovementPlayRate(1.f)
	, SphereTraceRadius(10.f)
	, SphereTraceVerticalOffset(FVector(0, 0, 50))
	, SphereTraceLength(100.f)
	, SphereTraceStep(25)
	, MinimumLandSpace(80)
	, bShortVault(false)
	, bIsVoidVault(false)
{
	bInZipline           = false;
	bStrafing            = false;
	bIsApplyingJumpBoost = false;
	JumpBoostDistance    = 50.0f;
	JumpBoostRemaining   = 0.0f;
	JumpBoostSpeed       = 200.0f;
	MaxWalkSpeed         = 500.0f;
	OpenShieldWalkSpeed  = 150.0f;
	StrafeWalkSpeed      = 300.f;

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractiveMovementComponent::SetInZipline(const bool bValue)
{
	bInZipline = bValue;
	EUmbrellaLocation UmbrellaLocation = bValue ? EUmbrellaLocation::UmbrellaCenter : EUmbrellaLocation::Back;
	OnZiplineStateChanged.ExecuteIfBound(UmbrellaLocation);
}

// Called when the game starts
void UInteractiveMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	GenericCharacter = Cast<AGenericCharacter>(GetOwner());
	if (GetOwner()->IsA(APlayerCharacter::StaticClass()))
	{
		Player = FGlobalPointers::PlayerCharacter;
	}
	
	for (UActorComponent* Component : GetOwner()->GetComponents())
	{
		if (Component->IsA(UCapsuleComponent::StaticClass()))
		{
			Capsule = Cast<UCapsuleComponent>(Component);
		}
		else if (Component->IsA(USkeletalMeshComponent::StaticClass()))
		{
			SkeletalMesh = Cast<USkeletalMeshComponent>(Component);
		}
		else if (Component->IsA(UCharacterMovementComponent::StaticClass()))
		{
			CharacterMovement = Cast<UCharacterMovementComponent>(Component);
		}
		else if (Component->IsA(UMotionWarpingComponent::StaticClass()))
		{
			MotionWarping = Cast<UMotionWarpingComponent>(Component);
		}
	}

	AnimInstance = SkeletalMesh->GetAnimInstance();
}

// +-------------------------------------------------+
// |                                                 |
// |					 Mantle                      |
// |                                                 |
// +-------------------------------------------------+

void UInteractiveMovementComponent::CalculateBestAngleToMantle(const TArray<AActor*>& IgnoredActors, const int& i, float& BestAngleToMantle) const
{
	for (float Angle = MantleTraceFanMinAngle; Angle <= MantleTraceFanMaxAngle; Angle += MantleTraceFanStep)
	{
		FHitResult Hit;
		FVector RotatedForward = GenericCharacter->GetActorForwardVector().RotateAngleAxis(Angle, FVector::UpVector);
		const FVector FanLineTraceStart = GenericCharacter->GetActorLocation() + RotatedForward * (i + MantleDistance) + GenericCharacter->GetActorUpVector() * MantleSphereTraceOffset;
		const FVector FanLineTraceEnd = FanLineTraceStart - GenericCharacter->GetActorUpVector() * STICK_TO_WALL_LINE_TRACE_LENGTH;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), FanLineTraceStart, FanLineTraceEnd, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, Hit, true, FColor::Blue))
		{
			if (Hit.bStartPenetrating)
			{
				continue;
			}
			
			if (FMath::Abs(Angle) < FMath::Abs(BestAngleToMantle))
			{
				BestAngleToMantle = Angle;
			}

			if (Angle == 0 || Angle > 0)
			{
				return;
			}
		}
	}
}

void UInteractiveMovementComponent::RotatePlayerToFaceWall(const TArray<AActor*>& IgnoredActors, const int& i, const float& BestAngleToMantle)
{
	FHitResult HitResult;
	const FVector AngleLineTraceStart = GenericCharacter->GetActorLocation() - GenericCharacter->GetActorUpVector() * Capsule->GetScaledCapsuleHalfHeight();
	const FVector AngleLineTraceEnd = AngleLineTraceStart + GenericCharacter->GetActorForwardVector().RotateAngleAxis(BestAngleToMantle, FVector::UpVector) * (i + MantleDistance);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), AngleLineTraceStart, AngleLineTraceEnd, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, HitResult, true, FColor::Blue))
	{
		FVector TargetForward = -HitResult.ImpactNormal;

		FRotator TargetRotation = TargetForward.Rotation();

		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;
		GenericCharacter->SetActorRotation(TargetRotation);
	}
}

void UInteractiveMovementComponent::MantleCameraDetach()
{
	TWeakObjectPtr<USpringArmComponent> SpringArmComponent = FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->GetCameraBoom();
	if (SpringArmComponent.IsValid())
	{
		SpringArmComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		CameraTargetPos = MantleStartPos + GenericCharacter->GetActorForwardVector() * GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() + GenericCharacter->GetActorUpVector() * MantleCameraVerticalMovement;
		FLatentActionInfo LatentInfoCamera;
		LatentInfoCamera.CallbackTarget = this;
		LatentInfoCamera.ExecutionFunction = FName("CameraMoveToTargetFinished");
		LatentInfoCamera.Linkage = 0;
		LatentInfoCamera.UUID = __LINE__;
		UKismetSystemLibrary::MoveComponentTo(SpringArmComponent.Get(), CameraTargetPos, SpringArmComponent->GetComponentRotation(), false, false, 0.7, true, EMoveComponentAction::Type::Move, LatentInfoCamera);
	}
}

bool UInteractiveMovementComponent::TryToMantle()
{
	if (!IsValid(Capsule) || !IsValid(SkeletalMesh) || !IsValid(CharacterMovement) || !IsValid(GenericCharacter) || IsInZipline())
	{
		return false;
	}
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	IgnoredActors.Add(FGlobalPointers::Umbrella);
	
	//float SphereTraceRadius = 10;
	//FHitResult OutHitSphere;
	//const FVector SphereTraceStart = GenericCharacter->GetActorLocation() + GenericCharacter->GetActorUpVector() * MantleSphereTraceOffset;
	//const FVector SphereTraceEnd = SphereTraceStart + GenericCharacter->GetActorForwardVector() * StickToWallDistance;
	//const bool bSphereTrace = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHitSphere, true);
	
	FHitResult OutHit;
	float BestAngleToMantle = 0;
	const float AuxAngle = MantleTraceFanMinAngle - MantleTraceFanStep;
	float StickToWallFinalDistance = 0;
	
	for (int i = 0; i < StickToWallDistance; i += STICK_TO_WALL_LINE_TRACE_STEP)
	{
		BestAngleToMantle = AuxAngle;
		CalculateBestAngleToMantle(IgnoredActors, i, BestAngleToMantle);
		StickToWallFinalDistance = i;
		
		const FVector LineTraceStart = GenericCharacter->GetActorLocation() + GenericCharacter->GetActorForwardVector().RotateAngleAxis(BestAngleToMantle, FVector::UpVector) * (i + MantleDistance) + GenericCharacter->GetActorUpVector() * MantleSphereTraceOffset;
		const FVector LineTraceEnd = LineTraceStart - GenericCharacter->GetActorUpVector() * STICK_TO_WALL_LINE_TRACE_LENGTH;
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true, FColor::Yellow))
		{
			break;
		}
	}

	if (IsValid(OutHit.GetActor()) && OutHit.GetActor()->Tags.Contains("NoClimb"))
	{
		return false;
	}
	
	AActor* ManagingActor = OutHit.HitObjectHandle.GetManagingActor();
	if (!IsValid(ManagingActor) || OutHit.bStartPenetrating || ManagingActor->Implements<UInteractiveMovement>())
	{
		return false;
	}

	//if (bSphereTrace && OutHitSphere.Distance < (i + MantleDistance))
	//{
	//	return;
	//}

	if (!DoesHaveEnoughRoomToMantle(OutHit.Location, IgnoredActors, BestAngleToMantle))
	{
		return false;
	}
	
	if (BestAngleToMantle == AuxAngle)
	{
		return false;
	}
	
	// Setting the Player State
	if (IsValid(Player) && Player->CanChangeToPlayerState(EPlayerState::InTraversalNoControls))
	{
		Player->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
	}
	
	RotatePlayerToFaceWall(IgnoredActors, StickToWallFinalDistance, BestAngleToMantle);
	
	OnMantleStarted.ExecuteIfBound(EUmbrellaLocation::Back);

	MantleStartPos = FVector(
		(OutHit.Location - GenericCharacter->GetActorForwardVector() * Capsule->GetScaledCapsuleRadius()).X,
		(OutHit.Location - GenericCharacter->GetActorForwardVector() * Capsule->GetScaledCapsuleRadius()).Y,
		OutHit.Location.Z - Capsule->GetScaledCapsuleHalfHeight() + MANTLE_CAPSULE_TARGET_LOCATION_OFFSET);

	MantleHandPos = OutHit.Location;

	if (IsValid(CharacterMovement))
	{
		CharacterMovement->SetMovementMode(MOVE_Flying);
	}
	
	MantleCameraDetach();
	
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("MoveToTargetFinished");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;
	
	UKismetSystemLibrary::MoveComponentTo(Capsule, MantleStartPos, GenericCharacter->GetActorRotation(), false, false, 0.1, true, EMoveComponentAction::Type::Move, LatentInfo);
	return true;
}

void UInteractiveMovementComponent::MoveToTargetFinished()
{
	Mantle();
}

void UInteractiveMovementComponent::CameraMoveToTargetFinished()
{
	if (IsValid(AnimInstance) && IsValid(MantleMontage))
	{
		if (IsValid(Player))
		{
			Player->ResetPlayerStateToFreeMovementAndCheckAim();
		}
		AnimInstance->Montage_Stop(0.25, MantleMontage);
	}

	TWeakObjectPtr<USpringArmComponent> SpringArmComponent = FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->GetCameraBoom();
	if (SpringArmComponent.IsValid())
	{
		SpringArmComponent->AttachToComponent(FGlobalPointers::PlayerCharacter->GetCapsuleComponent(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));

		SpringArmComponent->SetRelativeLocation(FVector(0.f,0.f,FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->GetRelativeTopOfCapsule()));
		FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->UpdateForAttachment(CameraTargetPos);
	}
}

bool UInteractiveMovementComponent::DoesHaveEnoughRoomToMantle(const FVector& OutHitLocation, const TArray<AActor*>& IgnoredActors, const float& BestAngleToMantle) const
{
	FHitResult OutHitSphereLandPosition;
	FVector SphereTraceStart = OutHitLocation - GenericCharacter->GetActorForwardVector().RotateAngleAxis(BestAngleToMantle, FVector::UpVector) * (Capsule->GetScaledCapsuleRadius() + STICK_TO_WALL_LINE_TRACE_STEP);
	FVector SphereTraceEnd = SphereTraceStart - GenericCharacter->GetActorUpVector() * Capsule->GetScaledCapsuleHalfHeight();

	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, Capsule->GetScaledCapsuleRadius(), TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHitSphereLandPosition, true))
	{
		//FLogger::DebugLog("Not enough room for the climb ledge start");
		return false;
	}

	// Check that the player has enough space at the destination location
	SphereTraceStart = OutHitLocation + GenericCharacter->GetActorForwardVector().RotateAngleAxis(BestAngleToMantle, FVector::UpVector) * (Capsule->GetScaledCapsuleRadius() / 2) + GenericCharacter->GetActorUpVector() * (Capsule->GetScaledCapsuleRadius() + MANTLE_LAND_LOCATION_OFFSET);
	SphereTraceEnd = SphereTraceStart + GenericCharacter->GetActorUpVector() * Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere() * 2;

	bool SphereTraceSingle = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, Capsule->GetScaledCapsuleRadius(), TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHitSphereLandPosition, true);

	if (SphereTraceSingle || OutHitSphereLandPosition.bStartPenetrating)
	{
		//FLogger::DebugLog("Not enough room for the climb ledge end");
		return false;
	}

	return true;
}

void UInteractiveMovementComponent::Mantle()
{
	if (IsValid(CharacterMovement))
	{
		CharacterMovement->SetMovementMode(MOVE_Flying);
	}

	FMotionWarpingTarget WarpingTarget;
	WarpingTarget.Location = MantleHandPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "MantleStart";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = MantleHandPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "MantleMiddle";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = MantleHandPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "MantleEnd";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);


	if (IsValid(AnimInstance) && IsValid(MantleMontage))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnMantleCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
		AnimInstance->Montage_Play(MantleMontage, MantleAnimPlayRate);
	}
}

void UInteractiveMovementComponent::TryToRoll()
{
	if (!IsValid(Player))
	{
		return;
	}
	//FLogger::DebugLog("FallInitialHeight: %f ---- FinalLocation: %f", FallInitialHeight, GenericCharacter->GetActorLocation().Z);
	if (FallInitialHeight - GenericCharacter->GetActorLocation().Z > MinHeightToRoll - 10)
	{
		if (IsValid(AnimInstance) && IsValid(RollMontage))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnRollCompleted);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
			AnimInstance->Montage_Play(RollMontage, RollMontagePlayRate);

			Player->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
		}
	}
}

// +-------------------------------------------------+
// |                                                 |
// |					 Swing                       |
// |                                                 |
// +-------------------------------------------------+

bool UInteractiveMovementComponent::CheckSwing() const
{
	if (!IsValid(Capsule) || !IsValid(SkeletalMesh) || !IsValid(CharacterMovement) || !IsValid(GenericCharacter))
	{
		return false;
	}

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	IgnoredActors.Add(FGlobalPointers::Umbrella);
	
	TArray<FHitResult> OutHits;

	FVector SphereTraceStart = GenericCharacter->GetActorLocation() + GenericCharacter->GetActorUpVector() * SwingTraceVerticalOffset;
	FVector SphereTraceEnd = SphereTraceStart + GenericCharacter->GetActorForwardVector() * CheckSwingDistance;
	const bool bMultiSphereTrace = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2), false, IgnoredActors, EDrawDebugTrace::None, OutHits, true);

	if (!bMultiSphereTrace)
	{
		return false;
	}

	for (FHitResult& HitActor : OutHits)
	{
		if (AActor* Actor = HitActor.GetActor())
		{
			if (!Actor->Implements<UInteractiveMovement>())
			{
				continue;
			}
			if (IInteractiveMovement* InterfacePtr = Cast<IInteractiveMovement>(Actor))
			{
				if (InterfacePtr->Interact(GenericCharacter))
				{
					return true;
				}
			}
		}
	}

	return false;
}

// +-------------------------------------------------+
// |                                                 |
// |					 Vault                       |
// |                                                 |
// +-------------------------------------------------+

bool UInteractiveMovementComponent::CheckVault()
{
	if (!IsValid(Capsule) || !IsValid(SkeletalMesh) || !IsValid(CharacterMovement) || !IsValid(MotionWarping) || !IsValid(GenericCharacter))
	{
		return false;
	}

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	IgnoredActors.Add(FGlobalPointers::Umbrella);
	
	FHitResult OutHit;

	const FVector LineTraceStart = GenericCharacter->GetActorLocation() - GenericCharacter->GetActorUpVector() * VAULT_OVER_MARGIN;
	const FVector LineTraceEnd = LineTraceStart + GenericCharacter->GetActorForwardVector() * CheckVaultDistance;
	bool bLineTrace = UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true);

	if (!OutHit.HasValidHitObjectHandle() || OutHit.bStartPenetrating || !bLineTrace || IsValid(OutHit.GetActor()) && OutHit.GetActor()->Tags.Contains("NoVault"))
	{
		return false;
	}

	FVector Origin;
	FVector BoxExtent;
	OutHit.GetActor()->GetActorBounds(true, Origin, BoxExtent);
	if (BoxExtent.Z * 2 > VaultOverHeight && !FMath::IsNearlyEqual(BoxExtent.Z * 2, VaultOverHeight, VAULT_OVER_MARGIN) && !OutHit.GetActor()->Tags.Contains("Vaultable"))
	{
		return false;
	}

	if (!(OutHit.bStartPenetrating) && bLineTrace)
	{
		bool bCouldVault = SetUpVault(OutHit.ImpactPoint, IgnoredActors);

		if (bCouldVault)
		{
			OnVaultDelegate.Broadcast();
			return true;
		}
	}
	return false;
}

bool UInteractiveMovementComponent::SetUpVault(const FVector& StartVaultLocation, TArray<AActor*> IgnoredActors)
{
	PrepareIgnoredActors(IgnoredActors);

	int LastIndex = 0;
	bool bCanLand = false;

	for (int Index = 0; Index <= (MaxVaultDistance + SphereTraceStep); Index += SphereTraceStep)
	{
		if (!HandleVaultTrace(StartVaultLocation, IgnoredActors, Index, LastIndex, bCanLand))
		{
			break;
		}
	}

	return FinalizeVault(LastIndex, StartVaultLocation, bCanLand);
}

void UInteractiveMovementComponent::PrepareIgnoredActors(TArray<AActor*>& IgnoredActors) const
{
	for (TActorIterator<AGenericCharacter> It(GetWorld()); It; ++It)
	{
		IgnoredActors.Add(*It);
	}
}

bool UInteractiveMovementComponent::HandleVaultTrace(const FVector& StartVaultLocation, const TArray<AActor*>& IgnoredActors, const int iIndex, int& LastIndex, bool& bCanLand)
{
	const FVector SphereTraceStart = StartVaultLocation + GenericCharacter->GetActorForwardVector() * iIndex + SphereTraceVerticalOffset;
	const FVector SphereTraceEnd = SphereTraceStart - (GenericCharacter->GetActorUpVector() * SphereTraceLength);
	FHitResult OutHit;

	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStart, SphereTraceEnd, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true))
	{
		return ProcessSphereTraceHit(OutHit, iIndex, LastIndex, bCanLand);
	}

	return ProcessVaultLanding(SphereTraceStart, OutHit, IgnoredActors, bCanLand);
}

bool UInteractiveMovementComponent::ProcessSphereTraceHit(const FHitResult& OutHit, const int Index, int& LastIndex, bool& bCanLand)
{
	LastIndex = Index;

	if (OutHit.bStartPenetrating)
	{
		bCanWarp = false;
		return false;
	}

	if (Index == 0)
	{
		if (OutHit.ImpactNormal.Z < SurfaceNormalZValue && !FMath::IsNearlyEqual(OutHit.ImpactNormal.Z, SurfaceNormalZValue, .001f))
		{
			bCanLand = false;
			return false;
		}

		VaultStartPos = OutHit.Location;
		VaultMiddlePos = VaultStartPos;
	}
	else
	{
		VaultMiddlePos = OutHit.ImpactPoint;
	}

	bCanWarp = true;

	return true;
}

bool UInteractiveMovementComponent::ProcessVaultLanding(const FVector& SphereTraceStart, const FHitResult& OutHit, const TArray<AActor*>& IgnoredActors, bool& bCanLand)
{
	DetectKickableActor(OutHit, SphereTraceStart, IgnoredActors);
	return !CheckLandingLocation(IgnoredActors, bCanLand);
}

void UInteractiveMovementComponent::DetectKickableActor(const FHitResult& OutHit, const FVector& SphereTraceStart, const TArray<AActor*>& IgnoredActors)
{
	FVector BoxTraceStart = OutHit.TraceStart + (GenericCharacter->GetActorForwardVector() * MinimumLandSpace);
	FVector BoxTraceEnd = SphereTraceStart + GenericCharacter->GetActorForwardVector() * KICK_BOX_TRACE_LENGTH;
	TArray<FHitResult> OutHits;

	if (!UKismetSystemLibrary::BoxTraceMulti(GetWorld(), BoxTraceStart, BoxTraceEnd, KickedBoxTraceExtent, GenericCharacter->GetActorRotation(), ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::None, OutHits, true))
	{
		return;
	}

	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UKickInterface>())
		{
			if (IKickInterface* KickInterfacePtr = Cast<IKickInterface>(HitActor))
			{
				KickInterfacePtr->PrepareKnockBackActor();
			}
			KickedActor = HitActor;
			KickHit = Hit;
			VaultMiddlePos = KickedActor->GetActorLocation();
			break;
		}
	}
}


bool UInteractiveMovementComponent::CheckLandingLocation(const TArray<AActor*>& IgnoredActors, bool& bCanLand)
{
	FVector LineTraceStart;
	if (IsValid(KickedActor))
	{
		LineTraceStart = KickedActor->GetActorLocation();
	}
	else
	{
		LineTraceStart = VaultMiddlePos + GenericCharacter->GetActorForwardVector() * MinimumLandSpace;
	}
	
	const FVector LineTraceEnd = LineTraceStart - GenericCharacter->GetActorUpVector() * VaultLandTraceLenght;
	FHitResult LandHit;

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, LandHit, true))
	{
		//if (!IsValid(KickedActor))
		//{
		//	// Check if the player has enough room to land
		//	const FVector CapsuleTraceStart = VaultMiddlePos + GenericCharacter->GetActorForwardVector() * Capsule->GetScaledCapsuleRadius() * 2;;
		//	const FVector CapsuleTraceEnd = CapsuleTraceStart - GenericCharacter->GetActorUpVector() * GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
		//	FHitResult CapsuleHit;
//
		//	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), CapsuleTraceStart, CapsuleTraceEnd, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() / 2, TraceTypeQuery1, false, IgnoredActors, EDrawDebugTrace::None, CapsuleHit, true))
		//	{
		//		bCanLand = false;
		//		return false;
		//	}
		//}
		
		VaultLandPos = LandHit.ImpactPoint;
		const float HeightDiff = FMath::Abs(LandHit.Location.Z - VaultMiddlePos.Z) - VaultOverHeight; 
		bIsVoidVault = !FMath::IsNearlyEqual(HeightDiff, KINDA_SMALL_NUMBER, VOID_VAULT_ERROR_TOLERANCE);
		if (IsValid(KickedActor))
		{
			bIsVoidVault = false;
		}
		bCanLand = true;
		return true;
	}

	bCanWarp = false;
	return false;
}


bool UInteractiveMovementComponent::TryToDoStep(const int LastIndex, const FVector& StartVaultLocation)
{
	if ((LastIndex >= MinimumLandSpace) && FVector::Distance(StartVaultLocation, GenericCharacter->GetActorLocation()) < StepCheckDistance)
	{
		// Setting the Player State
		if (IsValid(Player) && Player->CanChangeToPlayerState(EPlayerState::InTraversalNoControls))
		{
			Player->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
		}
			
		bCanWarp = true;
		StepStartPos = StartVaultLocation - GenericCharacter->GetActorUpVector() * StepLandVerticalOffset;
		StepLandPos = StepStartPos + GenericCharacter->GetActorForwardVector() * SphereTraceStep;
		return StepMotionWarp();
	}
	return false;
}

bool UInteractiveMovementComponent::FinalizeVault(const int LastIndex, const FVector& StartVaultLocation, const bool bCanLand)
{
	if (LastIndex < MinVaultableLength)
	{
		return false;
	}

	bShortVault = LastIndex <= ShortVaultDistance;
	bLongVault = LastIndex >= LongVaultDistance;

	if (!bCanLand)
	{
		TryToDoStep(LastIndex, StartVaultLocation);
		return false;
	}

	// Setting the Player State
	if (IsValid(Player) && Player->CanChangeToPlayerState(EPlayerState::InTraversalNoControls))
	{
		Player->SetPlayerGeneralState(EPlayerState::InTraversalNoControls);
		const UCustomCameraSystemComponent* CameraSystem = Player->GetCameraSystemComponent();
		if (IsValid(CameraSystem))
		{
			float UpdateRadius = Player->GetCameraSystemComponent()->GetProbeRadius() / CAM_RADIUS_MODIFIER;
			Player->GetCameraSystemComponent()->SetProbeRadius(UpdateRadius);
		}
	}
		
	return VaultMotionWarp();
}


bool UInteractiveMovementComponent::VaultMotionWarp()
{
	if (!bCanWarp)
	{
		return false;
	}

	CharacterMovement->SetMovementMode(MOVE_Flying);

	FMotionWarpingTarget WarpingTarget;
	WarpingTarget.Location = VaultStartPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "VaultStart";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = VaultMiddlePos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "VaultMiddle";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = VaultLandPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "VaultLand";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);
	
	if (bShortVault)
	{
		if (IsValid(AnimInstance) && IsValid(ShortVaultMontage))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnShortVaultCompleted);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
			AnimInstance->Montage_Play(ShortVaultMontage, ShortVaultAnimPlayRate);
			CurrentMontage = ShortVaultMontage;
		}
	}
	else
	{
		if (IsValid(AnimInstance) && IsValid(VaultMontage))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnVaultCompleted);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
			AnimInstance->Montage_Play(VaultMontage, VaultAnimPlayRate);
			CurrentMontage = VaultMontage;
		}
	}
	
	return true;
}

// +-------------------------------------------------+
// |                                                 |
// |					 Step                        |
// |                                                 |
// +-------------------------------------------------+

bool UInteractiveMovementComponent::StepMotionWarp()
{
	if (!bCanWarp)
	{
		return false;
	}

	CharacterMovement->SetMovementMode(MOVE_Flying, 0);

	FMotionWarpingTarget WarpingTarget;
	WarpingTarget.Location = StepStartPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "StepStart";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	WarpingTarget.Location = StepLandPos;
	WarpingTarget.Rotation = GenericCharacter->GetActorRotation();
	WarpingTarget.Name = "StepLand";
	MotionWarping->AddOrUpdateWarpTarget(WarpingTarget);

	if (IsValid(AnimInstance) && IsValid(StepMontage))
	{
		CurrentMontage = StepMontage;
		AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractiveMovementComponent::OnStepCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
		AnimInstance->Montage_Play(StepMontage, StepAnimPlayRate);
	}
	return true;
}

// +-------------------------------------------------+
// |                                                 |
// |			  Motion Warp Notifies               |
// |                                                 |
// +-------------------------------------------------+

void UInteractiveMovementComponent::OnMotionWarpStarted(FName NotifyName)
{
	if (NotifyName == "VaultMiddle")
	{
		if (bLongVault)
		{
			if (IsValid(AnimInstance) && IsValid(VaultMontage))
			{
				AnimInstance->Montage_SetPlayRate(VaultMontage, LongVaultAnimPlayRate);
			}
		}
	}
	
	if (NotifyName == "VaultLand")
    {
    	if (bLongVault)
    	{
    		if (IsValid(AnimInstance) && IsValid(VaultMontage))
    		{
    			AnimInstance->Montage_SetPlayRate(VaultMontage, VaultAnimPlayRate);
    		}
    	}
    }
}

void UInteractiveMovementComponent::KickCharacterDuringVault()
{
	IKickInterface* KickInterfacePtr = Cast<IKickInterface>(KickedActor);

	//DrawDebugSphere(GetWorld(), VaultMiddlePos, 10, 12, FColor::Orange, false, 2, 0U, 2);

	const FVector OwnerLocation = GenericCharacter->GetActorLocation();
	if (KickInterfacePtr)
	{
		KickInterfacePtr->KickActor(((OwnerLocation + GenericCharacter->GetActorForwardVector() * KICK_LAUNCH_MAGNITUDE) - OwnerLocation).GetSafeNormal());
		//DrawDebugLine(GetWorld(), Owner->GetActorLocation() /*- FVector(0,0, 80)*/, OwnerLocation + Owner->GetActorForwardVector() * 100, FColor::Green, false, 2.0f, 0, 2.0f);
	}

	KickedActor = nullptr;

	if (IsValid(Player) && Player->GetPlayerGeneralState() == EPlayerState::InTraversalNoControls)
	{
		Player->ResetPlayerStateToFreeMovementAndCheckAim();
	}
}

void UInteractiveMovementComponent::OnMotionWarpFinished(FName NotifyName)
{

}

void UInteractiveMovementComponent::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "Kick")
	{
		if (IsValid(KickedActor))
		{
			KickCharacterDuringVault();
		}
		else
		{
			if (bIsVoidVault)
			{
				CharacterMovement->SetMovementMode(MOVE_Falling);
			}
		}
	}
	
	if (NotifyName.IsEqual("ReturnInput"))
	{
		if (IsValid(Player))
		{
			Player->SetPlayerGeneralState(EPlayerState::FreeMovement);
		}
		
		AnimInstance->Montage_Stop(0.15f, CurrentMontage);
	}
	
	if (NotifyName.IsEqual("CheckAim"))
	{
		if (IsValid(Player))
		{
			Player->ResetPlayerStateToFreeMovementAndCheckAim();
		}
	}

	if (NotifyName.IsEqual("ReturnInputMantle"))
	{
		CharacterMovement->SetMovementMode(MOVE_Walking);
		if (IsValid(Player))
		{
			Player->ResetPlayerStateToFreeMovementAndCheckAim();
		}
		AnimInstance->Montage_Stop(0.f, MantleMontage);
	}
}

// +-------------------------------------------------+
// |                                                 |
// |			     Montage Completed               |
// |                                                 |
// +-------------------------------------------------+

void UInteractiveMovementComponent::OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == VaultMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnVaultCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
		
		if (!bIsVoidVault)
		{
			CharacterMovement->SetMovementMode(MOVE_Walking);
		}
		
		if (!IsValid(Player))
		{
			return;
		}
		
		bCanWarp = false;
		if (bInterrupted)
		{
			if (IsValid(Player->GetStatusEffectComponent()) && !Player->GetStatusEffectComponent()->IsElectrified())
			{
				Player->ResetPlayerStateToFreeMovementAndCheckAim();
			}
		}
		else
		{
			Player->ResetPlayerStateToFreeMovementAndCheckAim();
		}
	}

}

void UInteractiveMovementComponent::OnShortVaultCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == ShortVaultMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnShortVaultCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);

		if (!bIsVoidVault)
		{
			CharacterMovement->SetMovementMode(MOVE_Falling);
		}

		if (!IsValid(Player))
		{
			return;
		}

		Player->SetPlayerGeneralState(EPlayerState::Falling);
		bCanWarp = false;
		if (Player->GetPlayerGeneralState() == EPlayerState::InTraversalNoControls)
		{
			UCustomCameraSystemComponent* CameraSystem = Player->GetCameraSystemComponent();
			if (IsValid(CameraSystem))
			{
				float UpdateRadius = Player->GetCameraSystemComponent()->GetProbeRadius() * CAM_RADIUS_MODIFIER;
				Player->GetCameraSystemComponent()->SetProbeRadius(UpdateRadius);
			}
			if (bInterrupted)
			{
				if (IsValid(Player->GetStatusEffectComponent()) && !Player->GetStatusEffectComponent()->IsElectrified())
				{
					Player->ResetPlayerStateToFreeMovementAndCheckAim();
				}
			}
			else
			{
				Player->ResetPlayerStateToFreeMovementAndCheckAim();
			}
		}
	}

}

void UInteractiveMovementComponent::OnMantleCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == MantleMontage)
	{
		CharacterMovement->SetMovementMode(MOVE_Walking);
		if (IsValid(Player))
		{
			Player->ResetPlayerStateToFreeMovementAndCheckAim();
		}
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnMantleCompleted);
		Player->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
	}
	
}

void UInteractiveMovementComponent::OnStepCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == StepMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnStepCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
		CharacterMovement->SetMovementMode(MOVE_Walking);
		bCanWarp = false;
		if (IsValid(Player) && Player->GetPlayerGeneralState() == EPlayerState::InTraversalNoControls)
		{
			Player->ResetPlayerStateToFreeMovementAndCheckAim();
		}
	}

}


void UInteractiveMovementComponent::OnRollCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Player))
	{
		FGlobalPointers::PlayerCharacter->ResetPlayerStateToFreeMovementAndCheckAim();
	}
	
	if (IsValid(Montage) && Montage == RollMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UInteractiveMovementComponent::OnRollCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UInteractiveMovementComponent::OnMontageNotifyBegin);
	}
}


bool UInteractiveMovementComponent::CheckFloor(const float InputXYSizeSquared)
{
	if (!IsValid(GenericCharacter))
	{
		return false;
	}
	FHitResult OutHit;
	const FVector LineTraceStart = GenericCharacter->GetActorLocation() + GenericCharacter->GetActorForwardVector() * (Capsule->GetScaledCapsuleRadius() + CHECK_FLOOR_FORWARD_OFFSET * FMath::Clamp(InputXYSizeSquared, 0, 1));
	const FVector LineTraceEnd = LineTraceStart - GenericCharacter->GetActorUpVector() * (Capsule->GetScaledCapsuleHalfHeight() + CHECK_FLOOR_VERTICAL_OFFSET);
	const bool bLineTrace = UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineTraceStart, LineTraceEnd, TraceTypeQuery1, false, {GenericCharacter}, EDrawDebugTrace::None, OutHit, true);
	return bLineTrace;
}


void UInteractiveMovementComponent::TryToApplyJumpBoost(const float DeltaTime)
{
	if (IsApplyingJumpBoost() && GetJumpBoostRemaining() > KINDA_SMALL_NUMBER)
	{
		float DeltaMove = GetJumpBoostSpeed() * DeltaTime;

		if (DeltaMove > GetJumpBoostRemaining())
		{
			DeltaMove = GetJumpBoostRemaining();
		}

		const FVector ForwardDir = GetOwner()->GetActorForwardVector();
		const FVector Move = ForwardDir * DeltaMove;
    
		GetOwner()->AddActorWorldOffset(Move, true);

		float Remaining = GetJumpBoostRemaining() - DeltaMove;

		SetJumpBoostRemaining(Remaining);

		if (GetJumpBoostRemaining() <= KINDA_SMALL_NUMBER)
		{
			SetIsApplyingJumpBoost(false);
		}
	}
}
