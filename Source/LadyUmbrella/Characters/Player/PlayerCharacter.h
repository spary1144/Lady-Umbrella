// 
// PlayerCharacter.h
// 
// Main character, which is the character the player will control and posses throughout the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineInGameDTO.h"
#include "LadyUmbrella/Interfaces/GetWeaponInterface.h"
#include "LadyUmbrella/Interfaces/PlaySoundInterface.h"
#include "LadyUmbrella/Interfaces/StepsSoundEvent.h"
#include "LadyUmbrella/Interfaces/UnlockGadgetsInterface.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/SaveSystem/FSaveStructPlayer.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"

#include "PlayerCharacter.generated.h"

enum class EUmbrellaLocation : uint8;
class UDeathScreenComponent;
struct FVoiceLineInGameDTO;
class UPlayerStateComponent;
class AUmbrella;
class IInteractInterface;
class UAimAssistComponent;
class UAnimMontage;
class UCameraComponent;
class UCineCameraComponent;
class UCustomCameraSystemComponent;
class UFModAudioComponent;
class UFModComponentInterface;
class UFreeRoamCoverComponent;
class UHitReactionComponent;
class UInputAction;
class UInputMappingContext;
class UInteractiveMovementComponent;
class UMotionWarpingComponent;
class UNumericProgressBar;
class USpringArmComponent;
class UTimelineComponent;
enum class EPlayerState  : uint8; 
enum class EStatusEffect : uint8;
struct FInputActionValue;
struct FVoiceLineDTO;

DECLARE_DELEGATE(FOnPlayerShoot);

const float FALLING_ACHIEVE  = 400.0f;
const float FALLING_DISTANCE = 10000.0f;

UCLASS(config=Game)
class APlayerCharacter : public AGenericCharacter, public ISaveInterface, public IGetWeaponInterface, public IUnlockGadgetsInterface, public IPlaySoundInterface
{
	GENERATED_BODY()

	// +-------------------------------------------------+
	// |                                                 |
	// |                    VARIABLES                    |
	// |                                                 |
	// +-------------------------------------------------+
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* IdleToAimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AimToIdleMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	bool bWantsToJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinematic", meta = (AllowPrivateAccess = "true"))
	bool bIsInSecurityCinematic;
	
	// TODO: Why isn't this a simple OnOverlap?
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsInCombatVolume;

	UPROPERTY()
	bool bIsAimingButtonPressed;
	
	UPROPERTY()
	FVector PreviousTickLocation;
	
	UPROPERTY()
	float DelayToPutUmbrellaOnBackAfterArenaEnded;
	
	// TODO: Why are FleeRadius and CloseRadius in PlayerCharacter?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IA", meta = (AllowPrivateAccess = "true"))
	float FleeRadius;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IA", meta = (AllowPrivateAccess = "true"))
	float CloseRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RecoilAnimSpeedOffset;

	UPROPERTY()
	int32 CurrentPiecesForUpgrades;

	UPROPERTY()
	FTransform InitialMeshRelativeTransform;

	UPROPERTY()
	bool bPlayerInCombat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UPlayerStateComponent> PlayerStateComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCustomCameraSystemComponent> CameraSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAimAssistComponent> AimAssistComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFreeRoamCoverComponent> FreeRoamCoverComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UHitReactionComponent> HitReactionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodComponentVoiceLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodComponentSteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodThirdCinematic; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodHealthSnapshot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> ReloadFmodComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> AnimationsFmodComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> InGameVoicesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDeathScreenComponent> DeathScreenComponent;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	FTimerHandle MovementTimerHandleWhileGrappling;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Interactive Movement", meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimerHandle_GrapplingDelay;

	UPROPERTY()
	FTimerHandle TimerHandleHealthSnapshot;

	UPROPERTY()
	FTimerHandle ResetUmbrellaPositionTimerHandle;

	UPROPERTY()
	FTimerHandle CombatStateTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Umbrella", meta = (AllowPrivateAccess = "true"))
	float TimeToResetUmbrellaPosition;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General", meta = (AllowPrivateAccess = "true"))
	FSaveStructPlayer PlayerSaveStruct;
	
	UPROPERTY(EditAnywhere, Category = "Data Driven", DisplayName = "Row Type", meta = (RowType="BP_Character", AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

	const float HealthSnapshotThreshold		    = 50.f;
	const float HealthSnapshotCallingRate	    = 0.5f;
	const float GrappleMontagePlayRate          = 2.0f;
	const float IdleToAimMontagePlayRate		= 1.0f;
	const FString FmodComponentStepsEventName   = "FrancescaFootSteps";
	const FName FmodComponentStepsParameterName = "SurfaceStepped";
	const int32 VelocityMultiplier              = 2500;
	const FString PLAYER_DEFAULT_NAME           = FString(TEXT("Francesca"));
	const float MAX_TIME_WITH_UMBRELLA_ON_BACK_IN_COMBAT = 1.f;
	
	const float VELOCITY_IN_GAME_CINEMATIC = 250.f;
	float SaveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Umbrella", meta = (AllowPrivateAccess = "true"))
	EUmbrellaLocation CurrentUmbrellaLocation;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	
	UFUNCTION()
	void OnUmbrellaSheathEnded(UAnimMontage* Montage, bool bInterrupted);


	UFUNCTION()
	void ChangePlayerInCombatState(bool NewCombatState);

public:
	
	// NOTE: Delegates CANNOT be private because otherwise they cant broadcast outwards.
	FOnPlayerShoot OnPlayerShot;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	APlayerCharacter();
	void SwapGameplayVoices(const FString& NewChapter) const;

	void InitSounds() const;
	//virtual void PostInitProperties() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	void PlayFrancescaClimbing() const;

	virtual void Jump() override;
	
	UFUNCTION() void OnMeleeAttackListener();
	UFUNCTION() void OnMeleeCompleted(UAnimMontage* Montage, bool bInterrupted);
	virtual void OnVaultListener() override;
	virtual void OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned) override;

	// TODO: Review these functions.
	UFUNCTION() void Move(const FInputActionValue& Value);
	UFUNCTION() void StopMovingPlayer(const FInputActionValue& Value);
	UFUNCTION() void Look(const FInputActionValue& Value);
	UFUNCTION() void StopLook(const FInputActionValue& Value);
	UFUNCTION() void StartedHoldingAimButton();
	UFUNCTION() void ReleasedAimButton();
	UFUNCTION(BlueprintCallable) void StartAiming();
	UFUNCTION(BlueprintCallable) void ResetAiming();
	UFUNCTION() void Shoot() const;
	UFUNCTION() void ShootingUmbrella();
	UFUNCTION() void Reload();

	UFUNCTION() void GadgetShoot();
	UFUNCTION() void CycleUmbrellaBullets();
	bool CanSwapGadget() const;
	UFUNCTION() void SwapToHook();
	UFUNCTION() void SwapToElectric();
	UFUNCTION() void MovePlayerViaGrapple(const FVector& EndPosition);
	UFUNCTION() void Grappling(const FVector& EndPosition);
	UFUNCTION() void PlayGrappleAnim() const;

	//void SaveUmbrellaData() const;
	//void LoadUmbrellaData() const;
	virtual void SaveGame() override;
	virtual void LoadGame() override;
	virtual void SaveDataCheckPoint() override;
	virtual void LoadDataCheckPoint() override;
	UFUNCTION() void LoadPlayerFromDataTable();
	UFUNCTION(BlueprintCallable) void ToggleCover();
	UFUNCTION() void ResetWalkSpeedAfterFreeCover();

	UFUNCTION() bool LandmarkInteractUnderComms(const TArray<AActor*>& InteractingActors) const;
	UFUNCTION(BlueprintCallable) void InteractingWithOverlappingElements(const FInputActionValue& Value);

	UFUNCTION() void ToggleShieldInput();
	void ShoulderSwap();
	UFUNCTION() void HoldShieldInput();
	UFUNCTION() void StopShieldInput();
	void CheckForFloor();
	UFUNCTION() bool TryToOpenShield();
	UFUNCTION() bool TryToCloseShield();
	UFUNCTION() void OnUmbrellaOpenStateChange(bool bIsOpen);
	UFUNCTION() void AttachUmbrellaToSocket(const FName& NewPosition);
	void HideHUD(EUmbrellaLocation UmbrellaPosition);

	void ActivateInComsMode(bool IsCommunicationCinematic);
	void DeActivateInComsMode(const bool IsCommunicationCinematic);
	
	void ChangeHandling(float Value);
	void ChangeShieldMotion(float Value);
	UFUNCTION(BlueprintCallable)
	void ChangeUmbrellaLocation(const EUmbrellaLocation UmbrellaPosition, const bool bImmediateChange, const bool bPlayAnim);

	void PlayVoiceLine(const EVoiceLineState& VoiceLineState,const bool HasPriority = false,const bool queue = true);

	// IUnlockGadgetsInterface
	virtual void UnlockGadgets(const EGadgetType GadgetToUnlock) override;	
	// IPlaySoundInterface
	virtual void PlayStepsSounds(const uint8 StepSurface) override;
	virtual void PlayNotifiedActionSound(const FString& EventToPlay) override;
	virtual void PlayUmbrellaGadgetSound(const FString& EventToPlay) override;
	virtual void PlayNotifiedActionSoundWithParameter(const FString& EventToPlay, const float NewParameter) override;
	virtual void PlayNotifiedVoiceSound (const FString& EventToPlay) override;

	UFUNCTION()
	void ResetSnapshot();
	
	// TODO: Move to HealthComponent.
	
	virtual void SubtractHealth(const float HealthToSubtract, const EWeaponType DamageType, const FVector& LastDamageSourceLocation) override;
	UFUNCTION(BlueprintCallable, Category = "DeathManagement") void KillPlayer();
	virtual void SetDead() override;
	UFUNCTION() void Respawn();
	void CheckpointCameraReset();
	UFUNCTION() void ResetInputAfterRespawnFadeFinishes();
	
	
	// TODO: Move to StateComponent.
	
	UFUNCTION(BlueprintCallable) EPlayerState GetPlayerGeneralState() const;
	UFUNCTION(BlueprintCallable) bool SetPlayerGeneralState(EPlayerState NewPlayerState);
	UFUNCTION(BlueprintCallable) bool CanChangeToPlayerState(EPlayerState NewPlayerState);
	UFUNCTION(BlueprintCallable) void ResetPlayerStateToFreeMovementAndCheckAim();
	UFUNCTION() bool PlayMotionWarpingMontage(UAnimMontage* Montage, const float PlayRate, FMotionWarpingTarget MotionWarpingTarget) const;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                GETTERS & SETTERS                |
	// |                                                 |
	// +-------------------------------------------------+
	FORCEINLINE virtual AUmbrella* GetWeapon() const override { return FGlobalPointers::Umbrella; }

	FORCEINLINE bool IsInSecurityCinematic() const { return bIsInSecurityCinematic; }
	FORCEINLINE void SetInSecurityCinematic(const bool Value) { bIsInSecurityCinematic = Value; }

	UFUNCTION() FORCEINLINE UCustomCameraSystemComponent* GetCameraSystemComponent() const { return CameraSystemComponent; }
	FORCEINLINE TObjectPtr<UFModComponentInterface> GetFmodComponentStepsInterface() const { return FmodComponentSteps; }
	FORCEINLINE TObjectPtr<UFModComponentInterface> GetFmodComponentVoiceLinesInterface() const { return FmodComponentVoiceLines; }
	FORCEINLINE FDataTableRowHandle GetDataTableRowHandle() const { return DataTableRowHandler; }
	UFUNCTION() FORCEINLINE int32 GetCurrentPiecesForUpgrades() const { return CurrentPiecesForUpgrades; }
	FORCEINLINE TObjectPtr<UPlayerStateComponent> GetPlayerStateComponent() const { return PlayerStateComponent; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE bool IsAimButtonHeld() const { return bIsAimingButtonPressed; }
	FORCEINLINE bool IsHoldingAimButton() const { return bIsAimingButtonPressed; }
	FORCEINLINE bool IsInCombatVolume() const { return bIsInCombatVolume; }
	FORCEINLINE bool IsPlayerInCombat() const { return bPlayerInCombat; }

	FORCEINLINE EUmbrellaLocation GetCurrentUmbrellaLocation() const { return CurrentUmbrellaLocation; }
	
	UFUNCTION() FORCEINLINE void SetCurrentPiecesForUpgrades(const int32 Value) { CurrentPiecesForUpgrades = Value; }
	FORCEINLINE void SetIsInCombatVolume(const bool bValue) { bIsInCombatVolume = bValue; }
	FORCEINLINE void SetPlayerInCombat(const bool bValue) { bPlayerInCombat = bValue; }
	FORCEINLINE void SetCurrentUmbrellaLocation(const EUmbrellaLocation Location) { CurrentUmbrellaLocation = Location; }
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                DEVELOPMENT STUFF                |
	// |                                                 |
	// +-------------------------------------------------+
	// TODO: All development functions & variables need to be removed when releasing the final production build.
	UPROPERTY() bool bGodmode;
	UPROPERTY() bool bNoClip;
	
	FORCEINLINE bool IsGodmode() const { return bGodmode; }
	FORCEINLINE bool IsNoClip() const { return bNoClip; }
	
	FORCEINLINE void SetGodmode(const bool bValue) { bGodmode = bValue; }
	FORCEINLINE void SetNoClip(const bool bValue) { bNoClip = bValue; }
};
