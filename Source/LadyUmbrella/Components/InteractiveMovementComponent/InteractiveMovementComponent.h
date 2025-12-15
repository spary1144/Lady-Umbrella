#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveMovementComponent.generated.h"

enum class EUmbrellaLocation : uint8;
class AGenericCharacter;
class APlayerCharacter;
class UAnimMontage;
class UCapsuleComponent;
class UCharacterMovementComponent;
class UKickInterface;
class UMotionWarpingComponent;
class USkeletalMeshComponent;
enum class EVoiceLineState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMantleDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVaultDelegate);
DECLARE_DELEGATE_OneParam(FOnMantleStarted, EUmbrellaLocation);
DECLARE_DELEGATE_OneParam(FOnZiplineStateChanged, EUmbrellaLocation);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LADYUMBRELLA_API UInteractiveMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	// +-------------------------------------------------+
	// |                                                 |
	// |				    Components                   |
	// |                                                 |
	// +-------------------------------------------------+
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Owner actor of this component."))
	AGenericCharacter* GenericCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Owner actor of this component."))
	APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Capsule component of the character."))
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Skeletal mesh of the character."))
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Character movement component."))
	UCharacterMovementComponent* CharacterMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Motion warping component used for custom animation root motion."))
	UMotionWarpingComponent* MotionWarping;

	// +-------------------------------------------------+
	// |                                                 |
	// |				    Constants                    |
	// |                                                 |
	// +-------------------------------------------------+
	const float MANTLE_MOVE_COMPONENT_OVERTIME = 0.75f;
	const float KICK_LAUNCH_MAGNITUDE = 100.0f;
	const float KICK_BOX_TRACE_LENGTH = 100.f;
	const float CAM_RADIUS_MODIFIER = 4.0f;
	const float STICK_TO_WALL_LINE_TRACE_STEP = 10.f;
	const float STICK_TO_WALL_LINE_TRACE_LENGTH = 180.f;
	const float VAULT_OVER_MARGIN = 10.f;
	const float MANTLE_LAND_LOCATION_OFFSET = 15.f;
	const float MANTLE_CAPSULE_TARGET_LOCATION_OFFSET = 30.f;
	const float VOID_VAULT_ERROR_TOLERANCE = 10.f;
	const float CHECK_FLOOR_FORWARD_OFFSET = 50.f;
	const float CHECK_FLOOR_VERTICAL_OFFSET = 100.f;

	

	// +-------------------------------------------------+
	// |                                                 |
	// |				    Parameters                   |
	// |                                                 |
	// +-------------------------------------------------+

	// ANIMATION
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true", ToolTip = "End position of the mantle movement."))
	UAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for mantling."))
	UAnimMontage* CurrentMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for mantling."))
	UAnimMontage* MantleMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for long vaulting."))
	UAnimMontage* VaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for short vaulting."))
	UAnimMontage* ShortVaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for stepping up."))
	UAnimMontage* StepMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Roll", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage used for rolling."))
	UAnimMontage* RollMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vertical Hook", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* VerticalHookMovementMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Swing", meta = (AllowPrivateAccess = "true", ToolTip = "Animation montage played when swinging."))
	UAnimMontage* SwingMontage;
	
	// MANTLE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Vertical offset for the mantle sphere trace launched from the player."))
	float MantleSphereTraceOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to stick to wall when mantling."))
	float StickToWallDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "Base forward distance to start detecting a potential climbable surface."))
	float MantleDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "End position of the mantle movement."))
	FVector MantleStartPos;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true", ToolTip = "End position of the mantle movement."))
	FVector MantleHandPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	float MantleAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	FRotator SkeletalMeshInitialRelativeRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	FVector SkeletalMeshInitialRelativeLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	FVector CameraTargetPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	float MantleCameraVerticalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	float MantleTraceFanMinAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	float MantleTraceFanMaxAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Mantle", meta = (AllowPrivateAccess = "true"))
	float MantleTraceFanStep;
	
	// VAULT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check if vault is possible (Length of the trace launched from the player )."))
	float CheckVaultDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum length of vaultable surface."))
	float MinVaultableLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum vaultable height."))
	float VaultOverHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the vault animation."))
	float VaultAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the short vault animation."))
	float ShortVaultAnimPlayRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the short vault animation."))
	float LongVaultAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be able to vault."))
	float MaxVaultDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be able to vault."))
	float LongVaultDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be considered a short vault."))
	float ShortVaultDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to be considered a short vault."))
	float VaultLandTraceLenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum surface normal Z value to allow interaction. (1 = Horizontal; 0 = Vertical)"))
	float SurfaceNormalZValue;
	
	// Kick
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault|Kick", meta = (AllowPrivateAccess = "true", ToolTip = "Half-size of the box trace used to detect kickable enemies during vaulting."))
	FVector KickedBoxTraceExtent;
	
	// STEP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate of the step animation."))
	float StepAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check for stepping possibility."))
	int StepCheckDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check for stepping possibility."))
	float StepLandVerticalOffset;
	
	// SWING
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Swing", meta = (AllowPrivateAccess = "true", ToolTip = "Playback speed of the swing animation montage."))
	float SwingAnimationPlayRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Swing", meta = (AllowPrivateAccess = "true", ToolTip = "Maximum distance to check for swing possibility."))
	float CheckSwingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Swing", meta = (AllowPrivateAccess = "true", ToolTip = "Vertical offset applied to swing trace origin."))
	float SwingTraceVerticalOffset;

	// ROLL
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Roll", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum height to be able to roll."))
	float FallInitialHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Roll", meta = (AllowPrivateAccess = "true", ToolTip = "Minimum height to be able to roll."))
	float MinHeightToRoll;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Roll", meta = (AllowPrivateAccess = "true", ToolTip = "Playback rate for the roll animation."))
	float RollMontagePlayRate;

	// VERTICAL HOOK
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement|Vertical Hook", meta = (AllowPrivateAccess = "true"))
	float VerticalHookMovementPlayRate;

	// +-------------------------------------------------+
	// |                                                 |
	// |				    Variables                    |
	// |                                                 |
	// +-------------------------------------------------+

	// VAULT
	UPROPERTY()
	float SphereTraceRadius;
	
	UPROPERTY()
	FVector SphereTraceVerticalOffset;
	
	UPROPERTY()
	float SphereTraceLength;
	
	UPROPERTY()
	int SphereTraceStep;
	
	UPROPERTY()
	int MinimumLandSpace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "True if current vault is a short vault."))
	bool bShortVault;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "True if current vault is a short vault."))
	bool bLongVault;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Start position of the vault movement."))
	FVector VaultStartPos;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Middle position of the vault movement."))
	FVector VaultMiddlePos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Landing position after vaulting."))
	FVector VaultLandPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Landing position after vaulting."))
	bool bIsVoidVault;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault", meta = (AllowPrivateAccess = "true", ToolTip = "Whether motion warping can be applied to vault."))
	bool bCanWarp = false;

	// Kick
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault|Kick", meta = (AllowPrivateAccess = "true", ToolTip = "Result of the hit when kicking during vault."))
	FHitResult KickHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Vault|Kick", meta = (AllowPrivateAccess = "true", ToolTip = "Actor hit and kicked during vault."))
	AActor* KickedActor;
	
	// STEP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Landing position of the step."))
	FVector StepLandPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive Movement|Step", meta = (AllowPrivateAccess = "true", ToolTip = "Starting position of the step."))
	FVector StepStartPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bInZipline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bStrafing;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsGrappling;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSwinging;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInRope;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	float StrafeWalkSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	float OpenShieldWalkSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Boost", meta = (AllowPrivateAccess = "true"))
	bool bIsApplyingJumpBoost;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Boost", meta = (AllowPrivateAccess = "true"))
	float JumpBoostDistance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Jump Boost", meta = (AllowPrivateAccess = "true"))
	float JumpBoostRemaining;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Boost", meta = (AllowPrivateAccess = "true"))
	float JumpBoostSpeed;

public:	

	// NOTE: Delegates CANNOT be private because otherwise they cant broadcast outwards.
	UPROPERTY(BlueprintAssignable, Category = "Delegates") FOnMantleDelegate OnMantleDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Delegates") FOnVaultDelegate  OnVaultDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Delegates") FOnSwingDelegate  OnSwingDelegate;
	FOnMantleStarted OnMantleStarted;
	FOnZiplineStateChanged OnZiplineStateChanged;

	UInteractiveMovementComponent();
	
	void SetInZipline(const bool bValue);
	FORCEINLINE void SetStrafing(const bool bValue) { bStrafing = bValue; }

	FORCEINLINE bool IsInZipline() const { return bInZipline; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsStrafing() const { return bStrafing; }
	FORCEINLINE float GetJumpBoostDistance() const { return JumpBoostDistance; }
	FORCEINLINE float GetJumpBoostRemaining() const { return JumpBoostRemaining; }
	FORCEINLINE float GetJumpBoostSpeed() const { return JumpBoostSpeed; }
	FORCEINLINE float GetMaxWalkSpeed() const { return MaxWalkSpeed; }
	FORCEINLINE float GetOpenShieldWalkSpeed() const { return OpenShieldWalkSpeed; } 
	FORCEINLINE float GetStrafeWalkSpeed() const { return StrafeWalkSpeed; }
	FORCEINLINE UAnimMontage* GetVerticalHookMovementMontage() const { return VerticalHookMovementMontage; }
	FORCEINLINE float GetVerticalHookAnimPlayRate() const { return VerticalHookMovementPlayRate; }
	FORCEINLINE UAnimMontage* GetSwingMontage() const { return SwingMontage; }
	FORCEINLINE float GetSwingAnimPlayRate() const { return SwingAnimationPlayRate; }
	FORCEINLINE bool  IsApplyingJumpBoost() const { return bIsApplyingJumpBoost; }
	FORCEINLINE bool  IsGrappling() const { return bIsGrappling; }
	FORCEINLINE bool  IsSwinging() const { return bIsSwinging; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool  IsInRope() const { return bIsInRope; }

	FORCEINLINE void SetJumpBoostRemaining(const float Value) { JumpBoostRemaining = Value; }
	FORCEINLINE void SetIsApplyingJumpBoost(const bool bValue) { bIsApplyingJumpBoost = bValue; }
	FORCEINLINE void SetIsGrappling(const bool bValue) { bIsGrappling = bValue; }
	FORCEINLINE void SetIsSwinging(const bool bValue) { bIsSwinging = bValue; }
	FORCEINLINE void SetIsInRope(const bool bValue) { bIsInRope = bValue; }
	FORCEINLINE void SetMaxWalkSpeed(const float Value) { MaxWalkSpeed = Value; }
	FORCEINLINE void SetOpenShieldWalkSpeed(const float Value) { OpenShieldWalkSpeed = Value; } 
	FORCEINLINE void SetStrafeWalkSpeed(const float Value) { StrafeWalkSpeed = Value; }
	FORCEINLINE void SetFallInitialHeight(const float Value) { FallInitialHeight = Value; }
	
protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION() bool CheckFloor(const float InputXYSizeSquared);
	
	UFUNCTION() void TryToApplyJumpBoost(float DeltaTime);
		
	UFUNCTION()
	bool TryToMantle();

	UFUNCTION()
	void RotatePlayerToFaceWall(const TArray<AActor*>& IgnoredActors, const int& i, const float& BestAngleToMantle);
	
	UFUNCTION()
	void CalculateBestAngleToMantle(const TArray<AActor*>& IgnoredActors, const int& i, float& BestAngleToMantle) const;

	
	UFUNCTION()
	void MantleCameraDetach();

	UFUNCTION()
	bool DoesHaveEnoughRoomToMantle(const FVector& OutHitLocation, const TArray<AActor*>& IgnoredActors, const float& BestAngleToMantle) const;

	UFUNCTION()
	void MoveToTargetFinished();

	UFUNCTION()
	void CameraMoveToTargetFinished();

	UFUNCTION()
	void Mantle();

	UFUNCTION()
	void TryToRoll();
	
	UFUNCTION()
	bool CheckSwing() const;

	UFUNCTION()
	bool CheckVault();

	UFUNCTION()
	bool SetUpVault(const FVector& StartVaultLocation, TArray<AActor*> IgnoredActors);
	
	UFUNCTION()
	bool VaultMotionWarp();

	UFUNCTION()
	bool StepMotionWarp();

	UFUNCTION()
	void OnMotionWarpStarted(FName NotifyName);

	UFUNCTION()
	void OnMotionWarpFinished(FName NotifyName);

private:
	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	
	UFUNCTION()
	void PrepareIgnoredActors(TArray<AActor*>& IgnoredActors) const;
	
	UFUNCTION()
	bool HandleVaultTrace(const FVector& StartVaultLocation, const TArray<AActor*>& IgnoredActors, int iIndex, int& LastIndex, bool& bCanLand);
	
	UFUNCTION()
	bool ProcessSphereTraceHit(const FHitResult& OutHit, int Index, int& LastIndex, bool& bCanLand);
	
	UFUNCTION()
	bool ProcessVaultLanding(const FVector& SphereTraceStart, const FHitResult& OutHit, const TArray<AActor*>& IgnoredActors, bool& bCanLand);
	
	UFUNCTION()
	void DetectKickableActor(const FHitResult& OutHit, const FVector& SphereTraceStart, const TArray<AActor*>& IgnoredActors);

	UFUNCTION()
	void KickCharacterDuringVault();
	
	UFUNCTION()
	bool CheckLandingLocation(const TArray<AActor*>& IgnoredActors, bool& bCanLand);

	UFUNCTION()
	bool TryToDoStep(int LastIndex, const FVector& StartVaultLocation);

	UFUNCTION()
	bool FinalizeVault(int LastIndex, const FVector& StartVaultLocation, bool bCanLand);
	
	UFUNCTION()
	void OnMantleCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnVaultCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnShortVaultCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnStepCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRollCompleted(UAnimMontage* Montage, bool bInterrupted);
};
