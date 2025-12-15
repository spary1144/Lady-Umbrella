// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FreeRoamCoverAnimationState.h"
#include "FreeRoamCoverComponent.generated.h"


class UCapsuleComponent;
class AGenericCharacter;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoverExited);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UFreeRoamCoverComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsAtCorner;
public:	
	UFreeRoamCoverComponent();
	void InitializeCapsule();

	UPROPERTY()
	AGenericCharacter* OwnerCharacter;

	
	/** Enables debug drawing and logs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Debug", meta = (ToolTip = "Enable debug drawing and logging"))
	bool DoDebug;

	/** Radius used when detecting cover surfaces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Detection", meta = (ToolTip = "Radius used when detecting nearby cover surfaces"))
	float StartDetectionRadius;

	/** Force pulling the character into the wall once in cover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Movement", meta = (ToolTip = "Force pulling the character into the wall while in cover"))
	float PullForce;

	/** Radius of the sphere trace to check for cover height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Height", meta = (ToolTip = "Radius of sphere trace used to determine if crouch or standing cover"))
	float HeightCoverCheckRadius;

	/** Z offset for the height checking trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Height", meta = (ToolTip = "Vertical offset for starting the cover height trace"))
	float HeightCheckZOffset;

	/** Range to check for corners during cover movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Corner", meta = (ToolTip = "Distance forward to check for corner availability while in cover"))
	float CornerCheckRange;
	
	/** Rotation Speed of character in a cover corner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Corner", meta = (ToolTip = "Rotation Speed of character in a cover corner"))
	float RotationInterpSpeed;
	
	/** Movement speed while in cover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Movement", meta = (ToolTip = "Maximum walking speed while in cover"))
	float NormalCoverSpeed;

	/** Sphere trace distance used when transitioning between cover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Transition", meta = (ToolTip = "Distance to trace ahead for transitioning between cover points"))
	float TransitionTraceDistance;

	/** Cover transition animation montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Animation", meta = (ToolTip = "Animation played when transitioning between cover spots"))
	UAnimMontage* CoverTransitionMontage;

	/** Z offset for positioning the secondary collision capsule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Capsule", meta = (ToolTip = "Z offset to position the secondary capsule used during crouch/cover"))
	float CapsuleOffsetZ;

	/** Radius of the secondary collision capsule used in cover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Capsule", meta = (ToolTip = "Radius of the secondary capsule component used while in cover"))
	float CapsuleRadius;

	/** Half-height of the secondary collision capsule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Capsule", meta = (ToolTip = "Half-height of the secondary capsule component used while in cover"))
	float CapsuleHalfHeight;

	/** Distance to offset the player back from the wall when entering cover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Entry", meta = (ToolTip = "How far the character should be offset from the wall when entering cover"))
	float CoverOffsetFromWall;

	/** Distance to trace forward when checking for cover height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Height", meta = (ToolTip = "Length of the forward trace when checking cover height"))
	float CoverHeightForwardTraceLength;

	/** Multiplier for forward trace distance when checking for corner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Corner", meta = (ToolTip = "Multiplier used to scale forward trace when checking corners"))
	float CornerTraceForwardScale;

	/** Multiplier for side trace distance when checking for corner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Corner", meta = (ToolTip = "Multiplier used to scale sideways trace when checking corners"))
	float CornerTraceSideScale;

	/** Multiplier applied to detection radius when validating cover presence */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Validation", meta = (ToolTip = "Multiplier for the sphere trace radius when validating if cover is still valid"))
	float CoverValidationRadiusMultiplier;

	/** Threshold to determine if character is facing away from cover normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Direction", meta = (ToolTip = "Threshold for dot product to determine if the character is facing away from cover"))
	float FacingAwayThreshold;

	/** Current cover height state (standing, crouching, none) */
	UPROPERTY(BlueprintReadOnly, Category = "Cover|Height")
	ECoverHeightState CoverHeightState;

	/** Last normal of the surface used for cover */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|State")
	FVector LastCoverNormal;

	/** Position of the cover point being used */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|State")
	FVector CoverLocation;

	/** Capsule component used for additional collision in crouching/cover */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Capsule")
	UCapsuleComponent* CoverCapsule;
	
	UPROPERTY(BlueprintAssignable, Category="Cover")
	FOnCoverExited OnCoverExited;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Exit", 
	meta = (ToolTip = "Minimum dot product between input direction and wall normal required to count as moving away. Higher = stricter."))
	float ExitAwayDotThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Exit", 
		meta = (ToolTip = "Distance of a short probe trace in the wall-normal direction to confirm there is no wall still in front."))
	float ExitProbeDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Exit", 
		meta = (ToolTip = "How far the character must drift away from the wall before cover is released, to avoid accidental exits."))
	float ExitSlackFromWall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cover|Exit", meta=(ToolTip="Seconds after entering cover during which exit-by-input is ignored."))
	float ExitGraceTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cover|Exit", meta=(ToolTip="Minimum input magnitude (0..1) required to consider exit-by-input."))
	float ExitMinInputMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cover|Exit", meta=(ToolTip="Minimum horizontal speed (cm/s) to consider velocity as exit intent (when no input)."))
	float ExitMinSpeed;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	bool IsInFreeCover;

	double CoverEnterTime = 0.0;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION()
	void PullToCover();
	UFUNCTION()
	void StartCover();
	UFUNCTION()
	void CheckCoverHeight();
	UFUNCTION()
	bool CheckCorner();
	UFUNCTION()
	bool CheckIsInCover() const;
	UFUNCTION()
	void ResetCoverVariables();
	UFUNCTION()
	void ToggleCapsuleCollision() const;

	UFUNCTION()
	void TryCoverTransition();
	UFUNCTION()
	bool IsFacingAwayFromCover() const;
	
	// Utility trace helpers
	FHitResult SphereTraceSingle(const FVector& Start, const FVector& End, float Radius) const;
	TArray<FHitResult> SphereTraceMulti(const FVector& Start, const FVector& End, float Radius) const;
	bool ShouldExitCoverByInput() const;

	FORCEINLINE bool ShouldBlockMovement() const { return IsAtCorner && IsInFreeCover; }

};
