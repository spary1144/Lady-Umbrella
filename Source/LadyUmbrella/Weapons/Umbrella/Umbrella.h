// 
// Umbrella.h
// 
// Main tool/weapon the player will use. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ArrowComponent.h"
#include "Components/TimelineComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/ParticleSpawnerComponent/ParticleSpawnerComponent.h"
#include "LadyUmbrella/Components/UpgradeComponent/Upgrade.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeType.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"
#include "LadyUmbrella/Interfaces/UmbrellaUpgradeInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/GadgetType.h"

#include "Umbrella.generated.h"

struct FUpgradeDatatableRow;
class UFModComponentInterface;
class UFMODEvent;
class UFMODAudioComponent;
class UTimelineComponent;
class UMeleeComponent;
class UAnimSequence;
class UBoxComponent;
class UCableComponent;
class USkeletalMeshComponent;
class UHealthComponent;
class USoundCue;
class UWidgetComponent;
class AElectricProjectile;

DECLARE_DELEGATE_FourParams(FOnCurrentBulletsChanged, bool, int32, int32 , int32);
DECLARE_DELEGATE_OneParam(FOnTotalBulletsChanged, int32);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnUmbrellaStateChanged, bool, bool , bool, bool ); /*IsAiming, IsOpen, IsOnEnemy, IsOnElementHookable*/
DECLARE_DELEGATE_ThreeParams(FOnGrappleCooldown, bool, FTimerHandle, float);
DECLARE_DELEGATE_ThreeParams(FOnElectricDartCooldown, bool, FTimerHandle, float);
DECLARE_DELEGATE_OneParam(FOnGadgetSwap, EGadgetType);
DECLARE_DELEGATE_TwoParams(FOnUmbrellaHealthChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnUmbrellaRangeAndSpread, float, float, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsCoveredByUmbrellaChanged, bool, bNewState);
DECLARE_DELEGATE_OneParam(FOnMaxPelletsPerShot, int);
DECLARE_DELEGATE_TwoParams(FOnPelletHit, bool, TSet<AGenericCharacter*>);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMultiOnCurrentBulletsChanged);
DECLARE_DELEGATE_TwoParams(FOnUnlockingGadget, EGadgetType, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShootingCadence, FTimerHandle);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCanShoot, bool);
DECLARE_DELEGATE_OneParam(FOnUmbrellaUpgradeDelegate, EUpgradeType);
DECLARE_DELEGATE_OneParam(FOnUmbrellaAmmoCurrentMax, int32);
DECLARE_DELEGATE_ThreeParams(FOnUmbrelaLoadAmmo, int32, int32, int32);
DECLARE_DELEGATE_ThreeParams(FOnUmbrellaReloadStateChanged, EUmbrellaLocation, bool, bool);
DECLARE_MULTICAST_DELEGATE(FOnBrokenUmbrella)

const uint8 HOOK_ACHIEVEMENT_COUNT = 3;
const uint8 HOOK_ACHIEVEMENT_TIMER = 15;

const uint8 SHOCK_ACHIEVEMENT_COUNT = 3;
const uint8 SHOCK_ACHIEVEMENT_TIMER = 15;

UCLASS()
class LADYUMBRELLA_API AUmbrella : public AActor, public IHealthInterface, public IUmbrellaUpgradeInterface
{
	GENERATED_BODY()
/*
 *
 *	CONSTANTS
 * 
 */
	const int32 UMBRELLA_INSIDE_MATERIAL_INDEX	 = 1;
	const float UMBRELLA_OPENING_BASE_PLAY_RATE  = 1.0f;
	const FName UMBRELLA_OPENING_MORPH_TARGET_1  = FName("umbrella_c_bs_umbrellaOpen_c_crr0");
	const FName UMBRELLA_OPENING_MORPH_TARGET_2  = FName("umbrella_c_bs_umbrellaOpen_c_crr1");
	const FName UMBRELLA_OPENING_MORPH_TARGET_3  = FName("umbrella_c_bs_umbrellaOpen_c_crr2");
	const FName UMBRELLA_OPENING_MORPH_TARGET_4  = FName("umbrellaOpen_c_crr");
	const FName UMBRELLA_HEALTH_PARAMETER_NAME   = FName("PercentageDestroyed");
	const FName UmbrellaOpenFmodParameterName	 = FName("bIsOpenUmbrella");
	const FString UmbrellaRegeneratedFmodEventName = FString("UMBRELLA_REGENERATE");
	const FString UmbrellaBrokenFmodEventName = FString("UMBRELLA_BREAK");
	
	const FString GadgetHookName				 = FString("GADGET_HOOK_EQUIP");
	const FString GadgetElectricName			 = FString("GADGET_ELECTRIC_EQUIP");
	const uint8 UmbrellaOpenFmodParameterValueOpen = 0;
	const uint8 UmbrellaOpenFmodParameterValueClosed = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodUmbrellaShieldHealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodHookImpactComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodShieldOpenedComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UFModComponentInterface* FmodGadgetsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UFModComponentInterface* FmodShootingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UFModComponentInterface* FModImpactBulletsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UFModComponentInterface* FmodFallingShellsEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UFModComponentInterface* FmodOpenCloseUmbrella;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    UParticleSpawnerComponent* ParticleSpawnerComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMeleeComponent* MeleeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCableComponent* CableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UArrowComponent* MuzzleArrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = true))
	UBoxComponent* ProtectionCollisionComponent;

	UPROPERTY(EditAnywhere)
	UUpgradeComponent* UpgradeComponent;

	UPROPERTY(EditAnywhere)
	UTimelineComponent* OpenUmbrellaShieldTimelineComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* UmbrellaSmokeNiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StartReloadingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	float StartReloadingAnimPlayRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	float ReloadAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadPumpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	float ReloadPumpAnimPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* OpenUmbrellaMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CloseUmbrellaMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	float OpenCloseUmbrellaPlayRate;
	
#pragma region PARAMETERS
private:
	/****************************/
	/*          ATTACK          */
	/****************************/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int32 AmmoCurrentMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int32 AmmoReserveMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float BulletDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float BulletRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float ElectricDartAimRange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	UCurveFloat* DistanceDamageModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float MinReloadTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float MaxReloadTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float ShootingCadenceTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float NotAimingBulletDispersion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float AimingBulletDispersion;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	float BulletPatternSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	bool bDrawDebugBulletDispersion;

	UPROPERTY(EditAnywhere, Category = "Attack", meta = (AllowPrivateAccess = true))
	FRotator MuzzleFlashRotationOffset;
	
	UPROPERTY(EditAnywhere, Category="Shotgun|Spread")
	float SpreadAngleX;
	
	UPROPERTY(EditAnywhere, Category="Shotgun|Spread")
	float SpreadAngleZ;

	UPROPERTY(EditAnywhere, Category="Shotgun|CylinderTrace")
	float PelletCylinderRadius;

	UPROPERTY(EditAnywhere, Category="Shotgun|CylinderTrace")
	float PelletDamageMultiplier;

	UPROPERTY(EditAnywhere, Category="Shotgun|CylinderTrace")
	float SpawnForwardMargin; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Bounciness;
	
	/****************************/
    /*          DEFEND          */
    /****************************/
	
	/**
	 * The minimum percentage of its Max Health the umbrella must have to become "not broken" and be allowed to open. 
	 */
	UPROPERTY(EditAnywhere, Category="Defend", meta=(ClampMin=0.f, ClampMax=1.f, UIMin=0.f, UIMax=1.f))
    float MinHealthPercentageToAllowShieldOpen; 

	
	/****************************/
	/*         GADGETS          */
	/****************************/

	UPROPERTY(EditAnywhere, Category = "Gadget")
	TSubclassOf<AElectricProjectile> ElectricProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	float HookRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadget",meta = (AllowPrivateAccess = true))
	float ElectricCooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gadget",meta = (AllowPrivateAccess = true))
	float GrappleCooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grapple",meta = (AllowPrivateAccess = true))
	float HookSphereRadius;

	/****************************/
	/*        ANIMATIONS        */
	/****************************/

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UCurveFloat* UmbrellaOpeningCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta=(UIMin = 0, ClampMin = 0))
	float ClosingUmbrellaAnimationPlayRateMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta=(UIMin = 0, UIMax = 1, ClampMin=0, ClampMax=1))
	float MorphTarget1To2TransitionValue;

	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta=(UIMin = 0, UIMax = 1, ClampMin=0, ClampMax=1))
	float MorphTarget2To3TransitionValue;

	UPROPERTY(EditDefaultsOnly, Category = "Animations", meta=(UIMin = 0, UIMax = 1, ClampMin=0, ClampMax=1))
	float MorphTarget3To4TransitionValue;
	
	
	/****************************/
	/*         GRAPHICS         */
	/****************************/
	
	/**
	 * The curve that gets evaluated when calculating how damaged the umbrella should look based on its health.
	 * The curve goes from 0 to 1 in the X & Y Axis.
	 * X = 0 means that the Umbrella is fully destroyed and has no health left.
	 * X = 1 means that the Umbrella is not damaged and has full health.
	 * Y = 0 means that the Umbrella is looks fully destroyed through the Umbrella VFX Material.
	 * Y = 1 means that the Umbrella is looks full health through the Umbrella VFX Material.
	 */
	UPROPERTY(EditAnywhere, Category = "Graphics")
    UCurveFloat* UmbrellaShieldHealthVisualsCurve;

	UPROPERTY(EditAnywhere, Category = "Graphics")
	UParticleSystem* ImpactParticleSystem;
	
	UPROPERTY(EditAnywhere, Category = "Graphics")
	UParticleSystem* BulletImpactParticleSystem;

	UPROPERTY(EditAnywhere, Category = "Graphics")
	float DestroyedParticlesOffset; 

	UPROPERTY(EditAnywhere, Category = "Graphics")
	UParticleSystem* DestroyedParticleSystem;
	
	
	/****************************/
	/*          SOUND           */
	/****************************/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	USoundCue* SCShoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX", meta = (AllowPrivateAccess = "true"))
	float VolumeMultiplier;
	
	
	/****************************/
	/*            UI            */
	/****************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = true))
	float WidgetTraceRadius;
	
#pragma endregion 

#pragma region VARIABLES
private:
	/****************************/
	/*       GENERAL STATE      */
	/****************************/
	
	/**
	 * Reference to the Player Pawn that holds the umbrella.
	 */
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;
	
	/**
	 * Reference to the player's camera component.
	 */
	UPROPERTY()
	UCameraComponent* CameraComponent;

	UPROPERTY()
	bool bInfiniteAmmo;

	
	/****************************/
	/*          ATTACK          */
	/****************************/

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReloadTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int32 AmmoCurrent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int32 AmmoReserve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	int32 MaxNrBulletPerShot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	EGadgetType GadgetType;
	
	TMap<EGadgetType, bool> GadgetTypeStructMap;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> SpawnPattern;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bOnTarget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bOnTargetGadget;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsShooting;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsPumping;
	
	UPROPERTY()
	bool bHasHitGenericCharacter;

	UPROPERTY()
	TSet<AGenericCharacter*> EnemiesHitByPellets;

	UPROPERTY()
	FTimerHandle ShootingTimer;

	uint8 HookAchievementCount;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	FTimerHandle HookAchievementTimer;

	uint8 ShockAchievementCount;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ShockAchievementTimer;
	
	/****************************/
	/*          DEFEND          */
	/****************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Defend", meta = (AllowPrivateAccess = true))
	bool bOpen;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Defend", meta = (AllowPrivateAccess = true))
	bool bCantOpenBrokenUmbrellaDueToLowHealth;
	
	/****************************/
	/*         GRAPHICS         */
	/****************************/

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicInstanceInsideUmbrellaMaterial;
	
	/****************************/
	/*         GADGETS          */
	/****************************/

	UPROPERTY()
	FTimerHandle GrappleCooldownTimerHandle;
	
	UPROPERTY()
	FTimerHandle ElectricCooldownTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	bool bCanShootGrapple;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	bool bIsGrapplingCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gadget", meta = (AllowPrivateAccess = true))
	bool bCanShootElectric;

	FTimerHandle ShellFallingTimerHandle;

#pragma endregion

#pragma region DELEGATES
public:
	
	FOnCurrentBulletsChanged OnCurrentBulletsChanged;
	FOnTotalBulletsChanged OnTotalBulletsChanged;
	FOnUmbrellaStateChanged OnUmbrellaStateChanged;
	FOnIsCoveredByUmbrellaChanged OnIsCoveredByUmbrellaChanged;
	FOnGrappleCooldown OnGrappleCooldown;
	FOnElectricDartCooldown OnElectricDartCooldown;
	FOnGadgetSwap OnGadgetSwap;
	FOnUmbrellaHealthChanged OnUmbrellaHealthChanged;
	FOnUmbrellaRangeAndSpread OnUmbrellaRangeAndSpread;
	FOnMaxPelletsPerShot OnMaxPelletsPerShot;
	FOnPelletHit OnPelletHit;
	FMultiOnCurrentBulletsChanged OnMultiCurrentBulletsChanged;
	FOnUnlockingGadget OnUnlockingGadget;
	FOnShootingCadence OnShootingCadence;
	FOnCanShoot OnCanShoot;
	FOnUmbrellaUpgradeDelegate OnUmbrellaUpgradeDelegate;
	FOnUmbrellaAmmoCurrentMax OnUmbrellaAmmoCurrentMax;
	FOnUmbrelaLoadAmmo OnUmbrellaLoadAmmo;
	FOnUmbrellaReloadStateChanged OnUmbrellaReloadStateChanged;
	FOnBrokenUmbrella OnBrokenUmbrella;
	
#pragma endregion 

public:
	
	AUmbrella();
	
	UFUNCTION() void ApplyUpgrade(const EUpgradeType Type);
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

private:
	
	UFUNCTION()
	void RecalculateBulletSpreadPositions();

	UFUNCTION()
	FORCEINLINE void ResetElectricCooldown() { bCanShootElectric = true; };

	UFUNCTION()
	FORCEINLINE void ResetGrappleCooldown() { bCanShootGrapple = true; };
	
	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnReloadMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	FORCEINLINE void ResetShooting() { bIsShooting = false; };

	UFUNCTION()
	void StartPump();
	
	UFUNCTION()
	bool ShootingUmbrellaGrapple();

	UFUNCTION()
	TArray<FHitResult> PerformSphereTrace(float TraceRadius, float TraceLength,EGadgetType GadgetTypeEnum);

	UFUNCTION()
	void UmbrellaHealthChangedUpdate(int32 NewHealth, bool bLostHealth);

	UFUNCTION()
	void ResetHasHitGenericCharacter();

/*
	UFUNCTION()
	void TryAddEvent(UFMODEvent* EventToAdd, UFModComponentInterface* ComponentToAdd);
*/
	
	UFUNCTION()
	void NotifyUmbrellaIsNoLongerBroken() const;
	
	void PlayFallingShellSound() const;

	void PlayOpenCloseShield(const uint8 Value) const;
	
	float GetUpgradeMultiplier(const FUpgrade* Upgrade, const FUpgradeDatatableRow* Row) const;

	UFUNCTION()
	void OnReloadPumpMontageEnd(UAnimMontage* Montage, bool bInterrupted);

public:
	
	UFUNCTION()
	FORCEINLINE void SetVisibility(bool Visibility) const { SkeletalMeshComponent->SetVisibility(Visibility); };

	/**
	 * Tries to open the umbrella shield. If the umbrella is already opened or if it has no life left, it returns false.
	 * @return Whether the umbrella was successfully opened.
	 */
	UFUNCTION()
	bool TryToOpenShield();

	/**
 	* Tries to close the umbrella shield. If the umbrella was not open, it returns false.
 	* @return Whether the umbrella was successfully closed.
 	*/
	UFUNCTION()
	bool TryToCloseShield();
	
	/**
	 * Performs the logic when the player tries to shoot. If no bullets are stored, this will trigger an automatic reload.
	 * @return True if the umbrella managed to shoot bullets.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool TryToShoot();

	UFUNCTION()
	bool IsPointInCylinder(const FVector& Start, const FVector& End, float Radius, const FVector& Point);

	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool GadgetShoot();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	bool OnElectricShoot();
	
	UFUNCTION(BlueprintCallable, Category = "Attacks")
	void CycleBulletType();

	void SpawnGadgetCycleParticle() const;

	UFUNCTION(BlueprintCallable, Category = "Attacks")
	void SetBulletType(EGadgetType TargetGadget);

	UFUNCTION()
	void ResetGrapple();

	UFUNCTION()
	void SetCableEndLocation(const FVector& EndPoint) const;

	UFUNCTION()
	void StartReloading();
	
	UFUNCTION()
	void ReloadOneShell();
	
	UFUNCTION()
	void AutoReload();

	UFUNCTION()
	void InterruptReload();

	/**
	 * Sets the Morph Target values inside the Umbrella's Skeletal Mesh for it to appear as opened as the provided value.
	 * @param Value A value inside the 0 to 1 range, 0 representing a closed Umbrella, 1 representing a fully open umbrella.
	 */
	UFUNCTION(BlueprintCallable)
	void SetPercentageOpenedShield(const float Value);

	UFUNCTION()
	void UpdateVisualAmmoOnLoad() const;
	
	UFUNCTION()
	void UpdateVisualGadgetsOnLoad() const;

	FORCEINLINE bool IsAmmoReserveAtMaxValue() const { return AmmoReserve >= AmmoReserveMax; }
	FORCEINLINE bool IsAiming() const { return IsValid(PlayerCharacter) ? PlayerCharacter->IsAiming() : false; }
	FORCEINLINE bool IsOpen() const { return bOpen; }
	FORCEINLINE bool IsOnTarget() const {return bOnTarget; }
	FORCEINLINE bool IsOnTargetGrapple() const {return bOnTargetGadget; }
	FORCEINLINE bool IsInfiniteAmmo() const {return bInfiniteAmmo; }
	FORCEINLINE float GetBounciness() const { return Bounciness; }

	UFUNCTION()
	static bool IsOnTargetHit(const TArray<FHitResult>& HitResults,EGadgetType IsHookGadget);
	
	void UnlockGadgets(const EGadgetType GadgetToUnlock);

	virtual void SubtractHealth(const float HealthToSubtract, EWeaponType DamageType) override;

	virtual void SetHealth(const float NewHealth) override;
	
	virtual const float GetHealth() const override;
	
	float GetMaxHealth() const;

	// Setters
	void SetAmmoCurrent(const int32 Amount);
	void SetAmmoReserve(const int32 Amount);
	void SetAmmoCurrentMax(const int32 Amount);
	FORCEINLINE void SetInfiniteAmmo(const bool bValue) { bInfiniteAmmo = bValue; }
	FORCEINLINE void SetHasHitGenericCharacter(const bool bValue) { bHasHitGenericCharacter = bValue; }
	FORCEINLINE void SetGadgetsUnlocked (const TMap<EGadgetType, bool>& GadgetsState) { GadgetTypeStructMap = GadgetsState ; }
	// Getters
	FORCEINLINE int32 GetAmmoCurrent() const { return AmmoCurrent; }
	FORCEINLINE int32 GetAmmoReserve() const { return AmmoReserve; }
	FORCEINLINE int32 GetAmmoCurrentMax() const { return AmmoCurrentMax; }
	FORCEINLINE int32 GetAmmoReserveMax() const { return AmmoReserveMax; }
	FORCEINLINE int32 GetMaxNrBulletPerShot() const { return MaxNrBulletPerShot; }
	FORCEINLINE UUpgradeComponent* GetUpgradeComponent() const { return UpgradeComponent; }
	FORCEINLINE UCableComponent* GetCableComponent() const { return CableComponent; }
	FORCEINLINE UMeleeComponent* GetMeleeComponent() const { return MeleeComponent; }
	FORCEINLINE UParticleSystem* GetBulletImpactParticle() const { return BulletImpactParticleSystem; }
	FORCEINLINE float GetShootingCadence() const { return ShootingCadenceTime; }
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }
	FORCEINLINE float GetBulletRange() const { return BulletRange; }
	FORCEINLINE float GetSpreadAngleX() const { return SpreadAngleX; }
	FORCEINLINE float GetSpreadAngleZ() const { return SpreadAngleZ; }
	FORCEINLINE virtual UUpgradeComponent* GetUmbrellaUpgradeComponent() override {	return UpgradeComponent; }
	FORCEINLINE const TMap<EGadgetType, bool>& GetGadgetTypeStructMap() const noexcept { return GadgetTypeStructMap; }
	FORCEINLINE EGadgetType GetCurrentGadgetType() const{ return GadgetType; }
	FORCEINLINE UFModComponentInterface* GetFmodGadgetsComponent() const {return FmodGadgetsComponent; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grapple", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* GrappleStartTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grapple", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* GrappleStartCurve;

	UPROPERTY()
	FVector GrappleEndPoint;

	UPROPERTY()
	AActor* GrappleHitActor;

	UPROPERTY()
	UPrimitiveComponent* GrappleHitComponent;
	
	UFUNCTION()
	void GrappleStartTimelineProgress(float Value);

	UFUNCTION()
	void GrappleStartTimelineFinish();

private:
	
	UFUNCTION()
	void LoadUmbrellaFromDataTable();
	/**
	 * Element to control Rows from DataTablePtr Type
	 * RowType="CH_Francesca", 
	 */
	UPROPERTY(EditAnywhere, Category = "Data Driven", DisplayName = "Row Type", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

	UFUNCTION()
	void HookEnemy();
	
	UFUNCTION()
	void ShockEnemy();

	UFUNCTION()
	void HookAchievementCheck() const;
	
	UFUNCTION()
	void ShockAchievementCheck() const;
};
