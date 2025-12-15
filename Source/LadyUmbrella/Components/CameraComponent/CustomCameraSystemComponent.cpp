// 
// CameraSystemComponent.cpp
// 
// Implementation of the CameraSystemComponent class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "CustomCameraSystemComponent.h"

#include "Camera/CameraComponent.h"
#include "CameraComponentTypes/CameraInputSpeed.h"
#include "CameraComponentTypes/CameraStateKey.h"
#include "CameraComponentTypes/ModifierType.h"
#include "CameraComponentTypes/ShakeSource.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveEvaluation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"
#include "Shakes/LegacyCameraShake.h"

UCustomCameraSystemComponent::UCustomCameraSystemComponent()
{
	bAutoActivate = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("FollowCamera"));

	CameraLookMultiplierAimAssist = 1.0f;

	// TODO: These should be taken from the owner's capsule.
	float CapsuleHalfHeight = 96.0f;
	float CapsuleRadius = 45.0f;
	
	GetCameraBoom()->SetRelativeLocation(FVector(0,0, 96.0f * 0.75f));
	GetCameraBoom()->TargetArmLength  = 400.f;
	GetCameraBoom()->bEnableCameraLag = true;
	GetCameraBoom()->bUsePawnControlRotation = true;
	GetCameraBoom()->bDoCollisionTest = false;
	GetCameraBoom()->bInheritRoll	  = false;

	GetFollowCamera()->SetLensPresetByName(TEXT("Lenses30mm Prime f/1.4"));
	GetFollowCamera()->bConstrainAspectRatio     = false;
	GetFollowCamera()->bUsePawnControlRotation   = false;
	GetFollowCamera()->CurrentAperture           = 6.f;
	GetFollowCamera()->CurrentFocalLength        = 30.f;
	GetFollowCamera()->Filmback.SensorWidth      = 72.f;
	GetFollowCamera()->Filmback.SensorHeight     = 40.5f;
	GetFollowCamera()->FocusSettings.bSmoothFocusChanges = true;
	GetFollowCamera()->FocusSettings.FocusMethod = ECameraFocusMethod::Tracking;
	GetFollowCamera()->FocusSettings.FocusSmoothingInterpSpeed = 12.f;
	GetFollowCamera()->FocusSettings.TrackingFocusSettings.ActorToTrack = GetOwner();
	GetFollowCamera()->FocusSettings.TrackingFocusSettings.RelativeOffset = FVector(CapsuleRadius / 4.f, 0.f, CapsuleHalfHeight);
	GetFollowCamera()->CropSettings.AspectRatio = GetFollowCamera()->Filmback.SensorWidth / GetFollowCamera()->Filmback.SensorHeight;
	
	// Curves
	ManualYawCurve = AssetUtils::FindObject<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Camera/C_Yaw.C_Yaw'"));
	ManualPitchCurve = AssetUtils::FindObject<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Camera/C_Pitch.C_Pitch'"));

	// Shakes
	CameraShakes.Add(
		EShakeSource::RECOIL,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakePlayer.BP_ShakePlayer_C'")));

	CameraShakes.Add(
		EShakeSource::GADGET_RECOIL,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakePlayerGadget.BP_ShakePlayerGadget_C'")));
	
	CameraShakes.Add(
		EShakeSource::DIAGONAL,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakePlayer.BP_ShakePlayer_C'")));
		
	CameraShakes.Add(
		EShakeSource::IDLE,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_SwayIdle.BP_SwayIdle_C'")));

	CameraShakes.Add(
		EShakeSource::SWAY,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_SwayMove.BP_SwayMove_C'")));

	CameraShakes.Add(
		EShakeSource::SWING,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_Swing.BP_Swing_C'")));

	CameraShakes.Add(
		EShakeSource::VIBRATION,
		AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_Vibration.BP_Vibration_C'")));
	
	CameraShakes.Add(
	EShakeSource::IMPACT,
	AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakeImpact.BP_ShakeImpact_C'")));
	
	CameraShakes.Add(
	EShakeSource::ENVIRONMENT,
	AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakeEnv.BP_ShakeEnv_C'")));
	
	CameraShakes.Add(
	EShakeSource::UNKNOWN,
	AssetUtils::FindClass<ULegacyCameraShake>(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/CameraShakes/BP_ShakeDefault.BP_ShakeDefault_C'")));
	
	// Camera States
	CurrentState	= ECameraStateKey::DEFAULT;
	
	RelativeCapsuleLocation = CapsuleHalfHeight * 0.75f;
	MeshTopSocket = "HeadSocket";
	MuzzleTopSocket = "WeaponMuzzle";
	RelativeLowCoverCollisionHeightOffset = -100.f;
	
	CameraStates.Add(ECameraStateKey::DEFAULT, FCameraState());
	CameraStates.Add(ECameraStateKey::CINEMATIC, FCameraState());
	CameraStates.Add(ECameraStateKey::LOOKAT, FCameraState());
	CameraStates.Add(ECameraStateKey::COVER, FCameraState());
	CameraStates.Add(ECameraStateKey::AIM, FCameraState());
	CameraStates.Add(ECameraStateKey::SHIELD, FCameraState());
	
	CameraStates[ECameraStateKey::DEFAULT].Distance					= 235.f;
	CameraStates[ECameraStateKey::DEFAULT].DesiredScreenRatio		= FVector2D(0.42f,0.36f);
	CameraStates[ECameraStateKey::DEFAULT].BottomHitThreshold		= 0.2f;
	CameraStates[ECameraStateKey::DEFAULT].SideHitThreshold			= 0.2f;
	CameraStates[ECameraStateKey::DEFAULT].WallHitThreshold			= 0.2f;
	CameraStates[ECameraStateKey::DEFAULT].InterpolationType		= ECameraInterpolation::EASE_O;
	CameraStates[ECameraStateKey::DEFAULT].InterpolationSpeed		= 1.f;
	CameraStates[ECameraStateKey::DEFAULT].EaseExponent				= 5.f;
	CameraStates[ECameraStateKey::DEFAULT].bAfterCollisionDelay		= true;
	CameraStates[ECameraStateKey::DEFAULT].bCameraLag				= false;
	CameraStates[ECameraStateKey::DEFAULT].DelaySeconds				= 0.1f;

	CameraStates[ECameraStateKey::CINEMATIC].bFocusDistanceDynamic	= false;
	CameraStates[ECameraStateKey::CINEMATIC].bCameraLag				= true;
	CameraStates[ECameraStateKey::CINEMATIC].DesiredScreenRatio		= FVector2D(0.4f,0.5f);
	CameraStates[ECameraStateKey::CINEMATIC].InterpolationType		= ECameraInterpolation::EASE_IO;
	CameraStates[ECameraStateKey::CINEMATIC].Distance				= 150.f;
	CameraStates[ECameraStateKey::CINEMATIC].InterpolationSpeed		= 1.5f;
	CameraStates[ECameraStateKey::CINEMATIC].EaseExponent			= 3.f;
	CameraStates[ECameraStateKey::CINEMATIC].BottomHitThreshold		= 0.3f;
	CameraStates[ECameraStateKey::CINEMATIC].SideHitThreshold		= 0.3f;
	CameraStates[ECameraStateKey::CINEMATIC].WallHitThreshold		= 0.3f;
	
	CameraStates[ECameraStateKey::LOOKAT].bFocusDistanceDynamic		= true;
	CameraStates[ECameraStateKey::LOOKAT].bCameraLag				= true;
	CameraStates[ECameraStateKey::LOOKAT].DesiredScreenRatio		= FVector2D(0.1f,0.2f);
	CameraStates[ECameraStateKey::LOOKAT].InterpolationType			= ECameraInterpolation::EASE_IO;
	CameraStates[ECameraStateKey::LOOKAT].Distance					= 75.f;
	CameraStates[ECameraStateKey::LOOKAT].InterpolationSpeed		= 1.5f;
	CameraStates[ECameraStateKey::LOOKAT].EaseExponent				= 3.f;
	CameraStates[ECameraStateKey::LOOKAT].BottomHitThreshold		= 0.3f;
	CameraStates[ECameraStateKey::LOOKAT].SideHitThreshold			= 0.3f;
	CameraStates[ECameraStateKey::LOOKAT].WallHitThreshold			= 0.3f;
	
	CameraStates[ECameraStateKey::COVER].Distance					= 325.f;
	CameraStates[ECameraStateKey::COVER].DesiredScreenRatio			= FVector2D(0.4f,0.35f);
	CameraStates[ECameraStateKey::COVER].InterpolationType			= ECameraInterpolation::EASE_O;
	CameraStates[ECameraStateKey::COVER].InterpolationSpeed			= 1.f;
	CameraStates[ECameraStateKey::COVER].EaseExponent				= 5.f;
	CameraStates[ECameraStateKey::COVER].bCameraLag					= false;
	
	CameraStates[ECameraStateKey::AIM].bFocusDistanceDynamic		= true;
	CameraStates[ECameraStateKey::AIM].Distance						= 50.f;
	CameraStates[ECameraStateKey::AIM].DesiredScreenRatio			= FVector2D(0.05f,0.1f);
	CameraStates[ECameraStateKey::AIM].BottomHitThreshold			= 0.3;
	CameraStates[ECameraStateKey::AIM].SideHitThreshold				= 0.3;
	CameraStates[ECameraStateKey::AIM].WallHitThreshold				= 0.3;
	CameraStates[ECameraStateKey::AIM].InterpolationType			= ECameraInterpolation::LINEAR;
	CameraStates[ECameraStateKey::AIM].InterpolationSpeed			= 3.5f;
	CameraStates[ECameraStateKey::AIM].bCameraLag					= false;
	
	CameraStates[ECameraStateKey::SHIELD].bFocusDistanceDynamic		= true;
	CameraStates[ECameraStateKey::SHIELD].Distance					= 10.f;
	CameraStates[ECameraStateKey::SHIELD].DesiredScreenRatio		= FVector2D(0.01f,0.01f);
	CameraStates[ECameraStateKey::SHIELD].BottomHitThreshold		= 0.3;
	CameraStates[ECameraStateKey::SHIELD].SideHitThreshold			= 0.3;
	CameraStates[ECameraStateKey::SHIELD].WallHitThreshold			= 0.3;
	CameraStates[ECameraStateKey::SHIELD].InterpolationType			= ECameraInterpolation::LINEAR;
	CameraStates[ECameraStateKey::SHIELD].InterpolationSpeed		= 3.5f;
	CameraStates[ECameraStateKey::SHIELD].bCameraLag				= false;
	
	// Parameters
	ResetStepSize				= 0.1f;
	CameraMultipliers.Add("KBM", FCameraMultipliers());
	CameraMultipliers.Add("XInputController", FCameraMultipliers());
	
	CameraMultipliers["XInputController"].bSensibilityFitsCurve= true;
	CurrentInputSpeedState		= ECameraInputSpeed::STATIC;
	
	CameraCollisionRadius		= 40.f;
	TargetBoomLength			= 0.f;
	LookAtTarget				= FVector::ZeroVector;
	TargetOffset				= FVector::ZeroVector;
	CurrentOffset				= FVector::ZeroVector;
	PreviousInputAdjust			= FVector2D::ZeroVector;
	ViewPitchRange				= FVector2D(-60,50);
	
	CameraMoveAssistStrength	= 0.5f;
	CurrentTargetYaw			= 0.f;
	YawAssistanceTimer			= 1.f;	
	YawAssistanceDelay			= 5.f;
	CameraSideMultiplier		= 2.f;
	DelayedCollisionTimer		= 0.f;
	InterpTimer					= 0.f;
	InterpolationStart_Dist		= 0.f;
	CurrentAccelerationInput	= 0.f;
	InputAccelerationRate		= 0.08f;
	InputDecelerationRate		= 0.125f;
	InterpolationStart_Offset	= FVector::ZeroVector;
	
	bCameraIsResetting			= false;
	bYawAssistance				= false;
	bResetOnCover				= false;
}

void UCustomCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ignored Actors in regard to avoidance and collision
	TArray<AActor*> CameraTaggedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("CameraIgnore"), CameraTaggedActors);
	for (AActor* CameraTaggedActor : CameraTaggedActors)
	{
		IgnoredActorsCamera.Add(CameraTaggedActor);
	}
	
	SetUpCamera();
	
	for (TTuple<FName, FCameraMultipliers>& StareMult : CameraMultipliers)
	{
		StareMult.Value.SensibilityX = FMath::Clamp(StareMult.Value.SensibilityX, StareMult.Value.SensibilityRange.X, StareMult.Value.SensibilityRange.Y);
		StareMult.Value.SensibilityY = FMath::Clamp(StareMult.Value.SensibilityY, StareMult.Value.SensibilityRange.X, StareMult.Value.SensibilityRange.Y);
		StareMult.Value.AimMultiplier = FMath::Clamp(StareMult.Value.AimMultiplier, StareMult.Value.MultiplierRange.X, StareMult.Value.MultiplierRange.Y);
		StareMult.Value.DefaultMultiplier = FMath::Clamp(StareMult.Value.DefaultMultiplier, StareMult.Value.MultiplierRange.X, StareMult.Value.MultiplierRange.Y);
	}
}

void UCustomCameraSystemComponent::SetUpCamera()
{
	float TempMin = ViewPitchRange.GetMin();
	ViewPitchRange.Y = ViewPitchRange.GetMax();
	ViewPitchRange.X = TempMin;
	
	ResetCameraParams();
	InterpolationStart_Dist = CameraCollisionRadius / 4.f;
}

void UCustomCameraSystemComponent::CameraTick(const float& DeltaSeconds)
{
	if (IsValid(CameraBoom) && IsValid(FollowCamera))
	{
		UpdateForCameraState(DeltaSeconds);
		if (bCameraIsResetting)
		{
			ResetCamProgress(GetWorld()->GetDeltaSeconds());
		}
		ManageCameraCollisions();
		UpdateCameraPosition(DeltaSeconds);
	}
	//DrawDebugSphere(GetWorld(),GetCameraBoom()->GetComponentLocation(),20.f, 1, FColor::Purple);
}

void UCustomCameraSystemComponent::ResetCameraParams()
{
	if (!IsValid(CameraBoom) || !IsValid(FollowCamera))
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraBoom or FollowCamera is invalid. Aborting ResetCameraParams."));
		return;
	}
	
	CurrentState = ECameraStateKey::DEFAULT;

	TargetOffset = FVector::ZeroVector;
	GetCameraBoom()->TargetOffset = FVector::ZeroVector;
	GetCameraBoom()->TargetArmLength = CameraStates[CurrentState].Distance;
	
	FollowCamera->CurrentFocusDistance = MaxFocusDist;
	
	YawAssistanceTimer = YawAssistanceDelay;
	DelayedCollisionTimer = CameraStates[CurrentState].DelaySeconds;
	SetCameraInputMotion(ECameraInputSpeed::STATIC);

	if (IsValid(FGlobalPointers::MainController))
	{
		APlayerCameraManager* CameraManager = FGlobalPointers::MainController->PlayerCameraManager;
		if (IsValid(CameraManager))
		{
			CameraManager->ViewPitchMin = ViewPitchRange.X;
			CameraManager->ViewPitchMax = ViewPitchRange.Y;
		}
	}
}

void UCustomCameraSystemComponent::UpdateCurrentControlSensitivity(const FString& DeviceType, const float NewValue, const bool bHorizontal)
{
	FCameraMultipliers* TargetModifiers = &CameraMultipliers[FName(DeviceType)];
	if (bHorizontal)
	{
		TargetModifiers->SensibilityX = FMath::Clamp(NewValue, TargetModifiers->SensibilityRange.X, TargetModifiers->SensibilityRange.Y);
		return;
	}
	TargetModifiers->SensibilityY = FMath::Clamp(NewValue, TargetModifiers->SensibilityRange.X, TargetModifiers->SensibilityRange.Y);
}

void UCustomCameraSystemComponent::UpdateModifier(EModifierType Modifier, TArray<float> NewValues)
{
	TArray<FName> Keys;
	CameraMultipliers.GetKeys(Keys);
	switch (Modifier)
	{
	case EModifierType::DEFAULT:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			FCameraMultipliers* Current = &CameraMultipliers[Keys[Index]];
			Current->DefaultMultiplier = FMath::Clamp(NewValues[Index],Current->MultiplierRange.X,Current->MultiplierRange.Y);
		}
		break;
		
	case EModifierType::AIM:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			FCameraMultipliers* Current = &CameraMultipliers[Keys[Index]];
			Current->AimMultiplier = FMath::Clamp(NewValues[Index],Current->MultiplierRange.X,Current->MultiplierRange.Y);
		}
		break;
		
	case EModifierType::YAW:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			CameraMultipliers[Keys[Index]].InvertYaw = NewValues[Index] < 0;
		}
		break;
		
	case EModifierType::PITCH:
		for (int Index = 0; Index < CameraMultipliers.Num(); Index++)
		{
			CameraMultipliers[Keys[Index]].InvertPitch = NewValues[Index] < 0;
		}
		break;
	}
}

void UCustomCameraSystemComponent::UpdateModifier(const FName& DeviceType, const EModifierType Modifier, const float NewValue)
{
	FCameraMultipliers* TargetModifiers = &CameraMultipliers[FName(DeviceType)];
	
	switch (Modifier)
	{
		case EModifierType::DEFAULT:
			TargetModifiers->DefaultMultiplier = FMath::Clamp(NewValue,TargetModifiers->MultiplierRange.X,TargetModifiers->MultiplierRange.Y);
			break;
		
		case EModifierType::AIM:
			TargetModifiers->AimMultiplier = FMath::Clamp(NewValue,TargetModifiers->MultiplierRange.X,TargetModifiers->MultiplierRange.Y);
			break;
		
		case EModifierType::YAW:
			TargetModifiers->InvertYaw = NewValue < 0;
			break;
		
		case EModifierType::PITCH:
			TargetModifiers->InvertPitch = NewValue < 0;
			break;
	}
}


FVector2D UCustomCameraSystemComponent::GetAdjustedLookVector(FString DeviceType, FVector2D LookAxisVector)
{
	if (GetCurrentCameraStateKey() == ECameraStateKey::LOOKAT || GetCurrentCameraStateKey() == ECameraStateKey::CINEMATIC)
	{
		return FVector2D::ZeroVector;
	}

	if (CurrentInputSpeedState != ECameraInputSpeed::PEAKED)
	{
		CurrentAccelerationInput = FMath::Min(CurrentAccelerationInput + InputAccelerationRate, 1.f);
		SetCameraInputMotion(CurrentAccelerationInput < 1 ? ECameraInputSpeed::ACCELERATING : ECameraInputSpeed::PEAKED);
	}
	
	FCameraMultipliers ActiveMultipliers = CameraMultipliers[FName(DeviceType)];
	
	float ResultX = LookAxisVector.X * CurrentAccelerationInput;
	float ResultY = LookAxisVector.Y * CurrentAccelerationInput;

	if (ActiveMultipliers.bSensibilityFitsCurve)
	{
		if (IsValid(ManualYawCurve) && IsValid(ManualPitchCurve))
		{
			ResultX = ManualYawCurve->GetFloatValue(FMath::Abs(ResultX)) * (ResultX/FMath::Abs(ResultX));
			ResultY = ManualPitchCurve->GetFloatValue(FMath::Abs(ResultY)) * (ResultY/FMath::Abs(ResultY));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Yaw or Pitch Curve are invalid. Unable to do adjust Input to curve."));
		}
	}
	
	ResultX *= (ActiveMultipliers.InvertYaw ? -1 : 1) * ActiveMultipliers.SensibilityX;
	ResultY *= (ActiveMultipliers.InvertPitch ? -1 : 1) * ActiveMultipliers.SensibilityY;

	if (GetCurrentCameraStateKey() == ECameraStateKey::AIM)
	{
		ResultX *= ActiveMultipliers.AimMultiplier;
		ResultY *= ActiveMultipliers.AimMultiplier;
	}
	else
	{
		ResultX *= ActiveMultipliers.DefaultMultiplier;
		ResultY *= ActiveMultipliers.DefaultMultiplier;
	}
/*
	// Deprecated, remains just in case something similar is needed
	if (GetCurrentCameraStateKey() == ECameraStateKey::COVER)
	{
		FHitResult CameraCoverHit;
		const float ProvingDistance = 200.f;
		UCharacterCoverMovementComponent* CoverMovComponent = Cast<UCharacterCoverMovementComponent>(GetOwner()->GetComponentByClass(UCharacterCoverMovementComponent::StaticClass()));
		bool bLowCover = IsValid(CoverMovComponent) ? !CoverMovComponent->IsInTallCover() : false;
		FVector Start = GetCameraBoom()->GetComponentLocation() + FVector(0.f, 0.f, bLowCover ?  RelativeLowCoverCollisioneHeightOffset : 0.f);
		FVector End = GetCameraBoom()->GetComponentLocation() + (FollowCamera->GetComponentLocation() - GetCameraBoom()->GetComponentLocation()).GetSafeNormal() * ProvingDistance;
		End.Z = Start.Z;
		bool bHasCollided = UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActorsCamera,
		EDrawDebugTrace::None,
		CameraCoverHit,
		true,
		FColor::Yellow,
		FColor::Green,
		1.f);	

		const float SideDot		= FVector::DotProduct(FollowCamera->GetRightVector().GetSafeNormal(), CameraCoverHit.Normal);
		const float UpDot		= FVector::DotProduct(FollowCamera->GetUpVector().GetSafeNormal(), CameraCoverHit.Normal);
		
		bool bCoverCollitionAgainstMovement = (SideDot * ResultX) > 0;
		bool bFloorCollitionAgainstMovement = (UpDot * ResultY) > 0;

		if (bHasCollided)
		{
			 AActor* Cover = CameraCoverHit.GetActor();
			if (IsValid(Cover) && Cover->IsA(ACoverParent::StaticClass()) && bCoverCollitionAgainstMovement)
			{
				ResultX = 0.f;
			}Needs further testing
			if (!bFloorCollitionAgainstMovement)
			{
				FLogger::WarningLog("Cannot move anymore towards that direction (Hitting Floor)");
				ResultY = 0.f;
			}	
		}
	}
	*/

	PreviousInputAdjust = FVector2D(ResultX, ResultY);
	return PreviousInputAdjust;
}

float UCustomCameraSystemComponent::GetCurrentSensibility(FName Device, bool bHorizontal)
{
	if (bHorizontal)
	{
		return CameraMultipliers[Device].SensibilityX;
	}
	return CameraMultipliers[Device].SensibilityY;
}

FVector2D UCustomCameraSystemComponent::GetDeceleratingLookVector(float DeltaTime)
{
	CurrentAccelerationInput = FMath::Max(CurrentAccelerationInput - InputDecelerationRate, 0.f);
	SetCameraInputMotion(CurrentAccelerationInput > 0.f ? ECameraInputSpeed::DECELERATING : ECameraInputSpeed::STATIC);
	return PreviousInputAdjust * CurrentAccelerationInput;
}

void UCustomCameraSystemComponent::SetCameraInputMotion(ECameraInputSpeed MotionType)
{
	CurrentInputSpeedState = MotionType;
	if (MotionType == ECameraInputSpeed::STATIC)
	{
		YawAssistanceTimer = YawAssistanceDelay;
	}
}

void UCustomCameraSystemComponent::MoveAssistance(const FVector2D& MoveAxisVector, const bool bMoveControlledByCover) const
{
	if (CurrentInputSpeedState != ECameraInputSpeed::STATIC || GetCurrentCameraStateKey() != ECameraStateKey::DEFAULT)
	{
		return;
	}

	FVector2D NormalizedMovementVector = MoveAxisVector.GetSafeNormal();

	if (YawAssistanceTimer <= 0.f && !bMoveControlledByCover && IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->AddYawInput(NormalizedMovementVector.X * CameraMoveAssistStrength);
	}
}

void UCustomCameraSystemComponent::SetCameraState(ECameraStateKey StateKey)
{
	if(!CameraStates.Contains(StateKey))
	{
		UE_LOG(LogEngine, Error, TEXT("Inputted CameraStateKey doesn't exist"));
		return;
	}

	if(GetCurrentCameraState().AttachmentLocation != GetCameraState(StateKey).AttachmentLocation) 
	{
		FVector LocalBoomLocation = GetCameraBoom()->GetComponentLocation();
		
		switch (GetCameraState(StateKey).AttachmentLocation)
		{
			case ECameraAttachment::CAPSULE_TOP:
				if (IsValid(FGlobalPointers::PlayerCharacter))
				{
					GetCameraBoom()->AttachToComponent(FGlobalPointers::PlayerCharacter->GetCapsuleComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
					GetCameraBoom()->SetRelativeLocation(FVector(0.f,0.f,RelativeCapsuleLocation));
				}
				break;
			case ECameraAttachment::MESH_TOP:
				if (IsValid(FGlobalPointers::PlayerCharacter))
				{
					GetCameraBoom()->AttachToComponent(FGlobalPointers::PlayerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), FName(MeshTopSocket));
					GetCameraBoom()->SetRelativeLocation(FVector::ZeroVector);
				}
				break;
			case ECameraAttachment::WEAPON_MUZZLE:
				if (IsValid(FGlobalPointers::Umbrella))
				{
					GetCameraBoom()->AttachToComponent(FGlobalPointers::Umbrella->GetSkeletalMeshComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, false), FName(MuzzleTopSocket));
					GetCameraBoom()->SetRelativeLocation(FVector::ZeroVector);
				}
				break;
		}
		
		UpdateForAttachment(LocalBoomLocation);
	}

	CurrentState = StateKey;
	InterpolationStart_Dist = GetCameraBoom()->TargetArmLength;
	InterpolationStart_Offset = CurrentOffset;
	DelayedCollisionTimer = 0.f;
	InterpTimer = 0.f;
	if (GetCurrentCameraStateKey() == ECameraStateKey::COVER)
	{
		if (bResetOnCover)
		{
			ResetCameraPosition();
		}
	}
}

void UCustomCameraSystemComponent::SetCameraStateDistance(ECameraStateKey StateKey, float Distance2Player)
{
	if (CameraStates[StateKey].Distance != Distance2Player)
	{
		CameraStates[StateKey].Distance = Distance2Player;
		InterpolationStart_Dist = GetCameraBoom()->TargetArmLength;
		InterpTimer = 0.f;
	}
}

void UCustomCameraSystemComponent::SetCameraStateShoulder(ECameraStateKey StateKey, bool InvertedShoulder)
{
	if (CameraStates[StateKey].bScreenRatioMirrored != InvertedShoulder)
	{
		CameraStates[StateKey].bScreenRatioMirrored = InvertedShoulder;
		InterpolationStart_Offset = CurrentOffset;
		InterpTimer = 0.f;
	}
}

void UCustomCameraSystemComponent::SetCameraStateOffsetValue(ECameraStateKey StateKey, FVector2D Ratio)
{
	if (CameraStates[StateKey].DesiredScreenRatio != Ratio)
	{
		CameraStates[StateKey].DesiredScreenRatio.X = Ratio.X;
		CameraStates[StateKey].DesiredScreenRatio.Y = Ratio.Y;
		InterpolationStart_Offset = CurrentOffset;
		InterpTimer = 0.f;
	}
}

void UCustomCameraSystemComponent::ResetCameraPosition()
{
	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->DisableInput(FGlobalPointers::MainController);
		bCameraIsResetting = true;
	}
}

void UCustomCameraSystemComponent::ResetCamProgress(const double& DeltaSeconds)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		return;
	}

	FRotator ControlRot = FGlobalPointers::MainController->GetControlRotation();
	FQuat ControlQuat = ControlRot.Quaternion();

	const float AngularDistance = FMath::RadiansToDegrees(UKismetMathLibrary::Quat_AngularDistance(ControlQuat, ControlQuat));
	const float AngularThreshold = 1.0f;
	
	if (AngularDistance > AngularThreshold)
	{
		FQuat InterpolatedQuat = FQuat::Slerp(ControlQuat, ControlQuat, DeltaSeconds * ResetStepSize);
        FGlobalPointers::MainController->SetControlRotation(InterpolatedQuat.Rotator());
	}
	else
	{
		FGlobalPointers::MainController->EnableInput(FGlobalPointers::MainController);
		bCameraIsResetting = false;
	}
}

void UCustomCameraSystemComponent::UpdateForAttachment(const FVector& OriginalLocation)
{
	FVector Difference = GetCameraBoom()->GetComponentLocation() - OriginalLocation;

	GetCameraBoom()->TargetArmLength += Difference.X;

	Difference.X = 0.f;
	CurrentOffset -= Difference;
	GetCameraBoom()->TargetOffset = FVector(
		CurrentOffset.Y * -GetCameraBoom()->GetTargetRotation().Vector().Y,
		CurrentOffset.Y * GetCameraBoom()->GetTargetRotation().Vector().X,
		CurrentOffset.Z);
}

void UCustomCameraSystemComponent::UpdateForCameraState(const float& DeltaSeconds)
{
	FCameraState State = GetCurrentCameraState();
	TargetOffset = ComputeOffset(State);
	FollowCamera->SetUseFieldOfViewForLOD(State.bUseFoVForLOD);
}

void UCustomCameraSystemComponent::MakeCameraShake(const EShakeSource& Type, FVector Location, float RadiusFrom, float RadiusTo, bool bMirrored)
{
	if(Location.IsNearlyZero())
	{
		Location = GetOwner()->GetActorLocation();
	}

	UGameplayStatics::PlayWorldCameraShake(
		GetWorld(),
		CameraShakes[Type],
		Location,
		RadiusFrom,
		RadiusTo,
		1.f,
		true);
}

void UCustomCameraSystemComponent::UpdateCameraPosition(const float& DeltaSeconds)
{
	FCameraState CurrentCameraState = GetCurrentCameraState();
	GetCameraBoom()->bEnableCameraLag = CurrentCameraState.bCameraLag;	
	
	// Arm Length & Offset
	FVector DesiredOffset = CurrentOffset;
	float DesiredArmLength = GetCameraBoom()->TargetArmLength;
	float Progress = InterpolateFloat();
	
	DesiredOffset = CurrentOffset.Equals(TargetOffset)
	? TargetOffset
	: FMath::Lerp(InterpolationStart_Offset,TargetOffset, Progress);

	DesiredArmLength = FMath::IsNearlyEqual(GetCameraBoom()->TargetArmLength, TargetBoomLength)
	? TargetBoomLength
	: FMath::Lerp(InterpolationStart_Dist,TargetBoomLength, Progress);

	if (GetCurrentCameraStateKey() == ECameraStateKey::LOOKAT)
	{
		if (!IsValid(FGlobalPointers::MainController))
		{
			return;
		}
		
		FVector DirectionToTarget = (LookAtTarget - GetCameraBoom()->GetComponentLocation()).GetSafeNormal();
		FQuat TargetQuat = DirectionToTarget.Rotation().Quaternion();
		FQuat ControlQuat = FGlobalPointers::MainController->GetControlRotation().Quaternion();
		
		const float AngularDistance = FMath::RadiansToDegrees(UKismetMathLibrary::Quat_AngularDistance(ControlQuat, TargetQuat));
	
		if (!FMath::IsNearlyZero(AngularDistance))
		{
			FQuat InterpolatedQuat = FQuat::Slerp(ControlQuat, TargetQuat, DeltaSeconds);
			FGlobalPointers::MainController->SetControlRotation(InterpolatedQuat.Rotator());
		}
	}
	
	if (InterpTimer == 1.f)
	{
		InterpolationStart_Dist = GetCameraBoom()->TargetArmLength;
		InterpolationStart_Offset = CurrentOffset;
		GetCameraBoom()->bEnableCameraLag = false;
	}
	
	ManageArmExpectedValue(DesiredArmLength, DesiredOffset);
	
	// TODO: if (onStairs) -> Smooth Bumpy Behaviour (https://github.com/mrezai/GodotStairs/blob/master/Player/Player.gd) (https://youtu.be/Tb-R3l0SQdc?t=1699)
	// TODO: if (cameraEvent) -> Completely Lose Camera Control (New Asset)
	
	// Focus Distance
	if (CurrentCameraState.bFocusDistanceDynamic)
	{
		FollowCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
		TArray<AActor*> AttachedActors;
		GetOwner()->GetAttachedActors(AttachedActors);
		AttachedActors.Add(GetOwner());

		FHitResult HitForFocus;
		bool bAimingAtObject = UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			FollowCamera->GetComponentLocation(),
			FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * MaxFocusDist,
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			AttachedActors,
			EDrawDebugTrace::None,
			HitForFocus,
			true);
			
		if (bAimingAtObject)
		{
			FollowCamera->FocusSettings.ManualFocusDistance = HitForFocus.Distance;
		}
		else
		{
			FollowCamera->FocusSettings.ManualFocusDistance = MaxFocusDist;
		}
	}
	else
	{
		FollowCamera->FocusSettings.FocusMethod = ECameraFocusMethod::Tracking;
		FollowCamera->FocusSettings.TrackingFocusSettings.ActorToTrack = GetOwner();
	}

	// Timers
	if (bYawAssistance)
	{
		YawAssistanceTimer -= DeltaSeconds;
	}
	if (CurrentCameraState.bAfterCollisionDelay)
	{
		DelayedCollisionTimer -= DeltaSeconds;
	}
	const bool bCollisionDelayResolved	= !CurrentCameraState.bAfterCollisionDelay || DelayedCollisionTimer <= 0.f;
	if (InterpTimer < 1.f && (bCollisionDelayResolved || InterpolationStart_Dist > TargetBoomLength))
	{
		InterpTimer += DeltaSeconds * CurrentCameraState.InterpolationSpeed;
	}
}

void UCustomCameraSystemComponent::ManageCameraCollisions()
{
	FCameraState State = GetCurrentCameraState();

	const FVector Start = GetCameraBoom()->GetComponentLocation();
	const FVector End	= Start + GetCameraBoom()->TargetOffset - GetCameraBoom()->GetTargetRotation().Vector() * GetCameraBoom()->TargetArmLength;

	CameraHits.Empty();
	bool bHasCollided = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,
		End,
		CameraCollisionRadius,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActorsCamera,
		EDrawDebugTrace::None,
		CameraHits,
		true);
	
	if (bHasCollided)
	{
		if (State.bAfterCollisionDelay)
		{
			DelayedCollisionTimer = State.DelaySeconds;
		}

		if (!FMath::IsNearlyEqual(InterpolationStart_Dist, GetCameraBoom()->TargetArmLength))
		{
			InterpolationStart_Dist = GetCameraBoom()->TargetArmLength;
		}
		InterpolationStart_Offset = CurrentOffset;
		
		FHitResult SynthesizedHit;
		SynthesizedHit.Normal		= FVector::ZeroVector;
		SynthesizedHit.Location		= FVector::ZeroVector;
		SynthesizedHit.ImpactPoint	= FVector::ZeroVector;

		for (const FHitResult& Hit : CameraHits)
		{
			SynthesizedHit.Normal		+= Hit.Normal;
			SynthesizedHit.Location		+= Hit.Location;
			SynthesizedHit.ImpactPoint	+= Hit.ImpactPoint;
		}

		SynthesizedHit.Normal		/= CameraHits.Num();
		SynthesizedHit.Location		/= CameraHits.Num();
		SynthesizedHit.ImpactPoint	/= CameraHits.Num();
		
		EvaluateCollision(SynthesizedHit);
	}
	else
	{
		TargetBoomLength = State.Distance;
		GetCameraBoom()->bDoCollisionTest = false;
	}
	
}

FVector UCustomCameraSystemComponent::ComputeOffset(const FCameraState& CurrState)
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		return FVector::ZeroVector;
	}
	
	int32 ViewportWidth = 0, ViewportHeight = 0;
	FGlobalPointers::MainController->GetViewportSize(ViewportWidth,ViewportHeight);
	
	const float DesiredPercentX = CurrState.bScreenRatioMirrored ? (1.f - CurrState.DesiredScreenRatio.X) : CurrState.DesiredScreenRatio.X;
	const float DesiredPercentY = CurrState.DesiredScreenRatio.Y;
	const float RadiansFoV		= FMath::DegreesToRadians(FollowCamera->FieldOfView);
	const float FocalLenght		= (ViewportWidth / 2.0f) / FMath::Tan(RadiansFoV / 2.0f);

	// Perspective Projection supposing that Screen X is within [-ViewportWidth/2 to +ViewportWidth/2] being 0 the center
	const float RawScreenOffsetY = (0.5f - DesiredPercentX) * ViewportWidth;
	const float RawScreenOffsetZ = (DesiredPercentY - 0.5f) * ViewportHeight;
		
	float TargetOffsetY = RawScreenOffsetY * (CurrState.Distance / FocalLenght);
	float TargetOffsetZ = RawScreenOffsetZ * (CurrState.Distance / FocalLenght);
	
	return FVector(0.f, TargetOffsetY, TargetOffsetZ);
}

void UCustomCameraSystemComponent::EvaluateCollision(const FHitResult& CollisionResult)
{
	const FVector HitNormal		 = CollisionResult.Normal;
	const FCameraState CurrState = GetCurrentCameraState();

	const float SideDot		= FVector::DotProduct(FollowCamera->GetRightVector().GetSafeNormal(), HitNormal);
	const float ForwardDot	= FVector::DotProduct(FollowCamera->GetForwardVector().GetSafeNormal(), HitNormal);
	const float UpDot		= FVector::DotProduct(FollowCamera->GetUpVector().GetSafeNormal(), HitNormal);

	const float ProcessedSide = FMath::Abs(SideDot)/CurrState.SideHitThreshold;
	const float ProcessedFwd = ForwardDot/CurrState.WallHitThreshold;
	const float ProcessedUp = UpDot/CurrState.BottomHitThreshold;
	
	// Checking for most significant collision type
	bool bWallBehind		= ProcessedFwd > ProcessedUp && ProcessedFwd > ProcessedSide;
	bool bBottomCollision	= !bWallBehind && ProcessedUp > ProcessedSide;
	bool bSideCollision		= !(bWallBehind || bBottomCollision);
	
	if (bWallBehind || bBottomCollision || (bSideCollision && GetCurrentCameraStateKey() != ECameraStateKey::COVER))
	{
		CameraCorrectiveMovement(CollisionResult);
	}
	else if (bSideCollision)
	{
		const bool SideCollisionFromRight = SideDot > 0.f;
		CameraCorrectiveMovement(CollisionResult); // should be swaped with ClampLateralOffset(TargetOffset, CollisionResult.Location, SideCollisionFromRight);	
	}
}

void UCustomCameraSystemComponent::CameraCorrectiveMovement(const FHitResult& Hit)
{
	// Projecting Hit Location into Spring Arm Vector
	const FVector End(GetCameraBoom()->GetComponentLocation() + GetCameraBoom()->TargetOffset - GetCameraBoom()->GetTargetRotation().Vector() * GetCameraBoom()->TargetArmLength);
	const FVector OriginVector(End - GetCameraBoom()->GetComponentLocation());
	const FVector HitVector(Hit.ImpactPoint - (OriginVector.GetSafeNormal() * CameraCollisionRadius) - GetCameraBoom()->GetComponentLocation());
	const FVector ProjectedPoint(GetCameraBoom()->GetComponentLocation() + FVector::DotProduct(HitVector, OriginVector) / FVector::DotProduct(OriginVector, OriginVector) * OriginVector);

	// Setting Arm Length as distance to Projected Point
	float Dist2Point = (ProjectedPoint - GetCameraBoom()->GetComponentLocation()).Size();
	if (TargetBoomLength > Dist2Point)
	{
		if (FMath::IsNearlyEqual(GetCurrentCameraState().Distance, TargetBoomLength))
		{
			InterpTimer = 0.f;
		}

		if (Dist2Point > 40.f)
		{
			TargetBoomLength = Dist2Point;
		}
		else
		{
			TargetBoomLength = CameraCollisionRadius / 4.f;
		}
	}
	
	GetCameraBoom()->bDoCollisionTest = true;
}

FVector UCustomCameraSystemComponent::GetOffsetFromDistance(const float BoomDistance, const FVector TheoreticalOffset) const
{
	if (!IsValid(FGlobalPointers::MainController))
	{
		return TheoreticalOffset;
	}
	const float Scale = TheoreticalOffset.Equals(TargetOffset) ? FMath::Min(1.f, BoomDistance / GetCurrentCameraState().Distance) : 1.f;
	const float ExpectedLateralOffset = TheoreticalOffset.Y * Scale;
	const float ExpectedHeightOffset = TheoreticalOffset.Z * Scale;
	
	FRotator TargetRot = FGlobalPointers::MainController->GetControlRotation();
	FVector Right   = FRotationMatrix(TargetRot).GetUnitAxis(EAxis::Y); // Right vector
	FVector Up		= FollowCamera->GetUpVector(); // Right vector
	
	float CurrentPitch = FGlobalPointers::MainController->GetControlRotation().Pitch;
	CurrentPitch = CurrentPitch > 90.f ? CurrentPitch - 360.f : CurrentPitch;
	CurrentPitch /= 90.f;
	FVector OldMethod = FVector(
		ExpectedLateralOffset * -GetCameraBoom()->GetTargetRotation().Vector().Y,
		ExpectedLateralOffset * GetCameraBoom()->GetTargetRotation().Vector().X,
		ExpectedHeightOffset);
	
	FVector NewMethod = (Right * ExpectedLateralOffset) + /*(FVector::UpVector * 2.f * ExpectedHeightOffset -*/ (Up * ExpectedHeightOffset);
	return OldMethod;
}

float UCustomCameraSystemComponent::GetEquivalentDistanceFromCurrentOffset() const
{
	const float HeightDerivedDist = GetCameraBoom()->TargetOffset.Z / CurrentOffset.Z * GetCurrentCameraState().Distance;
	return HeightDerivedDist < CameraCollisionRadius / 4.f ? CameraCollisionRadius / 4.f : HeightDerivedDist;
}


void UCustomCameraSystemComponent::ManageArmExpectedValue(const float ExpectedDistance, FVector ExpectedOffset) 
{
	FCameraState CurrState = GetCurrentCameraState();
	
	const FVector ComputedExpectedOffset = GetOffsetFromDistance(ExpectedDistance, ExpectedOffset);
	const FVector ExpectedEnd = GetCameraBoom()->GetComponentLocation() + ComputedExpectedOffset - GetCameraBoom()->GetTargetRotation().Vector() * ExpectedDistance;
	
	FHitResult CorrectiveHit;
	const bool bGoingForward			= ExpectedDistance < GetCameraBoom()->TargetArmLength;
	const bool bCollisionDelayResolved	= !CurrState.bAfterCollisionDelay || DelayedCollisionTimer <= 0.f;
	bool bCorrectiveCollision			= UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		FollowCamera->GetComponentLocation(),
		ExpectedEnd,
		CameraCollisionRadius * 1.f,
		ETraceTypeQuery::TraceTypeQuery1,
		true,
		IgnoredActorsCamera,
		EDrawDebugTrace::None,
		CorrectiveHit,
		true);

	if (bCorrectiveCollision && !bGoingForward)
	{
		if (CurrState.bAfterCollisionDelay)
		{
			DelayedCollisionTimer = CurrState.DelaySeconds;
		}
		InterpolationStart_Dist = GetCameraBoom()->TargetArmLength;
		InterpolationStart_Offset = CurrentOffset;
		InterpTimer = 0.f;
	}
	
	if (bGoingForward)
	{
		GetCameraBoom()->TargetArmLength = ExpectedDistance < CameraCollisionRadius / 4.f ? CameraCollisionRadius / 4.f : ExpectedDistance;
		CurrentOffset = ExpectedOffset;
	}
	
	else if (!bCorrectiveCollision && bCollisionDelayResolved)
	{
		GetCameraBoom()->TargetArmLength = ExpectedDistance;
		CurrentOffset = ExpectedOffset;
	}

	FVector NewProcessedOffset = GetOffsetFromDistance(GetCameraBoom()->TargetArmLength, CurrentOffset);
	GetCameraBoom()->TargetOffset = NewProcessedOffset;
}

void UCustomCameraSystemComponent::ClampLateralOffset(FVector& DesiredOffset, const FVector& HitLocation, const bool FromRight) const
{
	FLogger::InfoLog("[UNTESTED] Clamping Lateral Offset");
	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector CameraRight = FollowCamera->GetRightVector();
	const FVector ToCollision = HitLocation - CameraLocation;

	// Clamp Lateral offset to side collision hit
	const float HitRightDist = FVector::DotProduct(ToCollision, CameraRight);
	const float DesiredRightDist = FVector::DotProduct(DesiredOffset, CameraRight);
	const float Excess = DesiredRightDist - HitRightDist;

	// Only if received the collision for the shoulder offset side
	if ((DesiredRightDist > 0 && Excess > 0) || (DesiredRightDist < 0 && Excess < 0))
	{
		DesiredOffset -= CameraRight * Excess;
	}
}

float UCustomCameraSystemComponent::GetFieldOfView() const
{
	return GetFollowCamera()->FieldOfView;
}

bool UCustomCameraSystemComponent::IsActorInFrustum(const AActor* Actor) const
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
		if (SceneView != nullptr)
		{
			return SceneView->ViewFrustum.IntersectSphere(
						Actor->GetActorLocation(), Actor->GetSimpleCollisionRadius());
		}			 
	}

	return false;
}

bool UCustomCameraSystemComponent::IsComponentInFrustum(const UPrimitiveComponent* Component) const
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
		if (SceneView != nullptr)
		{
			return SceneView->ViewFrustum.IntersectSphere(
						Component->GetComponentLocation(), 50.f);
		}			 
	}

	return false;
}

bool UCustomCameraSystemComponent::IsOutsideFrustum(const AActor* Actor, const float Tolerance) const
{
	const APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!IsValid(Player))
	{
		return true;
	}
	
	const FVector Forward = GetFollowCamera()->GetForwardVector();
	const FVector Difference = (Actor->GetActorLocation() - GetFollowCamera()->GetComponentLocation()).GetSafeNormal();
	
	const float DotProduct = FVector::DotProduct(Forward, Difference);
	const float FrustumLimit = 1.0f - (Player->GetCameraSystemComponent()->GetFieldOfView() / 360.0f) - Tolerance;
	
	return (DotProduct < FrustumLimit);
	
}

float UCustomCameraSystemComponent::InterpolateFloat()
{
	const FCameraState CurrentCameraState = GetCurrentCameraState();
	const float StepPercent = FMath::Clamp(InterpTimer, 0.f, 1.f);
	switch (CurrentCameraState.InterpolationType)
	{
		case ECameraInterpolation::EASE:
			return FMath::SmoothStep(0.f, 1.f, StepPercent);

		case ECameraInterpolation::EASE_I:
			return FMath::InterpEaseIn(0.f, 1.f, StepPercent, CurrentCameraState.EaseExponent);

		case ECameraInterpolation::EASE_O:
			return FMath::InterpEaseOut(0.f, 1.f, StepPercent, CurrentCameraState.EaseExponent);
		
		case ECameraInterpolation::EASE_IO:
			return FMath::InterpEaseInOut(0.f, 1.f, StepPercent, CurrentCameraState.EaseExponent);
		
		case ECameraInterpolation::SPRING_I:
			return ElasticEaseInLerp(0.f, 1.f, StepPercent);
		
		case ECameraInterpolation::SPRING_O:
			return ElasticEaseOutLerp(0.f, 1.f, StepPercent);
		
		case ECameraInterpolation::SPRING_IO:
			return ElasticEaseInOutLerp(0.f, 1.f, StepPercent);

		case ECameraInterpolation::LINEAR:
			return FMath::Lerp(0.f, 1.f, StepPercent);
		
		case ECameraInterpolation::CUSTOM:
				return UE::Curves::BezierInterp(CurrentCameraState.BezierPoints.X,CurrentCameraState.BezierPoints.Y,CurrentCameraState.BezierPoints.Z, CurrentCameraState.BezierPoints.W, StepPercent);

		default:
			return 1.f;
	}
}

void UCustomCameraSystemComponent::PrintCurrentState()
{
	switch (GetCurrentCameraStateKey())
	{
		case ECameraStateKey::DEFAULT:
			FLogger::DebugLog("Camera State [DEFAULT]");
			break;
		case ECameraStateKey::AIM:
			FLogger::DebugLog("Camera State [AIM]");
			break;
		case ECameraStateKey::COVER:
			FLogger::DebugLog("Camera State [COVER]");
			break;
		case ECameraStateKey::LOOKAT:
			FLogger::DebugLog("Camera State [LOOKAT]");
			break;
		case ECameraStateKey::CINEMATIC:
			FLogger::DebugLog("Camera State [CINEMATIC]");
			break;
		default:
			FLogger::DebugLog("Something Wrong with Camera States");
			break;
	}
}
