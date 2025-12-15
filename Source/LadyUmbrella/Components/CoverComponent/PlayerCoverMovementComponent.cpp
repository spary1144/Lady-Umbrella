// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCoverMovementComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/CameraStateKey.h"
#include "LadyUmbrella/Components/Covercomponent/CoverEnums.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineManager.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "LadyUmbrella/Math/SecondOrderSystems/SecondOrder1DSystem.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

class UVoiceLineManager;

UPlayerCoverMovementComponent::UPlayerCoverMovementComponent()
{
	// PARAMETERS

	// Entering Covers Evaluations
	MinMovementIntensityForValidPlayerDirection = 0.2f;
	CoverActorsAcceptanceRadius = 2500;
	CoverPointsAcceptanceRadius = 500;
	bIsPlayerDirectionBasedOnCameraWhenNoInput = true;
	bIsDistanceTestActive = true;
	bUseAdditionalDistanceForCoversInCameraDir = false;
	bUseAdditionalDistanceForCoversInMovementDir = false;
	bDisableAdditionalDistanceWhenNoPlayerMovement = false;
	AdditionalDistanceForSpecialCases = 200;
	bIsPlayerDirectionTestActive = true;
	bIsMovementDirectionTestActive = true;
	ValidDotProductFromGuidingVectorForAdditionalDistanceTest = 0.6f;
	
	// Cover Movement
	SecsNeededToExitCoverWithKeyboardInput = 0.4f;
	SecsNeededToExitCoverWithControllerInput = 0.4f;
	MinDistanceWithEnvironmentToAllowAim = 400.0f;
	MaxInputAngleBackwardsToTryToExitCoverWithController = 45.0f;
	MaxInputAngleBackwardsToTryToExitCoverWithKeyboard = 35.0f;
	NotExitingCoverTimerMultiplier = 1.5f;
	TimeToMoveBetweenCoversWithMovementInput = 0.6f;
	bCanMoveBetweenCoversWithMovementInput = true;
	HeightAddedToMoveBetweenCoversUI = 10.0f;
	InitialMoveBetweenCoversRibbonColor = FLinearColor(150, 150, 150, 150);
	FinalMoveBetweenCoversRibbonColor = FLinearColor(200, 200, 200, 200);

	// Cover Camera
	AimingCancellationDistanceWhenEnteringCover = 100.0f;
	MaxAdditionalCameraDisplacementWhileMoving = 0.1f;
	ShoulderChangeCameraTransitionDuration = 0.7f;
	DistanceFromCornerEdgeToAllowCornerAim = 80.f;
	ShoulderPositionChangeCurveType = EEaseType::InOutSine;
	
	// Debug
	bShowEnterCoverEvaluationsDebugVisuals = false;
	bShowPlayerCoveredStates = false;
	bShowInsideCoverStates = false;
	bShowEnteringCoverStates = false;

	// Variables
	TimeTryingToExitCoverWithInput = 0.0f;
	CoveredCameraHorizontalRatioInitialValue = 0.0f;
	ChangeToLeftShoulderPercentageComplete = 0.0f;
	TimeTryingToMoveCoverContinuousRight = 0.0f;
	TimeTryingToMoveCoverContinuousLeft = 0.0f;
	TimeTryingToMoveCoverForwardRight = 0.0f;
	TimeTryingToMoveCoverForwardLeft = 0.0f;
	LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
	
	BetweenCoversMovementNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BetweenCoversMovementNiagaraComponent"));
	BetweenCoversMovementNiagaraComponent->SetAutoActivate(true);
	InCoverCameraHorizontalAdditionalRatioSmooth = CreateDefaultSubobject<USecondOrder1DSystem>(TEXT("InCoverCameraHorizontalRatioSmooth"));
	InCoverCameraHorizontalAdditionalRatioSmooth->ChangeConstants(1.5f, 0.9f, 0.5f);
	NumberOfParticlesInCoverMovementNiagaraSystem = 0;
	bIsCoverUiOn = false;
}

#pragma region FUNCTIONS

bool UPlayerCoverMovementComponent::CanMoveContinuousBetweenCovers(bool bGoingLeftSection0) const
{
	const bool bInIncorrectSection = !CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline != (bGoingLeftSection0 ? 0 : 1);
	const bool bHasConnection = IsValid(CoverSplineCurrentlyUsing->GetMoveBetweenCoversPoint(bGoingLeftSection0, true));
	
	if (!IsValid(CoverSplineCurrentlyUsing) || bInIncorrectSection || !bHasConnection)
	{
		return false;
	}
	
	// Checking that our input is correct.
	const FVector2D InputVectorNormalized = StoredInCoverInput.GetSafeNormal();

	return bGoingLeftSection0 ? InputVectorNormalized.X < -MIN_X_INPUT_FOR_CONTINUOUS_MOVEMENT : InputVectorNormalized.X > MIN_X_INPUT_FOR_CONTINUOUS_MOVEMENT;
}

bool UPlayerCoverMovementComponent::CanMoveForwardBetweenCovers(bool bGoingLeftSection0) const
{
	const bool bInIncorrectSection = !CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline != (bGoingLeftSection0 ? 0 : 1);
	const bool bHasConnection = IsValid(CoverSplineCurrentlyUsing->GetMoveBetweenCoversPoint(bGoingLeftSection0, false));
	
	if (!IsValid(CoverSplineCurrentlyUsing) || bInIncorrectSection || !bHasConnection)
	{
		return false;
	}

	// Checking that our input is correct.
	const FVector2D InputVectorNormalized = StoredInCoverInput.GetSafeNormal();

	if (InputVectorNormalized.Y < -MIN_Y_INPUT_FOR_FORWARD_MOVEMENT)
	{
		return false;
	}
	
	return bGoingLeftSection0 ? InputVectorNormalized.X < -MIN_X_INPUT_FOR_FORWARD_MOVEMENT : InputVectorNormalized.X > MIN_X_INPUT_FOR_FORWARD_MOVEMENT;
}

void UPlayerCoverMovementComponent::DisplayMovingBetweenCoversUI(bool bGoingLeftSection0, bool bContinuousMovement,
	float ButtonHoldPercentageComplete)
{
	if (!IsValid(PlayerCharacter) || !IsValid(CoverSplineCurrentlyUsing))
	{
		return;
	}

	SetPathColor(FLinearColor::LerpUsingHSV(InitialMoveBetweenCoversRibbonColor, FinalMoveBetweenCoversRibbonColor, ButtonHoldPercentageComplete));

	const FVector CurrentPointInCover = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline) + FVector(0, 0, HeightAddedToMoveBetweenCoversUI);
	const AAICoverEntryPoint* TargetEntryPoint = CoverSplineCurrentlyUsing->GetMoveBetweenCoversPoint(bGoingLeftSection0, bContinuousMovement);
	const FVector TargetPointInCover = TargetEntryPoint->GetOwningSpline()->FindLocationAtTime(TargetEntryPoint->GetSection()) + FVector(0, 0, HeightAddedToMoveBetweenCoversUI);

	TArray<FVector> PathPoints;

	if (bContinuousMovement) // This is a line, so it is just a begining and an end point.
	{
		const FVector LastCoverPointInPercentagePath = FMath::Lerp(CurrentPointInCover, TargetPointInCover, ButtonHoldPercentageComplete);

		PathPoints.Add(CurrentPointInCover);
		PathPoints.Add(LastCoverPointInPercentagePath);
	}
	else // Forward Movement (Turning corners).
	{
		const FVector ForwardVector = CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline).Vector();
		const float DistanceToCenterFromOrigin = FVector::DotProduct(ForwardVector, TargetPointInCover - CurrentPointInCover);
		const FVector CurveCenter = CurrentPointInCover
			+ ForwardVector * DistanceToCenterFromOrigin
			+ FVector(0.f, 0.f, (TargetPointInCover.Z - CurrentPointInCover.Z) / 2); // The curve center must be in the height in between.

		const FVector VectorFromCenterMultiplier = FVector(MOVE_COVERS_FORWARD_DISTANCE_FROM_CENTER_MULTI, MOVE_COVERS_FORWARD_DISTANCE_FROM_CENTER_MULTI, 1.f);
		const FVector VectorFromCenterToOrigin = (CurrentPointInCover - CurveCenter) * VectorFromCenterMultiplier;
		const FVector VectorFromCenterToTarget = (TargetPointInCover - CurveCenter) * VectorFromCenterMultiplier;
		
		// Adding all of our points.
		for (int subdivision = 0; subdivision < MOVE_COVERS_FORWARD_NUM_OF_PATH_POINTS; subdivision++)
		{
			const float RadiansCompleted = ButtonHoldPercentageComplete * HALF_PI
				/ static_cast<float>(MOVE_COVERS_FORWARD_NUM_OF_PATH_POINTS)
				* static_cast<float>(subdivision)
				* MOVE_COVERS_FORWARD_CIRCUMFERENCE_LENGHT_MULTIPLIER;

			const FVector LastCoverPointInPercentagePath = CurveCenter
				+ VectorFromCenterToOrigin * cos(RadiansCompleted)
				+ VectorFromCenterToTarget * sin(RadiansCompleted);

			PathPoints.Add(LastCoverPointInPercentagePath);
		}
	}

	SetPathPoints(PathPoints);
	TurnCoverUIOn();
}

void UPlayerCoverMovementComponent::HideMovingBetweenCoversUI()
{
	if (!IsValid(PlayerCharacter) || !bIsCoverUiOn)
	{
		return;
	}

	TurnCoverUIOff();
}

void UPlayerCoverMovementComponent::INTERNAL_DrawDebugEntryPointSphere(const UWorld* World, const FCoverEvaluation& CoverEvaluationToDraw) const
{
#ifdef UE_EDITOR
	if (bShowEnterCoverEvaluationsDebugVisuals)
	{
		DrawDebugSphere(World, CoverEvaluationToDraw.Position, 10, 6, FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Green, CoverEvaluationToDraw.Evaluation).ToFColor(true), false, 2.f, 2.0f);
	}
#endif
}

void UPlayerCoverMovementComponent::OnEnterInCoverState()
{
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	OnIsCoveredChanged.Broadcast(true);

	// Setting up the camera position.
	PlayerCharacter->SetPlayerGeneralState(PlayerCharacter->IsAiming() ? EPlayerState::InCoverAiming : EPlayerState::InCover);

	// Resetting the In-Cover Camera Smoothing values
	if (IsValid(CameraAdditionalHorizontalRatioSmoothComponent))
	{
		CameraAdditionalHorizontalRatioSmoothComponent->ResetSystemToValue(0.0f);
	}

	// Making the player not rotate with the camera.
	UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (IsValid(MovementComponent) && !PlayerCharacter->IsAiming())
	{
		MovementComponent->bUseControllerDesiredRotation = false;
	}
}

void UPlayerCoverMovementComponent::OnExitInCoverState()
{
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// Resetting the timer of exiting the cover with input.
	TimeTryingToExitCoverWithInput = 0.0f;

	HideMovingBetweenCoversUI();
	OnIsCoveredChanged.Broadcast(false);
			
	//  Transition when we are not aiming.
	if (!PlayerCharacter->IsAiming())
	{
		PlayerCharacter->SetPlayerGeneralState(EPlayerState::FreeMovement);
		PlayerCharacter->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::DEFAULT);
	}
	// Making the player rotate with the camera if we are aiming.
	else if (UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement())
	{
		PlayerCharacter->SetPlayerGeneralState(EPlayerState::AimingUmbrella);
		MovementComponent->bUseControllerDesiredRotation = true;
	}
}

void UPlayerCoverMovementComponent::OnEnterBaseInCoverInsideCoverState()
{
	if (GetCoveredState() == ECoveredState::InCover) // Since the InsideCoverState gets reset when we exit a cover, we check whether we need to call it or not.
	{
		OnIsCoveredChanged.Broadcast(true);

		// Setting the shoulder value. We do this here and not in SetCoveredState() cause this is also called when going from Aiming in cover to not aiming.
		if (GetTargetOrientation() == 1)
		{
			ChangeToLeftShoulderPercentageComplete = 0;
		}
		else if (GetTargetOrientation() == -1)
		{
			ChangeToLeftShoulderPercentageComplete = 1;
		}
	}
}

float UPlayerCoverMovementComponent::CalculateMaxYInputBackwardsToExitCoverWithController() const
{
	return FVector2D::DotProduct(FVector2D(0.0f,-1.0f),
		FVector2D(0, 1).GetRotated(MaxInputAngleBackwardsToTryToExitCoverWithController));
}

float UPlayerCoverMovementComponent::CalculateMaxYInputBackwardsToExitCoverWithKeyboard() const
{
	return FVector2D::DotProduct(FVector2D(0.0f,-1.0f),
		FVector2D(0, 1).GetRotated(MaxInputAngleBackwardsToTryToExitCoverWithKeyboard));
}

void UPlayerCoverMovementComponent::UpdateShoulderCameraValues(const float DeltaTime)
{
	// Updating the camera Horizontal Desired Screen Ratio.
	if (!IsValid(CameraAdditionalHorizontalRatioSmoothComponent) || !IsValid(PlayerCharacter))
	{
		return;
	}
	
	// Updating the Shoulder to Shoulder camera Transition.
	ChangeToLeftShoulderPercentageComplete += DeltaTime / ShoulderChangeCameraTransitionDuration * GetTargetOrientation();
	ChangeToLeftShoulderPercentageComplete = FMath::Clamp(ChangeToLeftShoulderPercentageComplete, 0.0f, 1.0f);
	
	const FVector PlayerRightDirection = PlayerCharacter->GetActorRightVector();
	
	float TargetRatio = CoveredCameraHorizontalRatioInitialValue;

	// Adding additional displacement if the player is moving.
	const float NormalizedInCoverSpeed = FMath::Clamp(abs(GetInCoverSpeed()), 0.0f, MaxCoverBasicMovementSpeed) / MaxCoverBasicMovementSpeed;
	CameraAdditionalHorizontalRatioSmoothComponent->UpdateWithEstimatedVelocity(FMath::Sign(GetInCoverSpeed()) * NormalizedInCoverSpeed * MaxAdditionalCameraDisplacementWhileMoving, DeltaTime);
	TargetRatio -= abs(CameraAdditionalHorizontalRatioSmoothComponent->GetSystemStoredValue());
	
	// Making the screen ratio change depending on the rotation of the camera in regard to the character.
	// This makes the camera return to the center of the screen when the camera is perpendicular to the character,
	// and makes the camera invert its position when looking at the character from its front.
	const float TargetRatioRight = MIDDLE_SCREEN_RATIO + (TargetRatio - MIDDLE_SCREEN_RATIO) * FVector::DotProduct(PlayerCharacter->GetCameraSystemComponent()->GetFollowCamera()->GetRightVector().GetSafeNormal(), -PlayerRightDirection);
	const float TargetRatioLeft = MIDDLE_SCREEN_RATIO + (TargetRatio - MIDDLE_SCREEN_RATIO) * FVector::DotProduct(PlayerCharacter->GetCameraSystemComponent()->GetFollowCamera()->GetRightVector().GetSafeNormal(), PlayerRightDirection);

	// Lerping between 2 possible TargetRatios based on the Shoulder Position.
	DesiredCoveredCameraHorizontalRatio = FMath::Lerp(TargetRatioRight, TargetRatioLeft,
		UEasingLibrary::ApplyEase(ChangeToLeftShoulderPercentageComplete, ShoulderPositionChangeCurveType));
}

void UPlayerCoverMovementComponent::SetCameraInCoverShoulderValues()
{
	if (IsValid(CameraAdditionalHorizontalRatioSmoothComponent) && IsValid(PlayerCharacter->GetCameraSystemComponent()))
	{
		// Setting the new Desired Screen Ratio value.
		PlayerCharacter->GetCameraSystemComponent()->GetCameraState(ECameraStateKey::COVER).DesiredScreenRatio.X = DesiredCoveredCameraHorizontalRatio;
	}
}

bool UPlayerCoverMovementComponent::CheckIfTryingToExitCoverWithMovementInput(const float DeltaTime)
{
	// TObjectPtr<AMainController> AMainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TWeakObjectPtr MainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!IsValid(MainController.Get()))
	{
		FLogger::ErrorLog("Invalid MainController pointer in UPlayerCoverMovementComponent.MainController");
		return false;
	}
	
	// Checking if we are trying to exit cover via keyboard movement input.
	if (MainController->GetInputDevice() == "KBM" && StoredInCoverInput.Y < CalculateMaxYInputBackwardsToExitCoverWithKeyboard())
	{
		TimeTryingToExitCoverWithInput += DeltaTime;
		return TimeTryingToExitCoverWithInput > SecsNeededToExitCoverWithKeyboardInput;
	}

	// Checking if we are trying to exit cover via controller movement input.
	if (MainController->GetInputDevice() != "KBM" && StoredInCoverInput.Y < CalculateMaxYInputBackwardsToExitCoverWithController())
	{
		TimeTryingToExitCoverWithInput += DeltaTime;
		return TimeTryingToExitCoverWithInput > SecsNeededToExitCoverWithControllerInput;
	}
	
	// We aren't trying to exit cover via input, so we reduce the exiting cover timer.
	TimeTryingToExitCoverWithInput = FMath::Max(TimeTryingToExitCoverWithInput - DeltaTime * NotExitingCoverTimerMultiplier, 0.0f);

	return false;
}

void UPlayerCoverMovementComponent::CheckIfMovementIsTryingToMoveBetweenCovers(const float DeltaTime)
{
	if (!ShouldCheckMovementBetweenCovers())
	{
		LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
		return;
	}
	
	if (CanMoveContinuousBetweenCovers(false))
	{
		TimeTryingToMoveCoverContinuousRight += DeltaTime;

		if (LastTypeOfMovementBetweenCoversAttempted != EMovementBetweenCoversState::ContinuousRight)
		{
			// We force a reset of the Particle System to avoid Ribbons connecting to each other when they shouldn't.
			HideMovingBetweenCoversUI();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::ContinuousRight;
		}
		
		DisplayMovingBetweenCoversUI(false,	true,
			TimeTryingToMoveCoverContinuousRight / TimeToMoveBetweenCoversWithMovementInput);

		if (TimeTryingToMoveCoverContinuousRight > TimeToMoveBetweenCoversWithMovementInput)
		{
			TryToMoveBetweenCovers();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
		}
	}
	else if (CanMoveContinuousBetweenCovers(true))
	{
		TimeTryingToMoveCoverContinuousLeft += DeltaTime;

		if (LastTypeOfMovementBetweenCoversAttempted != EMovementBetweenCoversState::ContinuousLeft)
		{
			// We force a reset of the Particle System to avoid Ribbons connecting to each other when they shouldn't.
			HideMovingBetweenCoversUI();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::ContinuousLeft;
		}
		
		DisplayMovingBetweenCoversUI(true, true,
			TimeTryingToMoveCoverContinuousLeft / TimeToMoveBetweenCoversWithMovementInput);

		if (TimeTryingToMoveCoverContinuousLeft > TimeToMoveBetweenCoversWithMovementInput)
		{
			TryToMoveBetweenCovers();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
		}
	}
	else if (CanMoveForwardBetweenCovers(false))
	{
		TimeTryingToMoveCoverForwardRight += DeltaTime;

		if (LastTypeOfMovementBetweenCoversAttempted != EMovementBetweenCoversState::TurningExteriorCornerRight)
		{
			// We force a reset of the Particle System to avoid Ribbons connecting to each other when they shouldn't.
			HideMovingBetweenCoversUI();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::TurningExteriorCornerRight;
		}
		
		DisplayMovingBetweenCoversUI(false, false,
			TimeTryingToMoveCoverForwardRight / TimeToMoveBetweenCoversWithMovementInput);

		if (TimeTryingToMoveCoverForwardRight > TimeToMoveBetweenCoversWithMovementInput)
		{
			TryToMoveBetweenCovers();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
		}
	}
	else if (CanMoveForwardBetweenCovers(true))
	{
		TimeTryingToMoveCoverForwardLeft += DeltaTime;

		if (LastTypeOfMovementBetweenCoversAttempted != EMovementBetweenCoversState::TurningExteriorCornerLeft)
		{
			// We force a reset of the Particle System to avoid Ribbons connecting to each other when they shouldn't.
			HideMovingBetweenCoversUI();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::TurningExteriorCornerLeft;
		}

		DisplayMovingBetweenCoversUI(true, false,
			TimeTryingToMoveCoverForwardLeft / TimeToMoveBetweenCoversWithMovementInput);

		if (TimeTryingToMoveCoverForwardLeft > TimeToMoveBetweenCoversWithMovementInput)
		{
			TryToMoveBetweenCovers();
			LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
		}
	}
	else
	{
		// Not trying to move.
		LastTypeOfMovementBetweenCoversAttempted = EMovementBetweenCoversState::NoMovement;
		HideMovingBetweenCoversUI();
	}

	// Cleaning unused timers.
	TimeTryingToMoveCoverContinuousLeft = LastTypeOfMovementBetweenCoversAttempted == EMovementBetweenCoversState::ContinuousLeft ? TimeTryingToMoveCoverContinuousLeft : 0.f;
	TimeTryingToMoveCoverContinuousRight = LastTypeOfMovementBetweenCoversAttempted == EMovementBetweenCoversState::ContinuousRight ? TimeTryingToMoveCoverContinuousRight : 0.f;
	TimeTryingToMoveCoverForwardLeft = LastTypeOfMovementBetweenCoversAttempted == EMovementBetweenCoversState::TurningExteriorCornerLeft ? TimeTryingToMoveCoverForwardLeft : 0.f;
	TimeTryingToMoveCoverForwardRight = LastTypeOfMovementBetweenCoversAttempted == EMovementBetweenCoversState::TurningExteriorCornerRight ? TimeTryingToMoveCoverForwardRight : 0.f;
}

bool UPlayerCoverMovementComponent::ShouldCheckMovementBetweenCovers() const
{
	const bool bIsInCover = IsMovementControlledByCover() && IsValid(CoverSplineCurrentlyUsing);
	const bool bIsAlreadyMovingBetweenCovers = GetInsideCoverState() == EInsideCoverState::MovingBetweenCovers;
	const bool bIsCorrectSection = SectionOfCurrentlyUsingSpline == 0.0f || SectionOfCurrentlyUsingSpline == 1.0f;

	return bIsInCover && !bIsAlreadyMovingBetweenCovers && bIsCorrectSection;
}

// I honestly do not care to make it any better
//this is now truer than ever
TArray<AActor*> UPlayerCoverMovementComponent::GetCoversFiltered() 
{
    TArray<AActor*> All;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoverParent::StaticClass(), All);

    TArray<AActor*> Out;
    if (!PlayerCharacter) return Out;

    const FVector P = PlayerCharacter->GetActorLocation();

    const float MaxVerticalDelta = 120.f;
    const float MaxHorizontal    = 1000.f;
    const float MaxHorizontalSq  = MaxHorizontal > 0.f ? MaxHorizontal*MaxHorizontal : -1.f;

    for (AActor* A : All)
    {
        if (!IsValid(A)) continue;

        const FVector L = A->GetActorLocation();

        if (FMath::Abs(L.Z - P.Z) > MaxVerticalDelta)
            continue;

        if (MaxHorizontalSq > 0.f && FVector::DistSquared2D(L, P) > MaxHorizontalSq)
            continue;

        Out.AddUnique(A);
    }

    return Out;
}

void UPlayerCoverMovementComponent::CheckIfContinueInCoverState()
{
	if (!FGlobalPointers::PlayerCharacter->IsPlayerInCombat())
	{
		return;
	}
	
	FGlobalPointers::PlayerCharacter->PlayVoiceLine(EVoiceLineState::IN_COVER);
}

bool UPlayerCoverMovementComponent::EvaluateCoverPosition(
	FCoverEvaluation& CoverEvaluationStruct,
	const FVector& PlayerLocation,
	const FVector& PlayerDirection,
	const FVector& CameraDirection,
	const bool bPlayerInputDirectionWasValid,
	float& MaxEvaluationSoFar) const
{
	const FVector VectorXYToPoint = FVector(CoverEvaluationStruct.Position.X - PlayerLocation.X, CoverEvaluationStruct.Position.Y - PlayerLocation.Y, 0.0f);
	const float DistanceInZPlane = abs(CoverEvaluationStruct.Position.Z - PlayerLocation.Z);
	const float DistanceSquaredXYToPoint = VectorXYToPoint.SizeSquared2D();
	const bool bIsAnyAdditionalDistanceActive = bUseAdditionalDistanceForCoversInCameraDir || bUseAdditionalDistanceForCoversInMovementDir;
	const bool bShouldDisableAdditionalDistance = bDisableAdditionalDistanceWhenNoPlayerMovement && !bPlayerInputDirectionWasValid;
	const bool bUsingAdditionalDistanceTest = bIsDistanceTestActive && bIsAnyAdditionalDistanceActive && !bShouldDisableAdditionalDistance;

	const float MaxDistancePossibleToAPoint = CoverPointsAcceptanceRadius + (bUsingAdditionalDistanceTest ?	AdditionalDistanceForSpecialCases : 0.0f);
	
	// Filtering those that are too far away (in the XY plane OR the Z axis) from the player to be valid.
	const bool bIsTooFarAwayInXYPlane = DistanceSquaredXYToPoint > FMath::Square(MaxDistancePossibleToAPoint);
	const bool bIsTooFarAwayInZAxis = DistanceInZPlane > MAX_Z_DISTANCE_FOR_COVER_TO_BE_SELECTABLE;
	
	if (bIsDistanceTestActive && (bIsTooFarAwayInXYPlane || bIsTooFarAwayInZAxis))
	{
		CoverEvaluationStruct.Evaluation = 0;
		return false;
	}
	
	const float DistanceXYToPoint = FMath::Sqrt(DistanceSquaredXYToPoint);
    
    if (bIsDistanceTestActive)
    {
    	if (IsValid(DistanceTestEvaluationCurve))
    	{
    		// The closer to the player, the more valid it is.
    		CoverEvaluationStruct.Evaluation *= FMath::Clamp(DistanceTestEvaluationCurve->GetFloatValue(DistanceXYToPoint), 0.0f, 1.0f);
    	}
    	else
    	{
    		FLogger::ErrorLog("DistanceTestEvaluationCurve has not been set, cannot perform Distance Evaluation Test.");
    	}
    	
    	if (CoverEvaluationStruct.Evaluation <= MaxEvaluationSoFar)
    	{
    		return false;
    	}
    }
	
	// Additional checks if the Additional Distance is active.
	if (bUsingAdditionalDistanceTest && DistanceXYToPoint > CoverPointsAcceptanceRadius)
	{
		if (bUseAdditionalDistanceForCoversInCameraDir &&
			FVector::DotProduct(VectorXYToPoint.GetSafeNormal2D(),CameraDirection) < ValidDotProductFromGuidingVectorForAdditionalDistanceTest)
		{
			CoverEvaluationStruct.Evaluation = 0.0f;
		}
		if (bUseAdditionalDistanceForCoversInMovementDir &&
			FVector::DotProduct(VectorXYToPoint.GetSafeNormal2D(),PlayerDirection) < ValidDotProductFromGuidingVectorForAdditionalDistanceTest)
		{
			CoverEvaluationStruct.Evaluation = 0.0f;
		}

		if (CoverEvaluationStruct.Evaluation <= MaxEvaluationSoFar)
		{
			return false;
		}
	}

	const FVector PointFacingDirection = CoverEvaluationStruct.Cover->FindForwardRotationAtTime(CoverEvaluationStruct.Section).Vector().GetSafeNormal2D();

	if (bIsPlayerDirectionTestActive)
	{
		const float DotProductSharesAimingDirection = FVector::DotProduct(PlayerDirection.GetSafeNormal2D(),PointFacingDirection);

		// If the direction of the player is similar to that of the entry point.
		// We perform a different test depending on whether the player had a valid direction input.
		if (bPlayerInputDirectionWasValid && IsValid(PlayerDirectionTestEvaluationCurveWithValidPlayerInput))
		{
			CoverEvaluationStruct.Evaluation *= FMath::Clamp(
				PlayerDirectionTestEvaluationCurveWithValidPlayerInput->GetFloatValue((DotProductSharesAimingDirection + 1.f) / 2.f), 0.f, 1.f);
		}
		else if (!bPlayerInputDirectionWasValid && IsValid(PlayerDirectionTestEvaluationCurveWhenPlayerInputInvalid))
		{
			CoverEvaluationStruct.Evaluation *= FMath::Clamp(
				PlayerDirectionTestEvaluationCurveWhenPlayerInputInvalid->GetFloatValue((DotProductSharesAimingDirection + 1.f) / 2.f), 0.f, 1.f);
		}
		else
		{
			CoverEvaluationStruct.Evaluation = 0.f;
			FLogger::ErrorLog("Player Direction Test Evaluation Curves were not set, cannot perform Player Direction Test.");
		}

		if (CoverEvaluationStruct.Evaluation <= MaxEvaluationSoFar)
		{
			return false;
		}
	}

	// This test evaluates the dot product of the player's movement towards the cover compared to the cover's direction.
	// WE DON'T DO THIS IF WE ARE SUPER CLOSE TO THE PLAYER (or it would be possible to be on top of the point and still miss it)
	if (bIsMovementDirectionTestActive && DistanceXYToPoint > MIN_DISTANCE_BETWEEN_PLAYER_AND_COVER_TO_PERFORM_MOVEMENT_TEST)
	{
		const float DotProductFacingDirectionAndMovement = FVector::DotProduct(VectorXYToPoint.GetSafeNormal2D(), PointFacingDirection);

		if (IsValid(MovementDirectionTestEvaluationCurve))
		{
			CoverEvaluationStruct.Evaluation *= FMath::Clamp(
				MovementDirectionTestEvaluationCurve->GetFloatValue((DotProductFacingDirectionAndMovement + 1) / 2), 0.f, 1.f);
		}
		else
		{
			CoverEvaluationStruct.Evaluation = 0.f;
			FLogger::ErrorLog("Movement Direction Test Evaluation Curve was not set, cannot perform Movement Direction Test.");
		}
	}
	
	if (CoverEvaluationStruct.Evaluation > MaxEvaluationSoFar)
	{
		MaxEvaluationSoFar = CoverEvaluationStruct.Evaluation;
		return true;
	}

	return false;
}

bool UPlayerCoverMovementComponent::PlayerTryToEnterCover()
{
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}
	
	// We try to find a new cover to go into.
	const UWorld* World = GetWorld();
	CoverParents = GetCoversFiltered();

	
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector PlayerDirection = PlayerCharacter->GetMovementComponent()->GetLastInputVector();
	bool bPlayerInputDirectionWasValid = true;

	const UCameraComponent* Camera = PlayerCharacter->GetCameraSystemComponent()->GetFollowCamera().Get();
	const FVector CameraDirection = IsValid(Camera) ? Camera->GetForwardVector().GetSafeNormal2D() : FVector::ForwardVector;
	
	// If we have no concrete input for where to look for cover, we need a different way to get a direction.
	if (PlayerDirection.SizeSquared() < FMath::Square(MinMovementIntensityForValidPlayerDirection))
	{
		bPlayerInputDirectionWasValid = false;

		if (bIsPlayerDirectionBasedOnCameraWhenNoInput && IsValid(Camera))
		{
			PlayerDirection = CameraDirection;
		}
		else // We use the character's direction.
		{
			PlayerDirection = PlayerCharacter->GetActorForwardVector().GetSafeNormal2D();
		}
	}

	PlayerDirection = FVector(PlayerDirection.X, PlayerDirection.Y, 0).GetSafeNormal();

#ifdef UE_EDITOR
	if (bShowEnterCoverEvaluationsDebugVisuals)
	{
		const FVector CorrectedPlayerPos = PlayerLocation - FVector(0.0f, 0.0f, PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		
		// Painting the player direction.
		DrawDebugLine(World, CorrectedPlayerPos, CorrectedPlayerPos + PlayerDirection * (CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases), FColor::Orange, false, 3.0f, 2.0f);
		// Painting the area of Splines that we are evaluating.
		DrawDebugCylinder(World, CorrectedPlayerPos, CorrectedPlayerPos + FVector(0, 0, 50), CoverActorsAcceptanceRadius, 100, FColor::Silver, false, 3.0f, 2.0f);
		// Painting the Additional Distance Lines.
		if (bUseAdditionalDistanceForCoversInCameraDir && IsValid(Camera))
		{
			const FVector2D CameraDirection2DRight = FVector2D(CameraDirection.X, CameraDirection.Y).GetRotated(FMath::RadiansToDegrees(acos(ValidDotProductFromGuidingVectorForAdditionalDistanceTest)));
			const FVector2D CameraDirection2DLeft = FVector2D(CameraDirection.X, CameraDirection.Y).GetRotated(-FMath::RadiansToDegrees(acos(ValidDotProductFromGuidingVectorForAdditionalDistanceTest)));
			
			DrawDebugLine(World, CorrectedPlayerPos, CorrectedPlayerPos + FVector(CameraDirection2DRight, 0.0f) * (CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases), FColor::Cyan, false, 3.0f, 2.0f);
			DrawDebugLine(World, CorrectedPlayerPos, CorrectedPlayerPos + FVector(CameraDirection2DLeft, 0.0f) * (CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases), FColor::Cyan, false, 3.0f, 2.0f);
			DrawDebugCircleArc(World, CorrectedPlayerPos, CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases, CameraDirection, acos(ValidDotProductFromGuidingVectorForAdditionalDistanceTest), 10, FColor::Cyan, false, 3.0f, 2.0f);
		}
		if (bUseAdditionalDistanceForCoversInMovementDir && IsValid(Camera))
		{
			const FVector2D MovementDirection2DRight = FVector2D(PlayerDirection.X, CameraDirection.Y).GetRotated(FMath::RadiansToDegrees(acos(ValidDotProductFromGuidingVectorForAdditionalDistanceTest)));
			const FVector2D MovementDirection2DLeft = FVector2D(PlayerDirection.X, PlayerDirection.Y).GetRotated(-FMath::RadiansToDegrees(acos(ValidDotProductFromGuidingVectorForAdditionalDistanceTest)));
			
			DrawDebugLine(World, CorrectedPlayerPos, CorrectedPlayerPos + FVector(MovementDirection2DRight, 0.0f) * (CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases), FColor::Emerald, false, 3.0f, 2.0f);
			DrawDebugLine(World, CorrectedPlayerPos, CorrectedPlayerPos + FVector(MovementDirection2DLeft, 0.0f) * (CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases), FColor::Emerald, false, 3.0f, 2.0f);
			DrawDebugCircleArc(World, CorrectedPlayerPos, CoverPointsAcceptanceRadius + AdditionalDistanceForSpecialCases - 1.0f, PlayerDirection, acos(ValidDotProductFromGuidingVectorForAdditionalDistanceTest), 10, FColor::Emerald, false, 3.0f, 2.0f);
		}

		// Painting the valid range.
		DrawDebugCircle(World, CorrectedPlayerPos, CoverPointsAcceptanceRadius, 50, FColor::Red, false, 3.0f, 2.0f, 0.0f, FVector::ForwardVector, FVector::LeftVector);
	}
#endif

	TArray<FCoverEvaluation> CoverEvaluations;

	// Filling our CoverEvaluationsStruct
	for (AActor* DetectedActor : CoverParents)
	{
		const ACoverParent* CoverParent = Cast<ACoverParent>(DetectedActor);

		// Failed casts shouldn't continue.
		if (!IsValid(CoverParent) ||
			(CoverParent->GetActorLocation() - PlayerLocation).SizeSquared() > FMath::Square(CoverActorsAcceptanceRadius))
		{
			continue;
		}

		// Cover Splines are components of ACoverParent actors.
		TSet<UActorComponent*> Components = CoverParent->GetComponents();

		for (UActorComponent* Comp : Components)
		{
			UCoverSpline* Cover = Cast<UCoverSpline>(Comp);
			if (!IsValid(Cover))
			{
				continue;
			}

			TArray<FVector> BufferEntryPoints;
			float SectionLength = 0;
		
			Cover->CalculatePlayerEntryPoints(BufferEntryPoints, SectionLength);
			const int NumberOfPlayerEntryPoints = BufferEntryPoints.Num();

			for (int EntryPointIndex = 0; EntryPointIndex < NumberOfPlayerEntryPoints; EntryPointIndex++)
			{
				CoverEvaluations.Emplace(FCoverEvaluation(Cover,BufferEntryPoints[EntryPointIndex],
					SectionLength * static_cast<float>(EntryPointIndex),1.0f));
			}	
		}
	}

	if (CoverEvaluations.Num() == 0) // If we found no availible covers.
	{
		return false;
	}
	
	int ResultingIndex = -1;
	float MaxEvaluationSoFar = 0;
	
	// Evaluating every position.
	for (int Index = 0; Index < CoverEvaluations.Num(); Index++)
	{
		if (EvaluateCoverPosition(CoverEvaluations[Index], PlayerLocation, PlayerDirection, CameraDirection, bPlayerInputDirectionWasValid, MaxEvaluationSoFar))
		{
			ResultingIndex = Index;
		}
		
		INTERNAL_DrawDebugEntryPointSphere(World, CoverEvaluations[Index]);
	}
	
		
	// If we found a valid point, we move towards it.
	if (ResultingIndex != -1)
	{
		NotifyBeginTravellingToCover(CoverEvaluations[ResultingIndex].Cover, CoverEvaluations[ResultingIndex].Section);
		
#ifdef UE_EDITOR
		if (bShowEnterCoverEvaluationsDebugVisuals)
		{
			DrawDebugLine(World, PlayerLocation, CoverEvaluations[ResultingIndex].Position, FColor::Green, false, 3.0f, 2.0f);
		}
#endif
		
		return true;
	}
	
	return false;
}

bool UPlayerCoverMovementComponent::TryToMoveBetweenCovers()
{
	//FGlobalPointers::MainController->ParseInputDevice();

	if (FGlobalPointers::PlayerCharacter->GetPlayerGeneralState() == EPlayerState::ControlsDisabled ||
		FGlobalPointers::PlayerCharacter->GetPlayerGeneralState() != EPlayerState::InCover)
	{
		return false;
	}
	
	if (!ShouldCheckMovementBetweenCovers() || !IsValid(PlayerCharacter))
	{
		return false;
	}
	
	if (CanMoveContinuousBetweenCovers(true))
	{
		TargetOrientation = -1;
		MovementBetweenCoversState = EMovementBetweenCoversState::ContinuousLeft;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointContinuous_0()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointContinuous_0()->GetSection());
	}
	else if (CanMoveContinuousBetweenCovers(false))
	{
		TargetOrientation = 1;
		MovementBetweenCoversState = EMovementBetweenCoversState::ContinuousRight;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointContinuous_1()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointContinuous_1()->GetSection());
	}
	else if (CanMoveForwardBetweenCovers(true))
	{
		TargetOrientation = -1;
		MovementBetweenCoversState = EMovementBetweenCoversState::TurningExteriorCornerLeft;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointForward_0()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointForward_0()->GetSection());
	}
	else if (CanMoveForwardBetweenCovers(false))
	{
		TargetOrientation = 1;
		MovementBetweenCoversState = EMovementBetweenCoversState::TurningExteriorCornerRight;
		NotifyBeginTravellingToCover(CoverSplineCurrentlyUsing->GetReachablePointForward_1()->GetOwningSpline(),
				CoverSplineCurrentlyUsing->GetReachablePointForward_1()->GetSection());
	}

	HideMovingBetweenCoversUI();
	return true;
}

bool UPlayerCoverMovementComponent::ShouldAimInCoverOverLeftShoulder() const
{
	const UCoverSpline* CoverToTest = IsTravellingToCover() ? GetCoverSplineTravellingTowards() : GetCoverSplineCurrentlyUsing();
	const bool bCanEverAimOverLeftShoulder = IsValid(CoverToTest)
		&& !CoverToTest->IsSinglePointCover()
		&& !CoverToTest->IsLoop();
	
	return bCanEverAimOverLeftShoulder && CheckForTallCorner(true);
}

void UPlayerCoverMovementComponent::TurnCoverUIOn()
{
	if (!IsValid(BetweenCoversMovementNiagaraComponent) || bIsCoverUiOn)
	{
		return;
	}

	// OnMoveBetweenCoverDelegate.Broadcast();
	// BetweenCoversMovementNiagaraComponent->ResetSystem();
	BetweenCoversMovementNiagaraComponent->SetVariableBool(NIAGARA_SYSTEM_VISIBLE_PATH_VARIABLE_NAME, true);
	bIsCoverUiOn = true;
}

void UPlayerCoverMovementComponent::TurnCoverUIOff()
{
	if (!IsValid(BetweenCoversMovementNiagaraComponent))
	{
		return;
	}

	bIsCoverUiOn = false;
	CurrentRibbonID++;
	BetweenCoversMovementNiagaraComponent->SetVariableInt(RIBBON_ID_NIAGARA_SYSTEM_VARIABLE_NAME, CurrentRibbonID);
	BetweenCoversMovementNiagaraComponent->SetVariableBool(NIAGARA_SYSTEM_VISIBLE_PATH_VARIABLE_NAME, false);
}

void UPlayerCoverMovementComponent::SetPathPoints(const TArray<FVector>& PathPoints)
{
	if (!IsValid(BetweenCoversMovementNiagaraComponent) || PathPoints.IsEmpty())
	{
		return;
	}
 
	BetweenCoversMovementNiagaraComponent->SetVariableInt(NUMBER_OF_NIAGARA_SYSTEM_PATH_POINTS_VARIABLE_NAME, PathPoints.Num());
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(BetweenCoversMovementNiagaraComponent, NIAGARA_SYSTEM_PATH_POINTS_ARRAY_VARIABLE_NAME, PathPoints);
 
	if (NumberOfParticlesInCoverMovementNiagaraSystem != PathPoints.Num())
	{
		NumberOfParticlesInCoverMovementNiagaraSystem = PathPoints.Num();
		BetweenCoversMovementNiagaraComponent->ResetSystem();
		BetweenCoversMovementNiagaraComponent->ActivateSystem();
	}
}
 
void UPlayerCoverMovementComponent::SetPathColor(const FLinearColor& Color)
{
	if (!IsValid(BetweenCoversMovementNiagaraComponent))
	{
		return;
	}
 
	BetweenCoversMovementNiagaraComponent->SetVariableLinearColor(MOVE_BETWEEN_COVERS_RIBBON_COLOR_VARIABLE_NAME, Color);
}

#pragma endregion

#pragma region VIRTUAL FUNCTIONS

ECoveredState UPlayerCoverMovementComponent::SetCoveredState(const ECoveredState NewCoveredState)
{
	// We don't wanna do anything when we aren't really changing States.
	if (GetCoveredState() == NewCoveredState || !IsValid(PlayerCharacter))
	{
		return GetCoveredState();
	}

	// We do things before the state is changed in the Super::SetCoveredState.
	// Exit State Actions
	switch (GetCoveredState())
	{
	case ECoveredState::NoCover:
		break;
	case ECoveredState::InCover:
		{
			OnExitInCoverState();
			GetWorld()->GetTimerManager().ClearTimer(TimerHandleVoiceLine);
		}
		break;
	case ECoveredState::EnteringCover:
		// If we are cancelling entering a cover, we also cancel the camera (exclusively).
		if (NewCoveredState == ECoveredState::NoCover)
		{
			PlayerCharacter->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::DEFAULT);
		}
		break;
	case ECoveredState::ExitingCover:
		break;
	}

	// Enter State Actions
	switch (NewCoveredState)
	{
	case ECoveredState::NoCover:
		break;
	case ECoveredState::InCover:
		OnEnterInCoverState();
		break;
	case ECoveredState::EnteringCover:
		if (!GenericCharacter->IsAiming())
		{
			PlayerCharacter->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::COVER);
		}
		break;
	case ECoveredState::ExitingCover:
		break;
	}
	
	return Super::SetCoveredState(NewCoveredState);
}

EInsideCoverState UPlayerCoverMovementComponent::SetInsideCoverState(const EInsideCoverState NewInsideCoverState)
{
	// We don't wanna do anything when we aren't really changing States.
	if (InsideCoverState == NewInsideCoverState)
	{
		return InsideCoverState;
	}

	// We do things before the state is changed in the Super::SetInsideCoverState.
	if (IsValid(PlayerCharacter))
	{
		// Exit State Actions
		switch (InsideCoverState)
		{
		case EInsideCoverState::BaseCovered:
			OnIsCoveredChanged.Broadcast(false);
			break;
		case EInsideCoverState::Aiming:
			OnIsCoveredChanged.Broadcast(true);
			break;
		case EInsideCoverState::MovingBetweenCovers:
			break;
		}

		// Enter State Actions
		switch (NewInsideCoverState)
		{
		case EInsideCoverState::BaseCovered:
			OnEnterBaseInCoverInsideCoverState();
			break;
		case EInsideCoverState::Aiming:
			OnIsCoveredChanged.Broadcast(true);
			break;
		case EInsideCoverState::MovingBetweenCovers:
			break;
		}
	}
	
	return Super::SetInsideCoverState(NewInsideCoverState);
}

bool UPlayerCoverMovementComponent::NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter)
{
	// Checking if we need to stop aiming.
	if (IsValid(CoverToEnter) && IsValid(PlayerCharacter) && PlayerCharacter->IsAiming())
	{
		// We know that CoverSplineTravellingTowards is Valid because it is checked in the Super.
		const FVector EntryLocation = CoverToEnter->FindLocationAtTime(SectionToEnter);

		const float DistanceToEntrySquared = FVector::DistSquared2D(EntryLocation, PlayerCharacter->GetActorLocation());
		
		// If we are too far away, we cancel the aim.
		if (DistanceToEntrySquared > FMath::Square(AimingCancellationDistanceWhenEnteringCover))
		{
			PlayerCharacter->ResetAiming();
		}
	}

	if (FGlobalPointers::PlayerCharacter->IsPlayerInCombat())
	{
		FGlobalPointers::PlayerCharacter->PlayVoiceLine(EVoiceLineState::TAKING_COVER);
	}
	
	return Super::NotifyBeginTravellingToCover(CoverToEnter, SectionToEnter);
}

void UPlayerCoverMovementComponent::NotifyReachedCoverTravellingTowards()
{
	Super::NotifyReachedCoverTravellingTowards();

	// If the player is trying to aim when they reach the cover, but for some reason they aren't doing it, we enable aim.
	if (IsValid(PlayerCharacter) && !PlayerCharacter->IsAiming() && PlayerCharacter->IsAimButtonHeld())
	{
		PlayerCharacter->StartAiming();
	}

	if (!TimerDelegateVoiceLine.IsBound())
	{
		TimerDelegateVoiceLine.BindUObject(this, &UPlayerCoverMovementComponent::CheckIfContinueInCoverState);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandleVoiceLine,
		TimerDelegateVoiceLine,
		TIME_IN_COVER_VOICE_LINE,
		true
	);
}

void UPlayerCoverMovementComponent::BaseInCoverTickFunctionality(float DeltaTime)
{
	Super::BaseInCoverTickFunctionality(DeltaTime);

	SetCameraInCoverShoulderValues();
}

void UPlayerCoverMovementComponent::AimInCoverTickFunctionality(float DeltaTime)
{
	Super::AimInCoverTickFunctionality(DeltaTime);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	const UCameraComponent* Camera = PlayerCharacter->GetCameraSystemComponent()->GetFollowCamera().Get();
	const FVector CameraPosition = Camera->GetComponentLocation();
	const FVector ForwardVector = Camera->GetForwardVector();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(PlayerCharacter);
	
	FHitResult HitResult;
	FVector AimingDirection;
	
	const bool bIsAimingAtAnObject = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraPosition,
		CameraPosition + ForwardVector * DISTANCE_OF_LINE_TRACE_WHEN_CHECKING_AIMING_DIRECTION,
		ECollisionChannel::ECC_Camera,
		CollisionParams);
	
	if (bIsAimingAtAnObject && IsValid(GetCoverSplineCurrentlyUsing()))
	{
		// If the hit was far away enough.
		const bool bWasHitFarAwayEnough = HitResult.Distance > MinDistanceWithEnvironmentToAllowAim;
		
		// If the hit actor is the cover that we are shooting from (from a close-up distance).
		const bool bIsAimingAtCoverWeAreShootingFrom = HitResult.GetActor() == GetCoverSplineCurrentlyUsing()->TryGetParent();
		
		// If it is not the invisible cover that we are shooting from (from a close-up distance).
		const bool bIsAimingAtCoverWithEnvironmentTag = !HitResult.GetActor()->ActorHasTag(GetCoverSplineCurrentlyUsing()->ENVIRONMENT_NO_COVER_TAG);		
		
		const bool bAimingDirectionValid = bWasHitFarAwayEnough || (!bIsAimingAtCoverWeAreShootingFrom && !bIsAimingAtCoverWithEnvironmentTag);
		
		AimingDirection = bAimingDirectionValid ? HitResult.Location - PlayerCharacter->GetActorLocation()
			: PlayerCharacter->GetCoverMovementComponent()->INVALID_IN_COVER_AIMING_DIRECTION;
	}
	else // If we don't hit anything, we aim freely in the direction of the camera.
	{
		AimingDirection = CameraPosition + ForwardVector * DISTANCE_OF_LINE_TRACE_WHEN_CHECKING_AIMING_DIRECTION - PlayerCharacter->GetActorLocation();
	}
	
	PlayerCharacter->GetCoverMovementComponent()->SetInCoverAimingDirection(AimingDirection);
}

void UPlayerCoverMovementComponent::LerpingToCoverTickFunctionality(float DeltaTime)
{
	Super::LerpingToCoverTickFunctionality(DeltaTime);
	
	SetCameraInCoverShoulderValues();
}

void UPlayerCoverMovementComponent::EnteringCoverTickFunctionality(float DeltaTime)
{
	const bool bEnteringCoverTickAlreadyManaged = GetEnteringCoverState() == EEnteringCoverState::InterpolatingToCover
		|| GetEnteringCoverState() == EEnteringCoverState::InterpolatingWithSlideToCover;
	if (bEnteringCoverTickAlreadyManaged)
	{
		Super::EnteringCoverTickFunctionality(DeltaTime);
		return; // This logic is managed inside the parent script CharacterCoverMovementComponent.
	}

	SetCameraInCoverShoulderValues();

	const FVector TargetLocation = CoverSplineTravellingTowards->FindLocationAtTime(TemporaryUsedSectionOfSplineTravellingTowards);
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	const FVector VectorToTarget = TargetLocation - PlayerLocation;
	
	if (VectorToTarget.SizeSquared2D() < FMath::Square(MinDistanceToBeginInterpolationToCover))
	{
		NotifyReachedCoverTravellingTowards(); // This will begin an interpolation without a slide.
		return;
	}
	
	const float SpeedTowardsCover = FVector::DotProduct(PlayerCharacter->GetMovementComponent()->Velocity, VectorToTarget.GetSafeNormal());
	const bool bCanStartSlide = SpeedTowardsCover > MinSpeedToBeginSlidingToCover
		&& VectorToTarget.SizeSquared2D() < FMath::Square(MinDistanceToBeginSlidingToCover)
		&& VectorToTarget.SizeSquared2D() > FMath::Square(MaxDistanceToBeginSlidingToCover);

	if (bCanStartSlide)
	{
		BeginEnteringCoverSlidingInterpolation();
		return;
	}

	// Else we move towards the cover.
	PlayerCharacter->AddMovementInput(FVector(TargetLocation.X - PlayerLocation.X, TargetLocation.Y - PlayerLocation.Y, 0).GetSafeNormal());
}

void UPlayerCoverMovementComponent::MoveBetweenCoversTickFunctionality(float DeltaTime)
{
	Super::MoveBetweenCoversTickFunctionality(DeltaTime);

	SetCameraInCoverShoulderValues();
}

void UPlayerCoverMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GenericCharacter);

	if (IsValid(PlayerCharacter))
	{
		CameraAdditionalHorizontalRatioSmoothComponent = GetInCoverCameraHorizontalAdditionalRatioSmooth();
		CoveredCameraHorizontalRatioInitialValue = PlayerCharacter->GetCameraSystemComponent()->GetCameraState(ECameraStateKey::COVER).DesiredScreenRatio.X;
	}
}

void UPlayerCoverMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

#if UE_EDITOR
	if (bShowPlayerCoveredStates || bShowInsideCoverStates || bShowEnteringCoverStates)
	{
		FString ResultDebugString = FString(TEXT(""));

		if (bShowPlayerCoveredStates)
		{
			ResultDebugString += UEnum::GetValueAsString(GetCoveredState()) + FString(TEXT(", "));
		}
		if (bShowInsideCoverStates)
		{
			ResultDebugString += UEnum::GetValueAsString(GetInsideCoverState()) + FString(TEXT(", "));
		}
		if (bShowEnteringCoverStates)
		{
			ResultDebugString += UEnum::GetValueAsString(GetEnteringCoverState()) + FString(TEXT(", "));
		}
		
		FLogger::DebugLog(ResultDebugString);
	}
#endif
	
	UpdateShoulderCameraValues(DeltaTime);
	
	const bool bCanExitCoverWithInput = IsMovementControlledByCover() && (GetInsideCoverState() == EInsideCoverState::BaseCovered || GetInsideCoverState() == EInsideCoverState::Aiming);
	if (bCanExitCoverWithInput && CheckIfTryingToExitCoverWithMovementInput(DeltaTime))
	{
		NotifyExitCover();
		return;
	}
	
	CheckIfMovementIsTryingToMoveBetweenCovers(DeltaTime);
}

bool UPlayerCoverMovementComponent::TryToTransitionOutOfCover()
{
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}

	// If we need to have an Exit Transition because of an open shield while in Cover.
	if (IsValid(PlayerCharacter->GetWeapon()) && PlayerCharacter->GetWeapon()->IsOpen())
	{
		ExitCoverTransitionDirection = -CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline).Vector();
		return true;
	}
	
	// Do we need an Exit Transition?
	const float CurrentSplineLength = CoverSplineCurrentlyUsing->GetSplineLength();

	const bool bIsAtLeftCoverEdge = CoverSplineCurrentlyUsing->IsSinglePointCover() || SectionOfCurrentlyUsingSpline * CurrentSplineLength < MAX_DISTANCE_FROM_EDGE_TO_PERFORM_EXIT_TRANSITION;
	const bool bIsAtRightCoverEdge = CoverSplineCurrentlyUsing->IsSinglePointCover() || (1 - SectionOfCurrentlyUsingSpline) * CurrentSplineLength < MAX_DISTANCE_FROM_EDGE_TO_PERFORM_EXIT_TRANSITION;
	const bool bExitingCoverThroughTheLeft = bIsAtLeftCoverEdge && (StoredInCoverInput.X < -MIN_X_INPUT_FOR_COVER_REACTION || TargetOrientation < 0);
	const bool bExitingCoverThroughTheRight = bIsAtRightCoverEdge && (StoredInCoverInput.X > MIN_X_INPUT_FOR_COVER_REACTION || TargetOrientation > 0);
	const bool bIsTryingToExitForward = StoredInCoverInput.Y > 0.15f;
	
	if (!CoverSplineCurrentlyUsing->IsLoop() && bIsTryingToExitForward)
	{
		if (bIsAtLeftCoverEdge && bExitingCoverThroughTheLeft)
		{
			ExitCoverTransitionDirection = CoverSplineCurrentlyUsing->FindLeftRotationAtTime(0).Vector();
			return true;
		}
		
		if (bIsAtRightCoverEdge && bExitingCoverThroughTheRight)
		{
			ExitCoverTransitionDirection = CoverSplineCurrentlyUsing->FindRightRotationAtTime(CoverSplineCurrentlyUsing->IsSinglePointCover() ? 0 : 1).Vector();
			return true;
		}
	}

	// Else we don't need an Exit Transition
	return false;
}

#pragma endregion
