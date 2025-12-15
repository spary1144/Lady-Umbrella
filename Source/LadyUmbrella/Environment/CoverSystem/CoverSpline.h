// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICoverEntryPoint.h"
#include "CoverParent.h"
#include "Components/SplineComponent.h"
#include "CoverSpline.generated.h"

class AGenericCharacter;
class AAICoverEntryPoint;

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UCoverSpline : public USplineComponent
{
	GENERATED_BODY()

	UCoverSpline();

#pragma region COMPONENTS

private:
	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedStaticMesh;

	UPROPERTY(VisibleAnywhere)
	TArray<AAICoverEntryPoint*> IaEntryPoints;

	/**
 	* A reference to an Entry Point, near Section 0, that is around the corner in the direction the cover is facing.
 	*/
	UPROPERTY(VisibleInstanceOnly, Category="Cover Configuration", meta=(DisplayPriority=3))
	AAICoverEntryPoint* ReachablePointForward_0;

	/**
 	* A reference to an Entry Point, near Section 1, that is around the corner in the direction the cover is facing.
 	*/
	UPROPERTY(VisibleInstanceOnly, Category="Cover Configuration", meta=(DisplayPriority=3))
	AAICoverEntryPoint* ReachablePointForward_1;

	/**
	 * A reference to an Entry Point, near Section 0, that faces the same direction as this point and would be found if the cover continued in its current direction.
	 */
	UPROPERTY(VisibleInstanceOnly, Category="Cover Configuration", meta=(DisplayPriority=4))
	AAICoverEntryPoint* ReachablePointContinuous_0;

	/**
 	* A reference to an Entry Point, near Section 1, that faces the same direction as this point and would be found if the cover continued in its current direction.
 	*/
	UPROPERTY(VisibleInstanceOnly, Category="Cover Configuration", meta=(DisplayPriority=4))
	AAICoverEntryPoint* ReachablePointContinuous_1;
	
#pragma endregion

#pragma region CONSTANTS

private:
	const float MAX_DISTANCE_TO_CONTINUOUS_REACHABLE_POINT = 290.0f;
	const float MAX_DISTANCE_TO_FORWARD_REACHABLE_POINT = 150.0f;
	const float MAX_FORWARD_REACHABLE_POINT_DOT_PRODUCT_ANGLE_DIFFERENCE = 0.8f;
	
	const FCollisionQueryParams IS_TALL_COLLISION_QUERY_PARAMS = FCollisionQueryParams();
	const FVector IS_TALL_CHECK_HEIGHT = FVector(0, 0, 120);  // Maximum height of low covers is 100cm, so this height should only trigger high covers.
	const float IS_TALL_LENGTH_OF_TRACE_LINE = 100.0f;

	const float MAX_COVER_PARENT_DISTANCE = 1000.0f;

	const float SPLINE_TRACK_LINE_TRACE_HEIGHT_OFFSET = 15.0f;

public:
	const FName ENVIRONMENT_NO_COVER_TAG = FName("NotCover");
	const FVector AI_ENTRYPOINT_ELEVATION = FVector(0, 0, 20);

#pragma endregion 

#pragma region PARAMETERS

private:
	/**
	* Whether the Cover is Loopable (does not contain end points).
	*/
	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority = 0))
	bool bIsLooped;

	/**
	 * Whether this cover should be disabled. This will prevent Players and IAs from covering in this Cover. 
	 * It will also prevent IaCoverEntryPoints from spawning.
	 */
	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority = 0))
	bool bIsDisabled;
	
	/**
	 * Swaps whether the Spline is orientated to its right or its left side.
	 */
	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority = 0))
	bool bIsInverted;

	/**
	 * Whether this cover should function as if it was a point, and not a spline.
	 */
	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority = 0))
	bool bIsSinglePointCover;

	/**
	 * The number of IA Entry Points this Cover Spline has.
	 * Is reset when Resetting the Whole Spline.
	 * Can be overriden to force a specific number of Entry Points. After doing so, Update IA entry Points.
	 */
	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority = 1))
	unsigned int NumberOfIaEntryPoints;
	
#pragma endregion

#pragma region VARIABLES
private:
	UPROPERTY()
	bool bNeedsOnConstructReset;

	UPROPERTY()
	bool bNeedsOnConstructUpdate;

public:
	UPROPERTY(VisibleInstanceOnly)
	TArray<AGenericCharacter*> CharactersUsingTheSpline;

#pragma endregion

#pragma region EDITOR ONLY FUNCTIONS

	// Editor Buttons.
private:
	/**
 	* Safe-Checks the Spline's Points, respawns the IA Entry Points and repositions them along the Spline.
 	* Will NOT override the Number of IA Entry Points and their custom Section values.
 	*/
	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = -3), DisplayName=UpdateCoverSpline)
	void OrderUpdateSplinePoints();
	
	/**
	* Safe-Checks the Spline's Points, resets the number of IA Entry Points, respawns them with new section values, and repositions them along the Spline.
	* Will override the Number of IA Entry Points and their custom Section values.
	*/
	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = 3), DisplayName=ResetCoverSpline)
	void OrderResetSplinePoints();

public:
	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = 4))
	void ShowPlayerEntryPoints();

	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = 5))
	void HidePlayerEntryPoints();

	
	// Other Editor Functions.
	UFUNCTION()
	void ResetCoverSpline();

	UFUNCTION()
	void UpdateCoverSpline();

	UFUNCTION()
	void ResetIaEntryPointsOwningSplines();

	UFUNCTION()
	FORCEINLINE bool NeedsOnConstructReset() const { return bNeedsOnConstructReset; }

	UFUNCTION()
	FORCEINLINE bool NeedsOnConstructUpdate() const { return bNeedsOnConstructUpdate; }

	UFUNCTION()
	FORCEINLINE void OrderOnConstructUpdate() { bNeedsOnConstructUpdate = true; }
	
	/**
	 * Sorts the IA Entry Points by Section (Smaller -> Bigger).
	 * This ensures that the point with Section 0 is placed first in the array and the point with Section 1 is placed last.
	 */
	UFUNCTION()
	void SortIAEntryPoints();

#pragma endregion

#pragma region PUBLIC FUNCTIONS

public:
	UFUNCTION()
	FORCEINLINE bool IsLoop() const { return bIsLooped; };

	UFUNCTION()
	FORCEINLINE bool IsInverted() const { return bIsInverted; };

	UFUNCTION()
	FORCEINLINE bool IsDisabled() const { return bIsDisabled; };

	UFUNCTION()
	FORCEINLINE bool IsSinglePointCover() const { return bIsSinglePointCover; };

	/**
	 * Calculates which points the player can use to enter the Cover Spline.
	 * @param BufferArrayOfEntryPoints A buffer array to store the resulting position of points of entrance the Spline has.
	 * @param SectionLengthBuffer A buffer where the length of the sections will be stored.
	 */
	UFUNCTION()
	void CalculatePlayerEntryPoints(TArray<FVector>& BufferArrayOfEntryPoints, float& SectionLengthBuffer);
	
	/**
	 * Given a time from 0 to 1, shoots LineTraces to check if the cover is considered to be tall at that time.
	 * @param Time A value from 0 to 1, which indicates a point in the Spline from its first point (0) to its last (1).
	 * @return True if the CoverSpline is tall at the given time.
	 */
	UFUNCTION(BlueprintCallable)
	bool TestTallCoverAtTime(const float Time) const;
	
	UFUNCTION()
	bool TestTallCoverAtTimeWithHorizontalDisplacement(const float Time, const float HorizontalDisplacement) const;
	
	UFUNCTION()
	FORCEINLINE FVector FindLocationAtTime(const double _Time) const
	{ return GetLocationAtTime(FMath::Clamp(_Time, 0.0f, 1.0f), ESplineCoordinateSpace::World, true); };
	
	UFUNCTION()
	FORCEINLINE FRotator FindForwardRotationAtTime(const double _Time) const
	{ return FRotator(0, GetRotationAtTime(bIsSinglePointCover ? 0 : FMath::Clamp(_Time, 0.0f, 1.0f), ESplineCoordinateSpace::World, true).Yaw  + (bIsInverted ? 90 : -90), 0); }

	UFUNCTION()
	FORCEINLINE FRotator FindRightRotationAtTime(const double _Time) const
	{ return FRotator(0, GetRotationAtTime(bIsSinglePointCover ? 0 : FMath::Clamp(_Time, 0.0f, 1.0f), ESplineCoordinateSpace::World, true).Yaw  + (bIsInverted ? 180 : 0), 0); }
	
	UFUNCTION()
	FORCEINLINE FRotator FindLeftRotationAtTime(const double _Time) const
	{ return FRotator(0, GetRotationAtTime(bIsSinglePointCover ? 0 : FMath::Clamp(_Time, 0.0f, 1.0f), ESplineCoordinateSpace::World, true).Yaw  + (bIsInverted ? 0 : 180), 0); }

	/**
	 * 
	 * @param _Character The character that we want to check if is using the Spline.
	 */
	UFUNCTION()
	FORCEINLINE bool IsCharacterUsingTheSpline(AGenericCharacter* _Character) const { return CharactersUsingTheSpline.Contains(_Character); };

	/**
	 * 
	 * @return Whether there are any characters currently using the Cover Spline.
	 */
	UFUNCTION()	
	FORCEINLINE bool IsSplineEmpty() const { return CharactersUsingTheSpline.IsEmpty(); };
	
	/**
	 * 
	 * @param _Character The character that we want to register to be using the Spline.
	 */
	UFUNCTION()
	FORCEINLINE void RegisterEnterSpline(AGenericCharacter* _Character){ CharactersUsingTheSpline.AddUnique(_Character); };
	
	/**
	 * 
	 * @param _Character The character that we want to unregister from using the Spline.
	 */
	UFUNCTION()
	FORCEINLINE void RegisterExitSpline(AGenericCharacter* _Character){ CharactersUsingTheSpline.Remove(_Character); };

	UFUNCTION()
	FORCEINLINE unsigned int HowManyCharactersUsingTheSpline() const{ return CharactersUsingTheSpline.Num(); };

	UFUNCTION()
	int GetZone() const;

	UFUNCTION()
	void DestroyCover();

	UFUNCTION()
	void DestroyAllAttachedIaEntryPoints();

	UFUNCTION(BlueprintCallable)
	ACoverParent* TryGetParent() const;

	virtual void OnRegister() override;

	UFUNCTION(BlueprintCallable)
	const TArray<AAICoverEntryPoint*>& GetIaEntryPoints() const { return IaEntryPoints; };

	UFUNCTION()
	FORCEINLINE AAICoverEntryPoint* GetReachablePointContinuous_0() const { return GetValid(ReachablePointContinuous_0); };

	UFUNCTION()
	FORCEINLINE AAICoverEntryPoint* GetReachablePointContinuous_1() const { return GetValid(ReachablePointContinuous_1); };

	UFUNCTION()
	FORCEINLINE AAICoverEntryPoint* GetReachablePointForward_0() const { return GetValid(ReachablePointForward_0); };

	UFUNCTION()
	FORCEINLINE AAICoverEntryPoint* GetReachablePointForward_1() const { return GetValid(ReachablePointForward_1); };

	UFUNCTION()
	AAICoverEntryPoint* GetMoveBetweenCoversPoint(const bool bGetPoint0, const bool bGetContinuousPoint) const;
	
#pragma endregion

#pragma region PRIVATE METHODS

private:
	/**
	 * Resets the Number of IA Entry Points (not the position or the section value of the Entry Points themselves).
	 */
	UFUNCTION()
	void ResetNumberOfIAEntryPoints();

	UFUNCTION()
	void ResetSectionsOfIaEntryPoints();

	UFUNCTION()
	void UpdateSplineTrackPoints();
	
	/**
	 * 
	 * @return The number of Entry Points the Player should have access to.
	 */
	UFUNCTION()
	unsigned int CalculateNumberOfPlayerEntryPoints() const;
	
	/**
	 * 
	 * @return TRUE if changes were made to the Actor. FALSE if no changes were made.
	 */
	UFUNCTION()
	bool EnsureSpawnOfIaEntryPoints();
	
	/**
	* Positions the AIEntryCoverPoint actors dependent on this Cover Spline.
	*/
	UFUNCTION()
	void PlaceAIEntryPoints();

	/**
	 * Takes the Instantiated Static Mesh Component and updates the Player Entry Point Positions.
	 */
	UFUNCTION()
	void CreatePlayerEntryPointsWidgets();

	/**
	 * Checks if our connections are still valid and, if needed, for new reachable points in nearby Covers.
	 * @param CoverThatOrderedTheUpdate (OPTIONAL) If this function is called from another Spline, set this parameter to avoid stack overflows where both covers keep telling each other to update endlessly.
	 */
	UFUNCTION()
	void UpdateReachableCovers(const UCoverSpline* CoverThatOrderedTheUpdate = nullptr);

	UFUNCTION()
	bool IsPointReachableContinuous(bool FromPoint0, const AAICoverEntryPoint* Point) const;

	UFUNCTION()
	bool IsPointReachableForward(bool FromPoint0, AAICoverEntryPoint* Point) const;

	UFUNCTION()
	void DrawDebugConnectionLine(const UWorld* World, const bool FromPoint0, const AAICoverEntryPoint* TargetEntryPoint, const FColor Color) const;

#pragma endregion
};