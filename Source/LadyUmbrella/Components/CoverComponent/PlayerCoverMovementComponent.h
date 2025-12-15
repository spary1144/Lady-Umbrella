// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Components/coverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Math/EasingLibrary.h"
#include "PlayerCoverMovementComponent.generated.h"

/**
 * 
 */

class UNiagaraComponent;
class UMoveBetweenCoversWidget;
class UWidgetComponent;
class USecondOrder1DSystem;
class APlayerCharacter;
class UVoiceLineManager;
/**
 * Internal struct used to evaluate Cover Entry Points, don't use it outside of this script.
 */
USTRUCT()
struct FCoverEvaluation
{
	GENERATED_BODY()

	UCoverSpline* Cover;
	FVector Position;
	float Section;
	float Evaluation;
};

DECLARE_MULTICAST_DELEGATE(FOnMoveBetweenCoverDelegate)

UCLASS()
class LADYUMBRELLA_API UPlayerCoverMovementComponent : public UCharacterCoverMovementComponent
{
	GENERATED_BODY()

	UPlayerCoverMovementComponent();


#pragma region CONSTANTS

	const float MAX_Z_DISTANCE_FOR_COVER_TO_BE_SELECTABLE = 150.f;
	const float MIN_DISTANCE_BETWEEN_PLAYER_AND_COVER_TO_PERFORM_MOVEMENT_TEST = 35.0f;
	
	/**
	 * This is a multiplier used to separate the move forward between covers UI away from the cover to increase visibility.
	 */
	const float MOVE_COVERS_FORWARD_DISTANCE_FROM_CENTER_MULTI = 1.3f;
	/**
	 * This multiplier is used to extend the curvature of the move forward between covers UI past the 90º circumference.
	 */
	const float MOVE_COVERS_FORWARD_CIRCUMFERENCE_LENGHT_MULTIPLIER = 1.3f;
	const int MOVE_COVERS_FORWARD_NUM_OF_PATH_POINTS = 5;
	
	// This is the minimum angle required to distinguish a continuous movement from forward movement between covers.
	const float MIN_X_INPUT_FOR_CONTINUOUS_MOVEMENT = FMath::Cos(FMath::DegreesToRadians(35));
	const float MIN_X_INPUT_FOR_FORWARD_MOVEMENT = FMath::Cos(FMath::DegreesToRadians(80));
	const float MIN_Y_INPUT_FOR_FORWARD_MOVEMENT = FMath::Cos(FMath::DegreesToRadians(70));

	// Camera
	const float MIDDLE_SCREEN_RATIO = 0.5f;

	const FName RIBBON_ID_NIAGARA_SYSTEM_VARIABLE_NAME = FName(TEXT("RibbonID"));
	const FName NUMBER_OF_NIAGARA_SYSTEM_PATH_POINTS_VARIABLE_NAME = FName(TEXT("NumPointsInPath"));
	const FName MOVE_BETWEEN_COVERS_RIBBON_COLOR_VARIABLE_NAME = FName(TEXT("RibbonColor"));
	const FName NIAGARA_SYSTEM_PATH_POINTS_ARRAY_VARIABLE_NAME = FName(TEXT("PathPoints"));
	const FName NIAGARA_SYSTEM_VISIBLE_PATH_VARIABLE_NAME = FName(TEXT("PathVisible"));

	
#pragma endregion
	
#pragma region PARAMETERS
private:
	/********************/
	/*  COVER MOVEMENT  */
	/********************/
	
	/**
	 * The maximum distance between the player and a CoverParent for its Entry Points to be considered when trying to enter cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float CoverActorsAcceptanceRadius;

	/**
 	* Whether the Player Direction is based on the Player's Camera Direction when no movement input is provided.
 	* The alternative is taking the direction from the model's orientation.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MinMovementIntensityForValidPlayerDirection;
	
	/**
 	* Whether the Player Direction is based on the Player's Camera Direction when no movement input is provided.
 	* The alternative is taking the direction from the model's orientation.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bIsPlayerDirectionBasedOnCameraWhenNoInput;

	/**
	 * Whether the Distance Test is performed when trying to find a cover to enter.
	 * The Distance Test evaluates a cover point based on the XY distance to the Player.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bIsDistanceTestActive;

	/**
 	* Whether to provide additional length in the Distance Test for covers aligned with the player's camera.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bUseAdditionalDistanceForCoversInCameraDir;

	/**
 	* Whether to provide additional length in the Distance Test for covers aligned with the player's movement.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bUseAdditionalDistanceForCoversInMovementDir;

	/**
	 * Whether to disable the additional length in the Distance Test for moments when the player is not moving.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bDisableAdditionalDistanceWhenNoPlayerMovement;
	
	/**
 	* The maximum distance between the player and a Cover EntryPoint for it to be considered when trying to enter cover.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float CoverPointsAcceptanceRadius;
	
	/**
 	* The additional distance (which is added to the base valid distance) used to evaluate covers in the distance test.
 	* Can be triggered by toggling the "UseAdditionalDistanceForPointsVisibleInCamera" or the "UseAdditionalDistanceForPointsAlignedWithMovementDirection" options.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1, EditConditionHides = "bUseAdditionalDistanceForPointsVisibleInCamera || bUseAdditionalDistanceForPointsAlignedWithMovementDirection"))
	float AdditionalDistanceForSpecialCases;

	/**
 	* The maximum dot product value from the Guiding Vector (Camera or Movement Vector) that a cover would be allowed to deviate when checking if Additional Distance applies.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1, EditConditionHides = "bUseAdditionalDistanceForPointsVisibleInCamera || bUseAdditionalDistanceForPointsAlignedWithMovementDirection"))
	float ValidDotProductFromGuidingVectorForAdditionalDistanceTest;

	/**
 	* The curve used to evaluate points in the Distance Test. It goes from 0 to 1 in the Y axis.
 	* X = The distance between the Player and the Cover Point.
 	* Y = 0 represents a point being given the lowest evaluation possible.
 	* Y = 1 represents a point being given the highest evaluation possible.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Distance Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	UCurveFloat* DistanceTestEvaluationCurve;

	/**
 	* Whether the Player Direction Test is performed when trying to find a cover to enter.
 	* The Player Direction Test checks if the player's direction
 	* (determined by either the player's input or, in its absence, the camera's direction) is similar to that of the cover point.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Player Direction Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bIsPlayerDirectionTestActive;

	/**
 	* The curve used to evaluate points in the Player Direction Test when the Player's direction is taken from the movement input. It goes from 0 to 1 in the X & Y axis.
 	* X = 0 represents a Cover Direction opposite to the Player's Input.
 	* X = 1 represents a Cover Direction parallel to the Player's Input.
 	* Y = 0 represents a point being given the lowest evaluation possible.
 	* Y = 1 represents a point being given the highest evaluation possible.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Player Direction Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	UCurveFloat* PlayerDirectionTestEvaluationCurveWithValidPlayerInput;

	/**
	 * The curve used to evaluate points in the Player Direction Test when the Player's Direction is NOT taken from the movement input. It goes from 0 to 1 in the X & Y axis.
	 * X = 0 represents a Cover Direction opposite to the Player's Direction.
	 * X = 1 represents a Cover Direction parallel to the Player's Direction.
	 * Y = 0 represents a point being given the lowest evaluation possible.
	 * Y = 1 represents a point being given the highest evaluation possible.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Player Direction Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	UCurveFloat* PlayerDirectionTestEvaluationCurveWhenPlayerInputInvalid;
	
	/**
 	* Whether the Movement Direction Test is performed when trying to find a cover to enter.
 	* The Movement Direction Test checks if the direction that a player would move towards when
 	* taking cover at a point is similar to that cover point's direction.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Movement Direction Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bIsMovementDirectionTestActive;

	/**
	 * The curve used to evaluate points in the Movement Direction Test. It goes from 0 to 1 in the X & Y axis.
	 * X = 0 represents a Cover Direction opposite to the Direction of the Player Moving Towards it.
	 * X = 1 represents a Cover Direction parallel to the Direction of the Player Moving Towards it.
	 * Y = 0 represents a point being given the lowest evaluation possible.
	 * Y = 1 represents a point being given the highest evaluation possible.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Entering Cover|Evaluation Tests|Movement Direction Test", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	UCurveFloat* MovementDirectionTestEvaluationCurve;
	
	/**
	 * The amount of time (in secs) a players must be trying to exit cover via keyboard input for it to happen.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Exiting Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float SecsNeededToExitCoverWithKeyboardInput;

	/**
 	* The amount of time (in secs) a players must be trying to exit cover via controller input for it to happen.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Exiting Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float SecsNeededToExitCoverWithControllerInput;

	/**
	 * Determines the angle maximum angle with the backwards vector allowed when trying to exit a cover via input.
	 * Only for Controller.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Exiting Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxInputAngleBackwardsToTryToExitCoverWithController;

	/**
	* Determines the angle maximum angle with the backwards vector allowed when trying to exit a cover via input.
	* Only for Keyboard.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Exiting Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxInputAngleBackwardsToTryToExitCoverWithKeyboard;

	/**
	 * Determines how much faster the "SecStorageTryingToExitCoverWithInput" variable depletes when the player is not trying to exit cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Exiting Cover", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float NotExitingCoverTimerMultiplier;
	
	/**
	 * The minimum distance the player can be from an environment element for them to be allowed to aim the gun while in cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Aiming", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MinDistanceWithEnvironmentToAllowAim;
	
	/**
	 * Activates the ability for the player to move from one cover to another with only its movement input.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Moving Between Covers", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	bool bCanMoveBetweenCoversWithMovementInput;
	
	/**
	 * The amount of time a player needs to hold the movement input for the movement between covers to trigger.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Moving Between Covers", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float TimeToMoveBetweenCoversWithMovementInput;

	/**
	 * Height added to the move Between Covers UI.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Moving Between Covers", meta = (AllowPrivateAccess = "true", DisplayPriority = -1, ClampMin = 0.f, ClampMax = 200.f, UIMin = 0.f, UIMax = 200.f))
	float HeightAddedToMoveBetweenCoversUI;

	/**
 	* The initial color displayed by the Move Between Covers UI particle.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Moving Between Covers", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	FLinearColor InitialMoveBetweenCoversRibbonColor;

	/**
 	* The final color displayed by the Move Between Covers UI particle.
 	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Movement|Moving Between Covers", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	FLinearColor FinalMoveBetweenCoversRibbonColor;

	
	/********************/
	/*   COVER CAMERA   */
	/********************/

	/**
	 * The maximum distance the player can take cover at without disabling their aiming.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float AimingCancellationDistanceWhenEnteringCover;
	
	/**
	 * The maximum amount of additional camera displacement applied to the camera when the player is moving along the Cover.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float MaxAdditionalCameraDisplacementWhileMoving;
	
	/**
	 * The duration that the shoulder camera change will last for.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	float ShoulderChangeCameraTransitionDuration;
	
	/**
	 * The type of easing applied to the Should Change camera movement.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Camera", meta = (AllowPrivateAccess = "true", DisplayPriority = -1))
	EEaseType ShoulderPositionChangeCurveType;


	/*****************/
	/*     DEBUG     */
	/*****************/
	
	/**
	 * Whether to display the Enter Cover Evaluations Debug Visuals.
	 */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowEnterCoverEvaluationsDebugVisuals;
	
	/**
 	* Whether to print the Player Covered States.
 	*/
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowPlayerCoveredStates;

	/**
 	* Whether to print the Inside Cover States.
 	*/
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowInsideCoverStates;

	/**
	 * Whether to print the Entering Cover States.
	 */
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowEnteringCoverStates;

	UPROPERTY()
	int32 NumberOfParticlesInCoverMovementNiagaraSystem;
	
	UPROPERTY()
	int32 CurrentRibbonID;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<USecondOrder1DSystem> InCoverCameraHorizontalAdditionalRatioSmooth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> BetweenCoversMovementNiagaraComponent;

	UPROPERTY()
	bool bResetOnCover;

	UPROPERTY()
	bool bIsCoverUiOn;

	UPROPERTY()
	float InCoverLateralOffsetInitialValue;
	
#pragma endregion
	
#pragma region VARIABLES
private:
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	float TimeTryingToExitCoverWithInput;

	UPROPERTY()
	float TimeTryingToMoveCoverContinuousRight;
	
	UPROPERTY()
	float TimeTryingToMoveCoverContinuousLeft;

	UPROPERTY()
	float TimeTryingToMoveCoverForwardRight;

	UPROPERTY()
	float TimeTryingToMoveCoverForwardLeft;

	/**
	 * The original Horizontal Camera Ratio value for when the character is covered that was set up in the Camera Component.
	 */
	UPROPERTY()
	float CoveredCameraHorizontalRatioInitialValue;
	
	/**
	 * The smoothed Horizontal Camera Ratio value that gets passed to the Cover
	 */
	UPROPERTY()
	float DesiredCoveredCameraHorizontalRatio;

	UPROPERTY()
	float ChangeToLeftShoulderPercentageComplete;
	
	UPROPERTY()
	USecondOrder1DSystem* CameraAdditionalHorizontalRatioSmoothComponent;
	
	UPROPERTY()
	EMovementBetweenCoversState LastTypeOfMovementBetweenCoversAttempted;

	UPROPERTY()
	TArray<AActor*> CoverParents;

	FTimerHandle TimerHandleVoiceLine;
	FTimerDelegate TimerDelegateVoiceLine;
	const float TIME_IN_COVER_VOICE_LINE = 4.f;
#pragma endregion

#pragma region FUNCTIONS
private:
	UFUNCTION()
	bool CanMoveContinuousBetweenCovers(bool bGoingLeftSection0) const;

	UFUNCTION()
	bool CanMoveForwardBetweenCovers(bool bGoingLeftSection0) const;

	UFUNCTION()
	void DisplayMovingBetweenCoversUI(bool bGoingLeftSection0, bool bContinuousMovement, float ButtonHoldPercentageComplete);

	UFUNCTION()
	void HideMovingBetweenCoversUI();

	UFUNCTION()
	void INTERNAL_DrawDebugEntryPointSphere(const UWorld* World, const FCoverEvaluation& CoverEvaluationToDraw) const;

	UFUNCTION()
	void OnEnterInCoverState();

	UFUNCTION()
	void OnExitInCoverState();

	UFUNCTION()
	void OnEnterBaseInCoverInsideCoverState();

	/**
	 * Used to calculate the minimum Y Input (in-cover) to try to exit cover with a controller.
	 * @return 
	 */
	UFUNCTION()
	float CalculateMaxYInputBackwardsToExitCoverWithController() const;

	/**
	 * Used to calculate the minimum Y Input (in-cover) to try to exit cover with a keyboard.
	 * @return 
	 */
	UFUNCTION()
	float CalculateMaxYInputBackwardsToExitCoverWithKeyboard() const;

	/**
	 * Updating the Camera position when in cover based on the player's input.
	 * @param DeltaTime Time since last call.
	 */
	UFUNCTION()
	void UpdateShoulderCameraValues(const float DeltaTime);

	/**
	 * Sets the Shoulder value in the camera COVER State.
	 */
	UFUNCTION()
	void SetCameraInCoverShoulderValues();
	
	/**
	 * Checks if the player is trying to exit the current cover via an input in the cover's opposite direction.
	 * @param DeltaTime Time since last call.
	 * @return If the player started a Move Between Covers.
	 */
	UFUNCTION()
	bool CheckIfTryingToExitCoverWithMovementInput(const float DeltaTime);

	UFUNCTION()
	void CheckIfMovementIsTryingToMoveBetweenCovers(const float DeltaTime);

	UFUNCTION()
	bool ShouldCheckMovementBetweenCovers() const;

	/**
	 * Evaluates a Cover Point.
	 * @param CoverEvaluationStruct The struct that will be evaluated.
	 * @param PlayerLocation
	 * @param PlayerDirection
	 * @param bPlayerInputDirectionWasValid If the player input was valid, or we are using its camera instead as direction.
	 * @param bWasPlayerStopped Whether the player had a movement input equal to being stopped.
	 * @param MaxEvaluationSoFar The maximum evaluation achieved by every other Cover Evaluation before.
	 * @return True if the Cover Point is better than the last Evaluated Point.
	 */
	UFUNCTION()
	bool EvaluateCoverPosition(
		FCoverEvaluation& CoverEvaluationStruct,
		const FVector& PlayerLocation,
		const FVector& PlayerDirection,
		const FVector& CameraDirection,
		const bool bPlayerInputDirectionWasValid,
		float& MaxEvaluationSoFar) const;

	TArray<AActor*> GetCoversFiltered();

	UFUNCTION()
	void CheckIfContinueInCoverState();
public:
	FOnMoveBetweenCoverDelegate OnMoveBetweenCoverDelegate;
	
	UFUNCTION()
	bool PlayerTryToEnterCover();

	UFUNCTION()
	bool TryToMoveBetweenCovers();

	UFUNCTION()
	bool ShouldAimInCoverOverLeftShoulder() const;

#pragma endregion
	
#pragma region VIRTUAL FUNCTIONS
	
	virtual ECoveredState SetCoveredState(const ECoveredState NewCoveredState) override;

	virtual EInsideCoverState SetInsideCoverState(const EInsideCoverState NewInsideCoverState) override;

	virtual bool NotifyBeginTravellingToCover(UCoverSpline* CoverToEnter, float SectionToEnter) override;

	virtual void NotifyReachedCoverTravellingTowards() override;

	virtual void BaseInCoverTickFunctionality(float DeltaTime) override;
	
	virtual void AimInCoverTickFunctionality(float DeltaTime) override;

	virtual void LerpingToCoverTickFunctionality(float DeltaTime) override;

	virtual void EnteringCoverTickFunctionality(float DeltaTime) override;

	virtual void MoveBetweenCoversTickFunctionality(float DeltaTime) override;
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool TryToTransitionOutOfCover() override;
	
	UFUNCTION() void TurnCoverUIOn();
	UFUNCTION() void TurnCoverUIOff();
	UFUNCTION() void SetPathPoints(const TArray<FVector>& PathPoints);
	UFUNCTION() void SetPathColor(const FLinearColor& Color);
	UFUNCTION() FORCEINLINE bool GetResetOnCover() const { return bResetOnCover; }

	UFUNCTION() FORCEINLINE UNiagaraComponent* GetBetweenCoversMovementNiagaraComponent() const { return BetweenCoversMovementNiagaraComponent; }
	UFUNCTION() FORCEINLINE USecondOrder1DSystem* GetInCoverCameraHorizontalAdditionalRatioSmooth() const { return InCoverCameraHorizontalAdditionalRatioSmooth; }	
#pragma endregion
};
