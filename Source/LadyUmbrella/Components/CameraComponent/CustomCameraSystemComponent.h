//  
// CameraSystemComponent.h
// 
// Camera Pitch & Yaw player input.
// Management of Camera Collisions and Adjustments.
// Management of camera States (switching and behaviours). 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "CameraComponentTypes/FCameraMultipliers.h"
#include "GameFramework/SpringArmComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/FCameraState.h"
#include "Runtime/CinematicCamera/Public/CineCameraComponent.h"

#include "CustomCameraSystemComponent.generated.h"

enum class EModifierType : uint8;
enum class ECameraInputSpeed : uint8;
enum class ECameraStateKey : uint8;
enum class EShakeSource : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UCustomCameraSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Combat", meta = (AllowPrivateAccess = "true"))
	float CameraLookMultiplierAimAssist;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | General", meta = (AllowPrivateAccess = "true"))
	bool bResetOnCover; // Toggle if the camera position should reset on cover.
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | General", meta = (AllowPrivateAccess = "true", ClampMin = 0.01f, ClampMax = 10.f, UIMin = 0.01f, UIMax = 10.f))
	float ResetStepSize; // Step per second for camera reset rotation (the higher, the faster the camera resets).
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCineCameraComponent> FollowCamera; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Camera | Sensibility", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ManualYawCurve; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera | Sensibility", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ManualPitchCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera | Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault))
	TMap<FName, FCameraMultipliers> CameraMultipliers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera | Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault, ClampMin = "-90.0", ClampMax = "90.0", UIMin = "-90.0", UMax = "90.0"))
	FVector2D ViewPitchRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera | Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault, ClampMin = "0.001", ClampMax = "1.0", UIMin = "0.001", UMax = "1.0"))
	float InputAccelerationRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera | Sensibility", meta = (AllowPrivateAccess = "true", NoResetToDefault, ClampMin = "0.001", ClampMax = "1.0", UIMin = "0.001", UMax = "1.0"))
	float InputDecelerationRate; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | States", meta = (AllowPrivateAccess = "true"))
	TMap<ECameraStateKey, FCameraState> CameraStates; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | States", meta = (AllowPrivateAccess = "true"))
	float RelativeCapsuleLocation; // For ECameraAttachment::CAPSULE_TOP

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | States", meta = (AllowPrivateAccess = "true"))
	FString MeshTopSocket; // For ECameraAttachment::MESH_TOP
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | States", meta = (AllowPrivateAccess = "true"))
	FString MuzzleTopSocket; // For ECameraAttachment::MUZZLE
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | States", meta = (AllowPrivateAccess = "true"))
	float RelativeLowCoverCollisionHeightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Collisions", meta = (AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.0f, UIMin = 0.f, UIMax = 100.0f))
	float CameraCollisionRadius; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Yaw Assistance", meta = (EditConditionHides, AllowPrivateAccess = "true"))
	bool bYawAssistance; // Time without input until sideways movement affects camera rotation.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Yaw Assistance", meta = (EditCondition = "bYawAssistance", InlineEditCondition, AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 1.0f, UIMin = 0.f, UIMax = 1.0f))
	float CameraMoveAssistStrength; // How aggressive the camera correction is when following the movement input.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Yaw Assistance", meta = (EditCondition = "bYawAssistance", AllowPrivateAccess = "true", Units = "Seconds"))
	float YawAssistanceDelay; // Time without input until sideways movement affects camera rotation.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera | Yaw Assistance", meta = (EditCondition = "bYawAssistance", AllowPrivateAccess = "true", ClampMin = 0.f, ClampMax = 100.0f, UIMin = 0.f, UIMax = 100.0f, Units = "Times"))
	float CameraSideMultiplier; // How aggressive should the camera correction is when correcting on side collision.
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera | Shakes", meta = (AllowPrivateAccess = "true"))
	TMap<EShakeSource, TSubclassOf<class UCameraShakeBase>> CameraShakes;
	
public:

	UCustomCameraSystemComponent();
	
	void CameraTick(const float& DeltaSeconds);
	void SetUpCamera();
	void ResetCameraParams();
	
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE float GetCameraLookMultiplierAimAssist() const { return CameraLookMultiplierAimAssist; }
	FORCEINLINE ECameraStateKey GetCurrentCameraStateKey() const { return CurrentState; };
	FORCEINLINE FCameraState& GetCurrentCameraState() const { return const_cast<FCameraState&>(CameraStates[CurrentState]); };
	FORCEINLINE ECameraInputSpeed GetCameraInputMotion() const { return CurrentInputSpeedState; }
	FORCEINLINE FCameraState& GetCameraState(ECameraStateKey StateKey) const { return const_cast<FCameraState&>(CameraStates[StateKey]); };
	FORCEINLINE TObjectPtr<UCineCameraComponent> GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetProbeRadius() const { return CameraCollisionRadius; }
	FORCEINLINE float GetRelativeTopOfCapsule() const { return RelativeCapsuleLocation; }

	FORCEINLINE FVector2D GetSensibilityRange(const FName& Device) { return CameraMultipliers[Device].SensibilityRange; }

	FORCEINLINE void SetCameraLookMultiplierAimAssist(const float Value) { CameraLookMultiplierAimAssist = Value; }
	FORCEINLINE void SetProbeRadius(float NewRadius) { CameraCollisionRadius = NewRadius; }
	FORCEINLINE void SetFocusLocation(const FVector& TargetPoint) { LookAtTarget = TargetPoint; }
	
	FVector2D GetAdjustedLookVector(FString DeviceType, FVector2D LookAxisVector);
	float GetCurrentSensibility(FName Device, bool bHorizontal = true);
	FVector2D GetDeceleratingLookVector(float DeltaTime);
	float GetEquivalentDistanceFromCurrentOffset() const;
	float GetFieldOfView() const;
	FVector GetOffsetFromDistance(const float BoomDistance, const FVector TheoreticalOffset) const;

	void SetCameraInputMotion (ECameraInputSpeed MotionType);
	void SetCameraState(ECameraStateKey StateKey);
	void SetCameraStateDistance(ECameraStateKey StateKey, float Distance2Player);
	void SetCameraStateShoulder(ECameraStateKey StateKey, bool InvertedShoulder);
	void SetCameraStateOffsetValue(ECameraStateKey StateKey, FVector2D Ratio);
	void UpdateForAttachment(const FVector& OriginalLocation);
	
	bool IsActorInFrustum(const AActor* Actor) const;
	bool IsComponentInFrustum(const UPrimitiveComponent* Actor) const;
	bool IsOutsideFrustum(const AActor* Actor, const float Tolerance) const;
	
	void MoveAssistance(const FVector2D& MoveAxisVector, const bool bMoveControlledByCover) const;

	void UpdateCurrentControlSensitivity(const FString& DeviceType = "KBM", const float NewValue = 1.f, const bool bHorizontal = true);
	void UpdateModifier(EModifierType Modifier, TArray<float> NewValue);
	void UpdateModifier(const FName& DeviceType, const EModifierType Modifier, const float NewValue);
	
	UFUNCTION(BlueprintCallable, Category = "Camera Shakes", meta = (AllowPrivateAccess = "true"))
	void MakeCameraShake(const EShakeSource& Type, FVector Location = FVector(0.f, 0.f, 0.f), float RadiusFrom = 0.f, float RadiusTo = 2500.f, bool bMirrored = false);

	UFUNCTION(BlueprintCallable, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	void ResetCameraPosition();
	
private:
	virtual void BeginPlay() override;
	
	void CameraCorrectiveMovement(const FHitResult& Hit);
	FVector ComputeOffset(const FCameraState& CurrentState);
	void ClampLateralOffset(FVector& DesiredOffset, const FVector& HitLocation, const bool FromRight) const; 
	void EvaluateCollision(const FHitResult& CollisionResult); 
	void ManageArmExpectedValue(const float ExpectedDistance, FVector ExpectedOffset); 
	void ManageCameraCollisions();
	void ResetCamProgress(const double& DeltaSeconds);
	void UpdateForCameraState(const float& DeltaSeconds); 
	void UpdateCameraPosition(const float& DeltaSeconds);

	// Interpolation (will be removed from here)
	float InterpTimer;
	float InterpolationStart_Dist;
	FVector InterpolationStart_Offset;
	
	TArray<FHitResult> CameraHits;
	TArray<TObjectPtr<AActor>> IgnoredActorsCamera;
	ECameraStateKey CurrentState;
	ECameraInputSpeed CurrentInputSpeedState;

	float TargetBoomLength;
	FVector TargetOffset;
	FVector CurrentOffset;

	FVector LookAtTarget;
	
	float CurrentTargetYaw;
	float YawAssistanceTimer;
	float DelayedCollisionTimer;
	float CurrentAccelerationInput;
	FVector2D PreviousInputAdjust;
	
	bool bCameraIsResetting;

	const float TwoPIThirds = (2.f * PI) / 3.f;
	const float MaxFocusDist = 30000.f; // In centimeters
	
	// Move Interpolation Somewhere else
#pragma region INTERPOLATION
private:
	// Interpolate float from Current to Target (Next) with type: ECameraInterpolation
	float InterpolateFloat();
	void PrintCurrentState();

	// Ensures initial and final speed are 0 [Currently not working]
	template <class T>
	T SmoothStepLerp(T Current, T Next, T StepPercent) { return FMath::SmoothStep(Current, Next, StepPercent); }

	// Slow initial speed
	template<class T>
	T EaseInLerp(T Current, T Next, T StepPercent, T Exp) { return FMath::Lerp(Current, Next, FMath::Pow(StepPercent, Exp)); }
	
	// Slow final speed
	template<class T>
	T EaseOutLerp(T Current, T Next, T StepPercent, T Exp) { return FMath::Lerp(Current, Next, 1.f - FMath::Pow(1.f - StepPercent, Exp)); }

	template<class T>
	T ElasticEaseInLerp(T Current, T Next, T StepPercent)
	{
		float Step = StepPercent == 0.f ? 0.f : StepPercent == 1.f ? 1.f : -FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds);
		return FMath::Lerp(Current, Next, Step);
	}

	template<class T>
	T ElasticEaseOutLerp(T Current, T Next, T StepPercent)
	{
		float Step = StepPercent == 0.f ? 0.f : StepPercent == 1.f ? 1.f : FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds) + 1.f;
		return FMath::Lerp(Current, Next, Step);
	}
	
	template<class T>
	T ElasticEaseInOutLerp(T Current, T Next, T StepPercent)
	{
		float Step = StepPercent == 0.f ? 0.f : StepPercent == 1.f ? 1.f : StepPercent < 0.5f
		? -FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds)
		: FMath::Pow(2.f, -10.f * StepPercent) * FMath::Sin((StepPercent * 10.f - 0.75f) * TwoPIThirds) + 1.f;
		return FMath::Lerp(Current, Next, Step);
	}
	
#pragma endregion

};
