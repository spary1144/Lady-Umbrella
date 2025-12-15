// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCoverMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CoverComponent/CoverEnums.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverSpline.h"
#include "LadyUmbrella/Math/EasingLibrary.h"
#include "LadyUmbrella/Math/SecondOrderSystems/SecondOrder1DSystem.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

// Sets default values for this component's properties
UCharacterCoverMovementComponent::UCharacterCoverMovementComponent()
{
	// Variable Initialization
	CoveredState = ECoveredState::NoCover;
	InsideCoverState = EInsideCoverState::BaseCovered;

	InterpolateToCoverSpeed = 500.0f;
	SlideToCoverSpeed = 800.0f;
	MinDistanceToBeginInterpolationToCover = 75.0f;
	MinDistanceToBeginSlidingToCover = 450.0f;
	MaxDistanceToBeginSlidingToCover = 275.0f;
	MinSpeedToBeginSlidingToCover = 400.0f;
	MaxCoverBasicMovementSpeed = 300.0f;
	CoverMovementAcceleration = 800.0f;
	CoverMovementBreakingAcceleration = 1600.0f;
	MovingBetweenCoversContinuousTargetSpeed = 350.0f;
	CoverMovementSpeed = 0.0f;
	DistanceFromCornerEdgeToAllowCornerAim = 60.0f;
	bIsAimingFromTallCover = false;
	MaxHorizontalDisplacementWhenAimingFromTallCorner = 60.0f;
	CurrentHorizontalDisplacementWhenAimingFromTallCover = 0.0f;
	HorizontalDisplacementSpeedWhenAimingFromTallCorner = 300.0f;
	
	TargetOrientation = 1.0f;
	StoredInCoverInput = FVector2D::Zero();
	
	CoverSplineCurrentlyUsing = nullptr;
	SectionOfCurrentlyUsingSpline = INVALID_PERCENTAGE_COMPLETE_VALUE;
	CoverSplineTravellingTowards = nullptr;
	TemporaryUsedSectionOfSplineTravellingTowards = INVALID_PERCENTAGE_COMPLETE_VALUE;

	ToCoverTravelCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
	ToCoverTravelDuration = 0;
	
	ExitCoverTransitionCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
	ExitCoverTransitionDirection = FVector::ZeroVector;

	// Rest of Constructor.
	PrimaryComponentTick.bCanEverTick = true;
	
	CoverTraversingSpeedSystem = CreateDefaultSubobject<USecondOrder1DSystem>("CoverTraversingSpeedSystem");
	CoverOrientationSystem     = CreateDefaultSubobject<USecondOrder1DSystem>("CoverOrientationSystem");
	
	CoverTraversingSpeedSystem->ChangeConstants(2,1,0.2f);
	CoverOrientationSystem->ChangeConstants(4,1,0.1f);

	InCoverAimingDirection = INVALID_IN_COVER_AIMING_DIRECTION;
}

#pragma region TICK FUNCTIONALITY

void UCharacterCoverMovementComponent::MoveBetweenCoversTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(CoverSplineCurrentlyUsing) || !IsValid(CoverSplineTravellingTowards))
	{
		NotifyExitCover();
		return;
	}
	
	const FVector OriginLocation = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline)
		+ FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector TargetLocation = CoverSplineTravellingTowards->FindLocationAtTime(TemporaryUsedSectionOfSplineTravellingTowards)
		+ FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector VectorToTarget = TargetLocation - OriginLocation;

	const bool bTurningCornerMovement = MovementBetweenCoversState == EMovementBetweenCoversState::TurningExteriorCornerLeft
			|| MovementBetweenCoversState == EMovementBetweenCoversState::TurningExteriorCornerRight;
	const bool bMovingBetweenCoversContinuous = MovementBetweenCoversState == EMovementBetweenCoversState::ContinuousLeft
			|| MovementBetweenCoversState == EMovementBetweenCoversState::ContinuousRight;
	
	if (ToCoverTravelCompletePercentage == INVALID_PERCENTAGE_COMPLETE_VALUE) // Is it the first time that we entered this section.
	{
		ToCoverTravelCompletePercentage = 0.0f;
		const FVector RightVector = GenericCharacter->GetActorRightVector();
		const float DotProductToRightVector = FVector::DotProduct(RightVector, VectorToTarget);

		// If we are moving to the right.
		BetweenCoversIsTargetSpeedRight = DotProductToRightVector > 0;

		
		if (bTurningCornerMovement)
		{
			// Calculating the mid-point of our transition.
			const FVector DistanceToMidPoint = FVector(RightVector.X * DotProductToRightVector, RightVector.Y * DotProductToRightVector, (TargetLocation.Z - OriginLocation.Z) / 2);
			
			BetweenCoversTravellingMidPoint = OriginLocation + DistanceToMidPoint;
		}
	}

	CoverMovementSpeed += CoverMovementAcceleration * DeltaTime * (BetweenCoversIsTargetSpeedRight ? 1.0f : -1.0f);
	CoverMovementSpeed = FMath::Clamp(CoverMovementSpeed,-MovingBetweenCoversContinuousTargetSpeed, MovingBetweenCoversContinuousTargetSpeed);

	const float DistanceToTravel = bMovingBetweenCoversContinuous ?
		VectorToTarget.Length() : FVector2D(VectorToTarget.X, VectorToTarget.Y).Length() * 2.9f; // Trust me on this one I did the maths.;

	ToCoverTravelCompletePercentage += abs(CoverMovementSpeed * DeltaTime) / DistanceToTravel;
	ToCoverTravelCompletePercentage = FMath::Clamp(ToCoverTravelCompletePercentage, 0.0f, 1.0f);
	
	if (bMovingBetweenCoversContinuous)
	{
		GenericCharacter->SetActorLocation(FMath::Lerp(OriginLocation, TargetLocation, ToCoverTravelCompletePercentage));
	}
	else if (bTurningCornerMovement)
	{
		// Placing the character's transform.
		GenericCharacter->SetActorLocation(FMath::Lerp(
			FMath::Lerp(OriginLocation, BetweenCoversTravellingMidPoint, ToCoverTravelCompletePercentage),
			FMath::Lerp(BetweenCoversTravellingMidPoint, TargetLocation, ToCoverTravelCompletePercentage),
			ToCoverTravelCompletePercentage));
	}
	else
	{
		FLogger::WarningLog("Unkown MovementBetweenCoversState.");
	}

	GenericCharacter->SetActorRotation(FMath::Lerp(CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline),
		CoverSplineTravellingTowards->FindForwardRotationAtTime(TemporaryUsedSectionOfSplineTravellingTowards),
			ToCoverTravelCompletePercentage));

	if ((1 - ToCoverTravelCompletePercentage) * DistanceToTravel < MinDistanceToBeginInterpolationToCover)
	{
		NotifyReachedCoverTravellingTowards();
	}
}

void UCharacterCoverMovementComponent::LerpingToCoverTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter) || GetEnteringCoverState() != EEnteringCoverState::InterpolatingToCover)
	{
		return;
	}
	
	if (!IsValid(CoverSplineCurrentlyUsing))
	{
		NotifyExitCover();
		return;
	}
	
	// Updating the percentage of completion.
	ToCoverTravelCompletePercentage = FMath::Clamp(ToCoverTravelCompletePercentage + DeltaTime / ToCoverTravelDuration, 0.0f, 1.0f);

	
	// Lerping the position of the character.
	// We flip the Horizontal displacement if we are aiming from the left side of the cover.
	const float HorizontalDisplacementFromTallCoverAim = CurrentHorizontalDisplacementWhenAimingFromTallCover
		* (!CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline < 0.5f ? -LEFT_TALL_COVER_ADDITIONAL_MOVEMENT_MULTIPLIER : 1);
	const FVector TallCoverDisplacement = CoverSplineCurrentlyUsing->FindRightRotationAtTime(SectionOfCurrentlyUsingSpline).Vector() * HorizontalDisplacementFromTallCoverAim;
	
	const FVector TargetLocation = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline)
		+ FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
		+ TallCoverDisplacement;

	GenericCharacter->SetActorLocation(FMath::Lerp(OriginPositionForEnterCoverLerp, TargetLocation, ToCoverTravelCompletePercentage));

	// Lerping the rotation of the character.
	GenericCharacter->SetActorRotation(FQuat::Slerp(
		OriginRotationForEnterCoverLerp,
		TargetRotationForEnterCoverLerp,
		UEasingLibrary::ApplyEase(ToCoverTravelCompletePercentage, EEaseType::InOutSine)));
			
	// Finishing the interpolation.
	if (ToCoverTravelCompletePercentage == 1.0f)
	{
		SetCoveredState(ECoveredState::InCover);
	}
}

void UCharacterCoverMovementComponent::SlidingToCoverTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter) || GetEnteringCoverState() != EEnteringCoverState::InterpolatingWithSlideToCover)
	{
		return;
	}
	
	if (!IsValid(CoverSplineTravellingTowards))
	{
		NotifyExitCover();
		return;
	}
	
	// Updating the percentage of completion.
	ToCoverTravelCompletePercentage = FMath::Clamp(ToCoverTravelCompletePercentage + DeltaTime / ToCoverTravelDuration, 0.0f, 1.0f);

	const FVector TargetLocation = CoverSplineTravellingTowards->FindLocationAtTime(TemporaryUsedSectionOfSplineTravellingTowards);
			
	// Lerping the position of the character.
	GenericCharacter->SetActorLocation(FMath::Lerp(
		OriginPositionForEnterCoverLerp,
		TargetLocation + FVector(0, 0, GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		ToCoverTravelCompletePercentage));
			
	// Finishing the interpolation.
	if (ToCoverTravelCompletePercentage == 1.0f)
	{
		NotifyReachedCoverTravellingTowards();	// Normally, this will never be called.
												// Instead, the slide will be stopped and switched to a normal interpolation somewhere else.
	}
}

void UCharacterCoverMovementComponent::BaseInCoverTickFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	if (!IsValid(CoverSplineCurrentlyUsing))
	{
		NotifyExitCover();
		return;
	}

	// We flip the Horizontal displacement if we are aiming from the left side of the cover.
	const float HorizontalDisplacementFromTallCoverAim = CurrentHorizontalDisplacementWhenAimingFromTallCover
		* (!CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline < 0.5f ? -LEFT_TALL_COVER_ADDITIONAL_MOVEMENT_MULTIPLIER : 1);
	const FVector TallCoverDisplacement = CoverSplineCurrentlyUsing->FindRightRotationAtTime(SectionOfCurrentlyUsingSpline).Vector() * HorizontalDisplacementFromTallCoverAim;
	
	// Quick Return when we have no Input or Speed.
	if (StoredInCoverInput.X == 0.0f && CoverMovementSpeed == 0.0f)
	{
		GenericCharacter->SetActorLocation(CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline)
				+ FVector(0,0,GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
				+ TallCoverDisplacement);		GenericCharacter->SetActorRotation(CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline));
		GetCoverOrientationSystem()->UpdateWithEstimatedVelocity(TargetOrientation, DeltaTime);
		return;
	}

	// Setting the character orientation via player input.
	if (StoredInCoverInput.X > MIN_X_INPUT_FOR_COVER_REACTION)
	{
		TargetOrientation = 1;
	}
	else if (StoredInCoverInput.X < -MIN_X_INPUT_FOR_COVER_REACTION)
	{
		TargetOrientation = -1;
	}
	
	// Moving along a cover.
	if(!CoverSplineCurrentlyUsing->IsSinglePointCover())
	{
		float AbsStoredCoverXInput = abs(StoredInCoverInput.X);
		
		// If we are at the end of a (non-loop) cover, we use an input of 0.
		if (!CoverSplineCurrentlyUsing->IsLoop()
			&& ((SectionOfCurrentlyUsingSpline == 0 && StoredInCoverInput.X < 0) || (SectionOfCurrentlyUsingSpline == 1 && StoredInCoverInput.X > 0)))
		{
			AbsStoredCoverXInput = 0.0f;
		}
																						// Here, we are saying that we do not want to move at all if we are moving too little.
		const float InCoverXInputTreated = UEasingLibrary::ApplyEase(FMath::Clamp(AbsStoredCoverXInput < MIN_X_INPUT_FOR_COVER_REACTION ? 0 : AbsStoredCoverXInput, 0, 1), EEaseType::OutCubic);

		// If we are breaking, not accelerating.
		if (InCoverXInputTreated == 0 || FMath::Sign(CoverMovementSpeed) == -FMath::Sign(StoredInCoverInput.X))
		{
			const bool bOriginalSpeedWasNegativeBeforeBreaking = FMath::Sign(CoverMovementSpeed) == -1.0f;
			CoverMovementSpeed -= FMath::Sign(CoverMovementSpeed) * CoverMovementBreakingAcceleration * DeltaTime;

			if (bOriginalSpeedWasNegativeBeforeBreaking ? CoverMovementSpeed > 0.0f : CoverMovementSpeed < 0.0f)
			{
				CoverMovementSpeed = 0.0f;
			}
		}
		else // We are accelerating.
		{
			CoverMovementSpeed += InCoverXInputTreated * FMath::Sign(StoredInCoverInput.X) * CoverMovementAcceleration * DeltaTime;
			const float AbsCoverMovementSpeed = FMath::Abs(CoverMovementSpeed);
			CoverMovementSpeed = FMath::Clamp(AbsCoverMovementSpeed, 0.0f, MaxCoverBasicMovementSpeed) * FMath::Sign(CoverMovementSpeed);
		}
		
		// Overriding the character orientation if the Character Speed is different from the player input.
		if (CoverMovementSpeed > 0)
		{
			TargetOrientation = 1;
		}
		else if (CoverMovementSpeed < 0)
		{
			TargetOrientation = -1;
		}
		
		SectionOfCurrentlyUsingSpline += CoverMovementSpeed * DeltaTime / CoverSplineCurrentlyUsing->GetSplineLength();

		// Clamping the section is different depending on the type of cover.
		if (CoverSplineCurrentlyUsing->IsLoop())
		{
			if (SectionOfCurrentlyUsingSpline < 0)
			{
				SectionOfCurrentlyUsingSpline += 1;
			}
			else if (SectionOfCurrentlyUsingSpline > 1)
			{
				SectionOfCurrentlyUsingSpline -= 1;
			}
		}
		else
		{
			SectionOfCurrentlyUsingSpline = FMath::Clamp(SectionOfCurrentlyUsingSpline, 0.0f, 1.0f);
		}
	}
	
	// We need to do this every sec even if it is single point cover to avoid the character rising when collision box is wrong.
	GenericCharacter->SetActorLocation(CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline)
		+ FVector(0,0,GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
		+ TallCoverDisplacement);
	GenericCharacter->SetActorRotation(CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline));
	
	GetCoverOrientationSystem()->UpdateWithEstimatedVelocity(TargetOrientation, DeltaTime);
}

void UCharacterCoverMovementComponent::TransitionOutOfCoverFunctionality(float DeltaTime)
{
	if (!IsValid(GenericCharacter))
	{
		return;
	}
	
	ExitCoverTransitionCompletePercentage += DeltaTime / EXIT_COVER_TRANSITION_DURATION;

	if (ExitCoverTransitionCompletePercentage >= 1.0f) // End of the transition.
	{
		SetCoveredState(ECoveredState::NoCover);
	}
	else // Adding MovementInput to our character.
	{
		GenericCharacter->AddMovementInput(ExitCoverTransitionDirection, 1 - UEasingLibrary::ApplyEase(ExitCoverTransitionCompletePercentage, EEaseType::InQuart));
	}
}

void UCharacterCoverMovementComponent::AimInCoverTickFunctionality(float DeltaTime)
{
	// Updating the player's position.
	// We flip the Horizontal displacement if we are aiming from the left side of the cover.
	const float HorizontalDisplacementFromTallCoverAim = CurrentHorizontalDisplacementWhenAimingFromTallCover
		* (!CoverSplineCurrentlyUsing->IsSinglePointCover() && SectionOfCurrentlyUsingSpline < 0.5f ? -LEFT_TALL_COVER_ADDITIONAL_MOVEMENT_MULTIPLIER : 1);
	const FVector TallCoverDisplacement = CoverSplineCurrentlyUsing->FindRightRotationAtTime(SectionOfCurrentlyUsingSpline).Vector() * HorizontalDisplacementFromTallCoverAim;
	
	// We need to do this every sec even if it is single point cover to avoid the character rising when collision box is wrong.
	GenericCharacter->SetActorLocation(CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline)
		+ FVector(0,0,GenericCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
		+ TallCoverDisplacement);
}

void UCharacterCoverMovementComponent::EnteringCoverTickFunctionality(float DeltaTime)
{
	if (GetEnteringCoverState() == EEnteringCoverState::InterpolatingToCover)
	{
		LerpingToCoverTickFunctionality(DeltaTime);
	}
	else if (GetEnteringCoverState() == EEnteringCoverState::InterpolatingWithSlideToCover)
	{
		SlidingToCoverTickFunctionality(DeltaTime);
	}
}

#pragma endregion

#pragma region FUNCTIONS

bool UCharacterCoverMovementComponent::NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter)
{
	if (!IsValid(CoverToEnter))
	{
		// FLogger::ErrorLog("Trying to Enter a Cover but the pointer to it was not valid.");
		return false;
	}
	
	if (SectionToEnter < 0 || SectionToEnter > 1)
	{
		SectionToEnter = FMath::Clamp(SectionToEnter, 0, 1);
	}

	// Checking if the cover is being used by another Character.
	if (CoverToEnter->HowManyCharactersUsingTheSpline() > 0)
	{
		if (Cast<APlayerCharacter>(GetOwner()) != nullptr)
		{
			// If we want to notify the IAs inside the cover that we are entering it, this is where we would do it.
		}
		else
		{
			return false;	
		}
	}
	
	CoverSplineTravellingTowards = CoverToEnter;
	TemporaryUsedSectionOfSplineTravellingTowards = SectionToEnter;
	
	CoverToEnter->RegisterEnterSpline(Cast<AGenericCharacter>(GetOwner()));

	// If we are already in a cover, we are travelling between covers.
	if (GetCoveredState() == ECoveredState::InCover)
	{
		SetInsideCoverState(EInsideCoverState::MovingBetweenCovers);
	}
	else // We are entering a new cover.
	{
		SetEnteringCoverState(EEnteringCoverState::WalkingTowardsCover);
		SetCoveredState(ECoveredState::EnteringCover);
	}

	return true;
}

void UCharacterCoverMovementComponent::NotifyReachedCoverTravellingTowards()
{
	if (IsValid(CoverSplineCurrentlyUsing))
	{
		CoverSplineCurrentlyUsing->RegisterExitSpline(Cast<AGenericCharacter>(GetOwner()));
	}
	
	if(!IsValid(CoverSplineTravellingTowards))
	{
		NotifyExitCover();
		return;
	}

	if (IsValid(GenericCharacter) && GetEnteringCoverState() == EEnteringCoverState::InterpolatingWithSlideToCover)
	{
		GenericCharacter->StopAnimMontage(SlidingToCoverMontage);
	}
	
	CoverSplineCurrentlyUsing = CoverSplineTravellingTowards;
	CoverSplineTravellingTowards = nullptr;
	
	SectionOfCurrentlyUsingSpline = TemporaryUsedSectionOfSplineTravellingTowards;

	SetCoveredState(ECoveredState::EnteringCover);

	// We begin an interpolation without Slide to the Cover Spot. 
	BeginEnteringCoverBasicInterpolation();
}

void UCharacterCoverMovementComponent::NotifyExitCover()
{
	if (IsValid(CoverSplineCurrentlyUsing))
	{
		SetCoveredState(ECoveredState::ExitingCover);
		
		CoverSplineCurrentlyUsing->RegisterExitSpline(Cast<AGenericCharacter>(GetOwner()));
		CoverSplineCurrentlyUsing = nullptr;
	}
	else // We can't exit transition out of a cover that no longer exists (probably was destroyed).
	{
		SetCoveredState(ECoveredState::NoCover);
	}

	// We may try to Exit a cover while travelling to one too. That is also taken into account.
	if (IsValid(CoverSplineTravellingTowards))
	{
		CoverSplineTravellingTowards->RegisterExitSpline(Cast<AGenericCharacter>(GetOwner()));
		CoverSplineTravellingTowards = nullptr;
	}
}

ECoveredState UCharacterCoverMovementComponent::SetCoveredState(const ECoveredState NewCoveredState)
{
	if (GetCoveredState() == NewCoveredState || !IsValid(GenericCharacter))
	{
		return GetCoveredState();
	}

	// Exit State actions
	switch (GetCoveredState())
	{
	case ECoveredState::NoCover:
		break;
		
	case ECoveredState::InCover:
		SetInsideCoverState(EInsideCoverState::BaseCovered); // We reset our cover state when we exit a cover.
		
		SetInCoverAimingDirection(INVALID_IN_COVER_AIMING_DIRECTION);
		
		CurrentHorizontalDisplacementWhenAimingFromTallCover = 0.0f; // We reset any horizontal additional displacement.
		
		break;
		
	case ECoveredState::EnteringCover:
		// We reset the Entering Cover State to not entering cover.
		SetEnteringCoverState(EEnteringCoverState::NotEnteringCover);
		
		ToCoverTravelCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
		TemporaryUsedSectionOfSplineTravellingTowards = INVALID_PERCENTAGE_COMPLETE_VALUE;
		
		break;
		
	case ECoveredState::ExitingCover:
		ExitCoverTransitionCompletePercentage = INVALID_PERCENTAGE_COMPLETE_VALUE;
		break;
	}

	// Enter State actions
	switch (NewCoveredState)
	{
	case ECoveredState::NoCover:
		// Re-enabling collision.
		GenericCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				
		// Resetting variables.
		StoredInCoverInput = FVector2D::Zero();
		TemporaryUsedSectionOfSplineTravellingTowards = INVALID_PERCENTAGE_COMPLETE_VALUE;
		SectionOfCurrentlyUsingSpline = INVALID_PERCENTAGE_COMPLETE_VALUE;
		break;
		
	case ECoveredState::InCover:
		if (GenericCharacter->IsAiming())
		{
			SetInsideCoverState(EInsideCoverState::Aiming);
		}
		break;
		
	case ECoveredState::EnteringCover:
		// Resetting the orientation system.
		GetCoverOrientationSystem()->ResetSystemToValue(TargetOrientation);
		CoverMovementSpeed = 0.0f;
		
		break;

	case ECoveredState::ExitingCover:
		ExitCoverTransitionCompletePercentage = 0.0f;
		
		if (!TryToTransitionOutOfCover()) // If we fail when trying to begin a Transition Out of our Cover.
		{
			return SetCoveredState(ECoveredState::NoCover);
		}
		break;
	}
	
	CoveredState = NewCoveredState;
	return GetCoveredState();
}

EInsideCoverState UCharacterCoverMovementComponent::SetInsideCoverState(const EInsideCoverState NewInsideCoverState)
{
	if (GetInsideCoverState() == NewInsideCoverState || !IsValid(GenericCharacter))
	{
		return InsideCoverState;
	}

	// Exit State actions
	switch (InsideCoverState)
	{
	case EInsideCoverState::BaseCovered:
		break;
	case EInsideCoverState::Aiming:
		bIsAimingFromTallCover = false;
		break;
	case EInsideCoverState::MovingBetweenCovers:
		MovementBetweenCoversState = EMovementBetweenCoversState::NoMovement;
		break;
	default:
		break;
	}

	// Enter State actions
	switch (NewInsideCoverState)
	{
	case EInsideCoverState::BaseCovered:
		break;
	case EInsideCoverState::Aiming:
		CoverMovementSpeed = 0.0f;
		if (CheckForTallCorner(true) || CheckForTallCorner(false))
		{
			bIsAimingFromTallCover = true;
		}
		break;
	case EInsideCoverState::MovingBetweenCovers:
		break;
	default:
		break;
	}
	
	InsideCoverState = NewInsideCoverState;
	return InsideCoverState;
}

EEnteringCoverState UCharacterCoverMovementComponent::SetEnteringCoverState(
	const EEnteringCoverState NewEnteringCoverState)
{
	if (GetEnteringCoverState() == NewEnteringCoverState || !IsValid(GenericCharacter))
	{
		return GetEnteringCoverState();
	}

	// Exiting cases:
	switch (GetEnteringCoverState())
	{
	case EEnteringCoverState::NotEnteringCover:
		break;
	case EEnteringCoverState::WalkingTowardsCover:
		break;
	case EEnteringCoverState::InterpolatingToCover:
		break;
	case EEnteringCoverState::InterpolatingWithSlideToCover:
		break;
	}

	// Entering Cases
	switch (NewEnteringCoverState)
	{
	case EEnteringCoverState::NotEnteringCover:
		break;
	case EEnteringCoverState::WalkingTowardsCover:
		break;
		
	case EEnteringCoverState::InterpolatingToCover:
		// Resetting the speed of the Character.
		GenericCharacter->GetMovementComponent()->StopActiveMovement();
	    	
		// Disabling collision.
		GenericCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;
		
	case EEnteringCoverState::InterpolatingWithSlideToCover:
		// Resetting the speed of the Character.
		GenericCharacter->GetMovementComponent()->StopActiveMovement();
	    	
		// Disabling collision.
		GenericCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;
	}

	EnteringCoverState = NewEnteringCoverState;
	return EnteringCoverState;
}

bool UCharacterCoverMovementComponent::IsTravellingToCover() const
{
	return IsValid(CoverSplineTravellingTowards);
}

bool UCharacterCoverMovementComponent::IsInTallCover() const
{
	const bool bIsMovingBetweenCovers = GetInsideCoverState() == EInsideCoverState::MovingBetweenCovers;
	if (bIsMovingBetweenCovers)
	{
		return false;
	}

	const UCoverSpline* CoverToTest = IsTravellingToCover() ? CoverSplineTravellingTowards : CoverSplineCurrentlyUsing;
	if (!IsValid(CoverToTest))
	{
		return false;
	}
	
	const float SectionToTest = IsTravellingToCover() ? TemporaryUsedSectionOfSplineTravellingTowards : SectionOfCurrentlyUsingSpline;

	const float SectionFromCentreOfCheck = HORIZONTAL_DISTANCE_FROM_CENTRE_WHEN_CHECKING_IF_COVER_IS_TALL / CoverToTest->GetSplineLength();
	const float SectionToCheckLeft = FMath::Clamp(SectionToTest - SectionFromCentreOfCheck, 0.0f, 1.0f);
	const float SectionToCheckRight = FMath::Clamp(SectionToTest + SectionFromCentreOfCheck, 0.0f, 1.0f);
	
	return CoverToTest->TestTallCoverAtTime(SectionToCheckLeft) && CoverToTest->TestTallCoverAtTime(SectionToCheckRight);
}

FVector UCharacterCoverMovementComponent::GetCurrentCoverForwardVector() const
{
	if (IsValid(GetCoverSplineCurrentlyUsing()))
	{
		return GetCoverSplineCurrentlyUsing()->FindForwardRotationAtTime(GetSectionOfCurrentlyUsingSpline()).Vector();
	}

	return FVector::Zero();
}

float UCharacterCoverMovementComponent::GetInCoverOrientation() const
{
	if (IsValid(GenericCharacter))
	{
		return GetCoverOrientationSystem()->GetSystemStoredValue();
	}
	
	return 0;
}

float UCharacterCoverMovementComponent::GetInCoverSpeed() const
{
	return CoverMovementSpeed;
}

float UCharacterCoverMovementComponent::GetInCoverMaxSpeed() const
{
	return MaxCoverBasicMovementSpeed;
}

void UCharacterCoverMovementComponent::StoreInCoverInput(const float ForwardInput, const float RightInput)
{
	StoredInCoverInput = FVector2D(RightInput, ForwardInput);
}

void UCharacterCoverMovementComponent::BeginEnteringCoverBasicInterpolation()
{
	if (!IsValid(CoverSplineCurrentlyUsing) || !IsValid(GenericCharacter))
	{
		return;
	}

	SetEnteringCoverState(EEnteringCoverState::InterpolatingToCover);
	
	ToCoverTravelCompletePercentage = 0.0f;
	OriginPositionForEnterCoverLerp = GenericCharacter->GetActorLocation();
	OriginRotationForEnterCoverLerp = GenericCharacter->GetActorRotation().Quaternion();
	TargetPositionForEnterCoverLerp = CoverSplineCurrentlyUsing->FindLocationAtTime(SectionOfCurrentlyUsingSpline);
	ToCoverTravelDuration = FVector::DistXY(OriginPositionForEnterCoverLerp, TargetPositionForEnterCoverLerp) / InterpolateToCoverSpeed;

	// Deciding whether the character will face right or left when entering the cover.
	const float DotProductForOrientation = FVector::DotProduct(
		CoverSplineCurrentlyUsing->FindRightRotationAtTime(SectionOfCurrentlyUsingSpline).Vector(),
		TargetPositionForEnterCoverLerp - OriginPositionForEnterCoverLerp);
	   	
	if (DotProductForOrientation > 0) // Facing right.
	{
		TargetOrientation = 1;
	}
	else if (DotProductForOrientation < 0) // Facing left.
	{
		TargetOrientation = -1;
	}
		
	TargetRotationForEnterCoverLerp = CoverSplineCurrentlyUsing->FindForwardRotationAtTime(SectionOfCurrentlyUsingSpline).Quaternion();
}

void UCharacterCoverMovementComponent::BeginEnteringCoverSlidingInterpolation()
{
	if (!IsValid(CoverSplineTravellingTowards) || !IsValid(GenericCharacter))
	{
		return;
	}

	if (IsValid(SlidingToCoverMontage))
	{
		GenericCharacter->PlayMontage(SlidingToCoverMontage, 1.0f);
	}

	SetEnteringCoverState(EEnteringCoverState::InterpolatingWithSlideToCover);

	ToCoverTravelCompletePercentage = 0.0f;
	OriginPositionForEnterCoverLerp = GenericCharacter->GetActorLocation();
	TargetPositionForEnterCoverLerp = CoverSplineTravellingTowards->FindLocationAtTime(TemporaryUsedSectionOfSplineTravellingTowards);
	ToCoverTravelDuration = FVector::DistXY(OriginPositionForEnterCoverLerp, TargetPositionForEnterCoverLerp) / SlideToCoverSpeed;
}

bool UCharacterCoverMovementComponent::TryToTransitionOutOfCover()
{
	// Exit Cover Transitions only exists for Players, cause AIs have path finding.
	// If this isn't a player, we will cancel the transition.
	return false;
}

FVector UCharacterCoverMovementComponent::GetInCoverAimingDirection() const
{
	return InCoverAimingDirection;
}

bool UCharacterCoverMovementComponent::CheckForTallCorner(const bool bCheckIfCornerIsOnLeftSide) const
{
	const bool bIsMovingBetweenCovers = GetInsideCoverState() == EInsideCoverState::MovingBetweenCovers;
	if (bIsMovingBetweenCovers)
	{
		return false;
	}
	
	const UCoverSpline* CoverToTest = IsTravellingToCover() ? CoverSplineTravellingTowards : CoverSplineCurrentlyUsing;
	if (!IsValid(CoverToTest))
	{
		return false;
	}
	
	const float Section = IsTravellingToCover() ? TemporaryUsedSectionOfSplineTravellingTowards : SectionOfCurrentlyUsingSpline;
	const float SectionThatHasToBeTall = FMath::Clamp(Section, 0.0f, 1.0f);

	const float TestDisplacement = bCheckIfCornerIsOnLeftSide ? -DistanceFromCornerEdgeToAllowCornerAim : DistanceFromCornerEdgeToAllowCornerAim;

	// If the cover is tall and Single Point, it will have a corner for sure.
	// Otherwise, we check that there is one.
	return CoverToTest->TestTallCoverAtTime(SectionThatHasToBeTall)
		&& (CoverToTest->IsSinglePointCover() || !CoverToTest->TestTallCoverAtTimeWithHorizontalDisplacement(Section, TestDisplacement));
}

#pragma endregion

#pragma region VIRTUAL FUNCTIONS

void UCharacterCoverMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (GetCoveredState())
	{
	case ECoveredState::NoCover:
		break;

		// In cover Logic.	
	case ECoveredState::InCover:
		switch (InsideCoverState)
		{
		case EInsideCoverState::BaseCovered:
			BaseInCoverTickFunctionality(DeltaTime);
				break;
		case EInsideCoverState::Aiming:
			AimInCoverTickFunctionality(DeltaTime);
				break;
		case EInsideCoverState::MovingBetweenCovers:
			MoveBetweenCoversTickFunctionality(DeltaTime);
				break;
		}
		break;
		
	case ECoveredState::EnteringCover:
		EnteringCoverTickFunctionality(DeltaTime);
		break;
		
	case ECoveredState::ExitingCover:
		TransitionOutOfCoverFunctionality(DeltaTime);
		break;
	}

	float NewDisplacement = CurrentHorizontalDisplacementWhenAimingFromTallCover;
	
	// If we are NOT aiming from a Tall Cover we always reduce the horizontal displacement of that action.
	if (!bIsAimingFromTallCover && CurrentHorizontalDisplacementWhenAimingFromTallCover > 0.0f)
	{
		NewDisplacement -= HorizontalDisplacementSpeedWhenAimingFromTallCorner * DeltaTime * TALL_COVER_ADDITIONAL_MOVEMENT_BREAK_MULTIPLIER;
	}
	// If we are aiming from a Tall Cover, we want to move the player horizontally to allow the aiming.
	else if (bIsAimingFromTallCover && CurrentHorizontalDisplacementWhenAimingFromTallCover < MaxHorizontalDisplacementWhenAimingFromTallCorner)
	{
		NewDisplacement += HorizontalDisplacementSpeedWhenAimingFromTallCorner * DeltaTime;
	}

	CurrentHorizontalDisplacementWhenAimingFromTallCover = FMath::Clamp(
			NewDisplacement,
			0.0f,
			MaxHorizontalDisplacementWhenAimingFromTallCorner);
}

void UCharacterCoverMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	GenericCharacter = Cast<AGenericCharacter>(GetOwner());
}

#pragma endregion
