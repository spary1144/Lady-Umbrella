// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LadyUmbrella/Components/CoverComponent/CoverEnums.h"
#include "LadyUmbrella/Components/FreeRoamCoverComponent/FreeRoamCoverAnimationState.h"
#include "CharacterCoverMovementComponent.generated.h"

class USecondOrder1DSystem;
class UCoverSpline;
class AGenericCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsCoveredChanged, bool, newState);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )

class LADYUMBRELLA_API UCharacterCoverMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation",  meta = (AllowPrivateAccess = true))
	ECoverHeightState CoverHeightState;

	UPROPERTY()
	FVector InCoverAimingDirection;
	
	
public:	
	// Sets default values for this component's properties
	UCharacterCoverMovementComponent();

	const FVector INVALID_IN_COVER_AIMING_DIRECTION = FVector::Zero();

#pragma region CONSTANTS
	
	const float MAX_DISTANCE_FROM_EDGE_TO_PERFORM_EXIT_TRANSITION = 100.0;
	const float MIN_X_INPUT_FOR_COVER_REACTION = 0.15f;
	const float EXIT_COVER_TRANSITION_DURATION = 0.5f;
	const float INVALID_PERCENTAGE_COMPLETE_VALUE = -1.0f;
	const float ZERO_TRAVERSAL_SPEED = 0.0f;
	const float HORIZONTAL_DISTANCE_FROM_CENTRE_WHEN_CHECKING_IF_COVER_IS_TALL = 40.0f;
	const float DISTANCE_OF_LINE_TRACE_WHEN_CHECKING_AIMING_DIRECTION = 2000.0f;
	const float MINIMUM_SPEED_TO_AVOID_BREAKING = 0.05f;
	const float LEFT_TALL_COVER_ADDITIONAL_MOVEMENT_MULTIPLIER = 1.7f;
	const float TALL_COVER_ADDITIONAL_MOVEMENT_BREAK_MULTIPLIER = 1.5f;

#pragma endregion

#pragma region PARAMETERS
public:
	/**
	 * The Maximum Speed that the character can move with in cover.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement", meta = (DisplayPriority = -2))
	float MaxCoverBasicMovementSpeed;

	/**
	 * The acceleration that the player has when trying to gain speed inside a cover.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement", meta = (DisplayPriority = -2))
	float CoverMovementAcceleration;

	/**
 	* The acceleration that the player has when trying to stop moving inside a cover.
 	*/
	UPROPERTY(EditAnywhere, Category= "Cover Movement", meta = (DisplayPriority = -2))
	float CoverMovementBreakingAcceleration;

protected:
	/**
 	* The animation that will play when sliding towards a cover.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	UAnimMontage* SlidingToCoverMontage;
	
	/**
	 * When a character is told to enter a cover, it starts walking towards it.
	 * When the distance between that character and the cover point it travels to is smaller than this variable, 
	 * the character stops walking and its position starts interpolating with that of the cover.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Entering Cover", meta = (DisplayPriority = -2))
	float MinDistanceToBeginInterpolationToCover;

	/**
 	* When a character is told to enter a cover, it starts walking towards it.
 	* When the distance between that character and the cover point it travels to is SMALLER than this variable, 
 	* & the character has enough speed towards the cover, the character starts sliding towards the cover.
 	*/
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Entering Cover", meta = (DisplayPriority = -2))
	float MinDistanceToBeginSlidingToCover;

	/**
 	* When a character is told to enter a cover, it starts walking towards it.
 	* When the distance between that character and the cover point it travels to BIGGER than this variable, 
 	* & the character has enough speed towards the cover, the character starts sliding towards the cover.
 	*/
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Entering Cover", meta = (DisplayPriority = -2))
	float MaxDistanceToBeginSlidingToCover;

	/**
 	* When a character is told to enter a cover, it starts walking towards it.
 	* When the distance between that character and the cover point is small enough & the character has a speed towards
 	* the cover higher than this variable, the character starts sliding towards the cover.
 	*/
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Entering Cover", meta = (DisplayPriority = -2))
	float MinSpeedToBeginSlidingToCover;

	/**
	* The speed with which the character interpolates to the cover when entering it without sliding.
 	*/
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Entering Cover", meta = (DisplayPriority = -2))
	float InterpolateToCoverSpeed;

	/**
 	* The speed with which the character slides towards a cover when entering it with enough speed.
 	*/
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Entering Cover", meta = (DisplayPriority = -2))
	float SlideToCoverSpeed;

	/**
	 * The speed that the character tries to reach when moving between continuous covers. Overrides MaxCoverMovementSpeed.
	 */
	UPROPERTY(EditAnywhere, Category= "Cover Movement|Moving Between Covers", meta = (DisplayPriority = -2))
	float MovingBetweenCoversContinuousTargetSpeed;

	/**
 	* The distance from the corner point that allows for corner aiming.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Aiming", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float DistanceFromCornerEdgeToAllowCornerAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Aiming", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxHorizontalDisplacementWhenAimingFromTallCorner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Aiming", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float HorizontalDisplacementSpeedWhenAimingFromTallCorner;

	UPROPERTY(EditDefaultsOnly)
	USecondOrder1DSystem* CoverOrientationSystem;
	
	UPROPERTY(EditDefaultsOnly)
	USecondOrder1DSystem* CoverTraversingSpeedSystem;
	
#pragma endregion

#pragma region VARIABLES

protected:
	UPROPERTY()
	ECoveredState CoveredState;

	UPROPERTY()
	EInsideCoverState InsideCoverState;

	UPROPERTY()
	EEnteringCoverState EnteringCoverState;

	UPROPERTY()
	EMovementBetweenCoversState MovementBetweenCoversState;
	
	UPROPERTY()
	float TargetOrientation;
	
	/**
	 * The character's 2D movement input relative to the cover the character is in.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	FVector2D StoredInCoverInput;
	
	/**
	 * 	The Cover Spline Component where the character is currently taking cover.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	UCoverSpline* CoverSplineCurrentlyUsing;
	
	/**
	 * The Section that the character occupies in the CoverSpline.
	 * 0 is the left-most point. 1 is the right-most point.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement")
	float SectionOfCurrentlyUsingSpline;
	
	/**
	 * The Spline the character is travelling towards.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement|Entering Cover")
	UCoverSpline* CoverSplineTravellingTowards;
	
	UPROPERTY()
	float TemporaryUsedSectionOfSplineTravellingTowards;
	
	UPROPERTY()
	bool bIsAimingFromTallCover;
	
private:
	UPROPERTY()
	float CurrentHorizontalDisplacementWhenAimingFromTallCover;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Cover Movement")
	float CoverMovementSpeed;
	
	// Travelling to cover
	UPROPERTY(VisibleInstanceOnly, Category = "Cover Movement|Entering Cover")
	float ToCoverTravelCompletePercentage;

	UPROPERTY(VisibleInstanceOnly, Category = "Cover Movement|Entering Cover")
	float ToCoverTravelDuration;

	// Entering cover
	UPROPERTY()
	FVector OriginPositionForEnterCoverLerp;
	UPROPERTY()
	FVector TargetPositionForEnterCoverLerp;

	UPROPERTY()
	FQuat OriginRotationForEnterCoverLerp;
	UPROPERTY()
	FQuat TargetRotationForEnterCoverLerp;

protected:
	// Exiting cover
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Cover Movement|Exiting Cover")
	float ExitCoverTransitionCompletePercentage;

	UPROPERTY()
	FVector ExitCoverTransitionDirection;

private:
	// Moving between covers
	UPROPERTY()
	FVector BetweenCoversTravellingMidPoint;

	UPROPERTY()
	bool BetweenCoversIsTargetSpeedRight;

protected:
	UPROPERTY()
	AGenericCharacter* GenericCharacter;

#pragma endregion 

#pragma region TICK FUNCTIONALITY
private:
	UFUNCTION()
	void TransitionOutOfCoverFunctionality(float DeltaTime);

protected:
	UFUNCTION()
	virtual void LerpingToCoverTickFunctionality(float DeltaTime);

	UFUNCTION()
	virtual void MoveBetweenCoversTickFunctionality(float DeltaTime);
	
	UFUNCTION()
	void SlidingToCoverTickFunctionality(float DeltaTime);

	UFUNCTION()
	virtual void BaseInCoverTickFunctionality(float DeltaTime);

	UFUNCTION()
	virtual void AimInCoverTickFunctionality(float DeltaTime);

	UFUNCTION()
	virtual void EnteringCoverTickFunctionality(float DeltaTime);

#pragma endregion
	
#pragma region FUNCTIONS

public:
	// Notifies
	UFUNCTION()
	virtual bool NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter);

	UFUNCTION()
	virtual void NotifyReachedCoverTravellingTowards();

	UFUNCTION()
	void NotifyExitCover();

	// Input
	UFUNCTION()
	void StoreInCoverInput(const float ForwardInput, const float RightInput);
	
	// Cover states.
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ECoveredState GetCoveredState() const { return CoveredState; };

	UFUNCTION(BlueprintCallable)
	EInsideCoverState GetInsideCoverState() const { return InsideCoverState; };

	UFUNCTION(BlueprintCallable)
	EEnteringCoverState GetEnteringCoverState() const { return EnteringCoverState; };

	UFUNCTION(BlueprintCallable)
	virtual ECoveredState SetCoveredState(const ECoveredState NewCoveredState);
	
	UFUNCTION(BlueprintCallable)
	virtual EInsideCoverState SetInsideCoverState(const EInsideCoverState NewInsideCoverState);

	UFUNCTION(BlueprintCallable)
	virtual EEnteringCoverState SetEnteringCoverState(const EEnteringCoverState NewEnteringCoverState);

	// Quick checks and Getters.
	UFUNCTION(BlueprintCallable)
	bool IsTravellingToCover() const;

	UFUNCTION(BlueprintCallable)
	bool IsMovementControlledByCover() const { return GetCoveredState() == ECoveredState::InCover || GetCoveredState() == ECoveredState::EnteringCover; };

	/**
	 * True when the character is not behind cover OR when the character is travelling between covers.
	 * @return 
	 */
	UFUNCTION()
	FORCEINLINE bool CanCharacterBePulledByHook() const
		{ return GetCoveredState() == ECoveredState::NoCover || GetCoveredState() == ECoveredState::EnteringCover || GetCoveredState() == ECoveredState::ExitingCover || InsideCoverState == EInsideCoverState::MovingBetweenCovers; };

	UFUNCTION(BlueprintCallable)
	bool IsInTallCover() const;

	UFUNCTION(BlueprintCallable)
	FVector GetCurrentCoverForwardVector() const;
	
	UFUNCTION()
	FORCEINLINE UCoverSpline* GetCoverSplineCurrentlyUsing() const { return CoverSplineCurrentlyUsing; };

	UFUNCTION()
	FORCEINLINE UCoverSpline* GetCoverSplineTravellingTowards() const { return CoverSplineTravellingTowards; };
	
	UFUNCTION()
	FORCEINLINE float GetExitCoverTransitionCompletePercentage() const { return ExitCoverTransitionCompletePercentage; };
	
	UFUNCTION(BlueprintCallable)
	float GetInCoverSpeed() const;

	UFUNCTION(BlueprintCallable)
	float GetInCoverMaxSpeed() const;

	UFUNCTION()
	FORCEINLINE float GetTargetOrientation() const { return TargetOrientation; };
	
	UFUNCTION(BlueprintCallable)
	float GetInCoverOrientation() const;

	UFUNCTION(BlueprintCallable)
	FVector GetInCoverAimingDirection() const;

	/**
 	* Checks if the cover is tall on the player's chosen side but is low or non-existent on the opposite side.
 	* @param bCheckIfCornerIsOnLeftSide If we are checking if there is a tall corner on the Left side (instead of the right one).
 	* @return True if there is a corner in the specified side.
 	*/
	UFUNCTION()
	bool CheckForTallCorner(const bool bCheckIfCornerIsOnLeftSide) const;
	
	UFUNCTION()
	FORCEINLINE float GetSectionOfCurrentlyUsingSpline() const { return SectionOfCurrentlyUsingSpline; };

	FORCEINLINE TObjectPtr<USecondOrder1DSystem> GetCoverOrientationSystem() const { return CoverOrientationSystem; }
	FORCEINLINE TObjectPtr<USecondOrder1DSystem> GetCoverTraversingSpeedSystem() const { return CoverTraversingSpeedSystem; }

	FORCEINLINE void SetInCoverAimingDirection(const FVector& Value) { InCoverAimingDirection = Value; }
	FORCEINLINE void SetCoverHeightState(const ECoverHeightState Value) { CoverHeightState = Value; }
	
protected:
	UFUNCTION()
	void BeginEnteringCoverBasicInterpolation();

	UFUNCTION()
	void BeginEnteringCoverSlidingInterpolation();

	UFUNCTION()
	virtual bool TryToTransitionOutOfCover();
	
#pragma endregion

#pragma region VIRTUAL FUNCTIONS
public:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region DELEGATES
public:
	FOnIsCoveredChanged OnIsCoveredChanged;

#pragma endregion
};
