// 
// GenericWeapon.h
// 
// Class from which all weapons inherit from. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Weapons/Umbrella/BulletType.h"
#include "GenericWeapon.generated.h"

class UFModComponentInterface;
class AElectricProjectile;
class AUmbrella;
struct FWeaponDataDriven;
class UCapsuleComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USkeletalMeshComponent;

UCLASS()
class LADYUMBRELLA_API AGenericWeapon : public AActor
{
	GENERATED_BODY()

	#pragma region CONSTANTS
	
	const FName BULLET_START_POSITION_VARIABLE_NAME = FName(TEXT("StartPosition"));
	const FName BULLET_END_POSITION_VARIABLE_NAME = FName(TEXT("EndPosition"));
	const FName BULLET_PROJECTILE_SPEED_VARIABLE_NAME = FName(TEXT("ProjectileSpeed"));
	const FName LIFETIME_AFTER_BULLET_DEACTIVATION_NIAGARA_VARIABLE_NAME = FName(TEXT("LifetimeAfterBulletHasDeactivated"));
	const FName RIBBON_ID_NIAGARA_VARIABLE_NAME = FName(TEXT("RibbonID"));
	const FName CHARGING_ELECTRIC_DART_SPHERE_LENGTH = FName(TEXT("ChargingSphereTotalLength"));
	const FName CHARGING_ELECTRIC_DART_TOTAL_LENGTH = FName(TEXT("ChargingElectricDartTotalLength"));
	const FName ELECTRIC_BULLET_START_POSITION_VARIABLE_NAME = FName(TEXT("InitialPosition"));
	const FName ELECTRIC_BULLET_END_POSITION_VARIABLE_NAME = FName(TEXT("FinalPosition"));
	const FName ELECTRIC_BULLET_PROJECTILE_SPEED_VARIABLE_NAME = FName(TEXT("ProjectileSpeed"));
	const FName MUZZLE_SOCKET_NAME = FName(TEXT("MuzzleSocket"));
	const float MAX_BULLET_PARTICLE_DISTANCE_TRAVELLED = 10000;
	const FName PARAMETER_NAME_DART_CHARGE = "ELECTRIC_DART_CHARGE";
	const FName PARAMETER_NAME_DART_SHOOT = "ELECTRIC_DART_SHOOT";
	
	#pragma endregion

	/****************************/
	/*         GAMEPLAY         */
	/****************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int32 AmmoCurrent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int32 AmmoCurrentMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int32 AmmoReserve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int32 AmmoReserveMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	int32 MinAmmoToStartReloading;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	bool bReloading;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float WeaponCadence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float MaxWeaponCadence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float MinWeaponCadence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|WeaponDamage", meta = (AllowPrivateAccess = true))
	float WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|WeaponDamage", meta = (AllowPrivateAccess = true))
	float MaxWeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|WeaponDamage", meta = (AllowPrivateAccess = true))
	float MinWeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|WeaponDamage", meta = (AllowPrivateAccess = true))
	float CurveMiddlePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|WeaponDamage", meta = (AllowPrivateAccess = true))
	float DamageCurveLeftSlope;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|WeaponDamage", meta = (AllowPrivateAccess = true))
	float DamageCurveRightSlope;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = true))
	float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = true))
	EBulletType WeaponBulletType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric Dart", meta = (AllowPrivateAccess = true))
	TSubclassOf<AElectricProjectile> ElectricProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric Dart", meta = (AllowPrivateAccess = true))
	float PartialChargingTimeElectricBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric Dart", meta = (AllowPrivateAccess = true))
	float ChargingTimeElectricBullet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Electric Dart", meta = (AllowPrivateAccess = true))
	bool bIsElectricDartActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Electric Dart", meta = (AllowPrivateAccess = true))
	bool bIsElectricDartShooted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LifeSpan", meta = (AllowPrivateAccess = true))
	float LifeSpan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LifeSpan", meta = (AllowPrivateAccess = true))
	float MassKg;
	
	/****************************/
	/*         GRAPHICS         */
	/****************************/

	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	UParticleSystem* ElectricEffect;

	UPROPERTY(EditAnywhere, Category = "Graphics|Bullets")
	float ProjectileParticleSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Graphics|Bullets")
	float BulletReboundMaxDifference;
	
	UPROPERTY(EditAnywhere, Category = "Graphics|Bullets")
	float TrailLifetimeAfterBulletIsDeactivated;

	UPROPERTY(EditAnywhere, Category = "Graphics|Bullets")
	bool bDoBulletsReboundAfterHittingUmbrella;
	
	UPROPERTY(EditAnywhere, Category = "Graphics|Bullets")
	float ReboundParticleRange;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* Capsule;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* MuzzleSmokeNiagaraComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ProjectileEffectsNiagaraComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ElectricChargeNiagaraComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* MuzzleFlashNiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Graphics|Electric Dart")
	FVector ElectricChargeDisplacementFromMuzzlePosition;

	UPROPERTY(EditDefaultsOnly, Category = "Graphics|MuzzleFlash")
	FRotator MuzzleRotationOffset;
	
	/****************************/
	/*          SOUNDS          */
	/****************************/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAccess = true))
	USoundBase* FireSoundBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAccess = true))
	UFModComponentInterface* FireSoundFModComponent; 

	const FString FmodEventParameterName = "ShootingVariant"; 

	bool IsShooting; 
	
	UPROPERTY()
	AUmbrella* Umbrella;
	
	UPROPERTY()
	int32 RibbonID;
	
public:
	
	AGenericWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool Reload(const bool ConsumeAmmo);

	FORCEINLINE bool IsElectricBulletActivated() const { return bIsElectricDartActivated; }
	FORCEINLINE void SetElectricBulletActivated(bool Activated) { bIsElectricDartActivated = Activated; }
	FORCEINLINE bool IsElectricBulletShooted() const { return bIsElectricDartShooted; }
	FORCEINLINE void SetElectricBulletShooted(bool Shooted) { bIsElectricDartShooted = Shooted; }

	FORCEINLINE bool HasBullets() const { return AmmoCurrent > 0; }
	FORCEINLINE int32 GetAmmoCurrent() const { return AmmoCurrent; }
	FORCEINLINE int32 GetAmmoCurrentMax() const { return AmmoCurrentMax; }
	FORCEINLINE int32 GetAmmoReserve() const { return AmmoReserve; }
	FORCEINLINE int32 GetAmmoReserveMax() const { return AmmoReserveMax; }
	FORCEINLINE UCapsuleComponent* GetCapsule() const { return Capsule; }
	float GetRandomWeaponCadence();
	FORCEINLINE float GetWeaponCadence() const { return WeaponCadence; }
	FORCEINLINE float GetMinWeaponCadence() const { return MinWeaponCadence; }
	FORCEINLINE float GetMaxWeaponCadence() const { return MaxWeaponCadence; }
	FORCEINLINE float GetWeaponDamage() const { return WeaponDamage; }
	FORCEINLINE float GetCurveMiddlePoint() const { return CurveMiddlePoint; }
	FORCEINLINE float GetDamageCurveLeftSlope() const { return DamageCurveLeftSlope; }
	FORCEINLINE float GetDamageCurveRightSlope() const { return DamageCurveRightSlope; }
	FORCEINLINE float GetMaxWeaponDamage() const { return MaxWeaponDamage; }
	FORCEINLINE float GetMinWeaponDamage() const { return MinWeaponDamage; }
    float CalculateWeaponDamage(float Distance);
	FORCEINLINE float GetWeaponRange() const { return WeaponRange; }
	FORCEINLINE float GetMinAmmoToStartReloading() const { return MinAmmoToStartReloading; }
	FORCEINLINE EBulletType GetWeaponBulletType() const { return WeaponBulletType; }
	FORCEINLINE float GetElectricBulletChargeTime() const { return ChargingTimeElectricBullet; }
	FORCEINLINE TWeakObjectPtr<USkeletalMeshComponent> GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }
	FORCEINLINE FVector GetMuzzleSocketLocation() const { return IsValid(SkeletalMeshComponent) ? SkeletalMeshComponent->GetSocketLocation(MUZZLE_SOCKET_NAME) : FVector::Zero(); }
	FORCEINLINE FRotator GetMuzzleSocketRotation() const { return IsValid(SkeletalMeshComponent) ? SkeletalMeshComponent->GetSocketRotation(MUZZLE_SOCKET_NAME) : FRotator::ZeroRotator; }

	FORCEINLINE void SetAmmoCurrent(const int32 Value) { AmmoCurrent = Value; }
	FORCEINLINE void SetAmmoCurrentMax(const int32 Value) { AmmoCurrentMax = Value; }
	FORCEINLINE void SetAmmoReserve(const int32 Value) { AmmoReserve = Value; }
	FORCEINLINE void SetAmmoReserveMax(const int32 Value) { AmmoReserveMax = Value; }
	FORCEINLINE void SetWeaponCadence(const float Value) { WeaponCadence = Value; }
	FORCEINLINE void SetWeaponDamage(const float Value) { WeaponDamage = Value; }
	FORCEINLINE void SetCurveMiddlePoint(const float Value) { CurveMiddlePoint = Value; }
	FORCEINLINE void SetDamageCurveLeftSlope(const float Value) { DamageCurveLeftSlope = Value; }
	FORCEINLINE void SetDamageCurveRightSlope(const float Value) { DamageCurveRightSlope = Value; }
	FORCEINLINE void SetElectricBulletChargeTime(const float Value) { ChargingTimeElectricBullet = Value; }
	FORCEINLINE void SetMaxWeaponDamage(const float Value) { MaxWeaponDamage = Value; }
	FORCEINLINE void SetMinWeaponDamage(const float Value) { MinWeaponDamage = Value; }
	FORCEINLINE void SetWeaponRange(const float Value) { WeaponRange = Value; }
	FORCEINLINE void SetWeaponBulletType(const EBulletType Value) { WeaponBulletType = Value;}

	FORCEINLINE void SetFmodComponent(UFModComponentInterface* NewFmodComponent) { FireSoundFModComponent = NewFmodComponent; }
	FORCEINLINE UFModComponentInterface* GetFmodComponentInterface() const { return FireSoundFModComponent;}
	//FORCEINLINE void SetSkeletalMeshComponent(const TWeakObjectPtr<USkeletalMeshComponent> Value) { SkeletalMeshComponent = Value; }

	void SimulatePhysicsBeforeDestroy();

	UFUNCTION()
	void SpawnElectricCharge();

	UFUNCTION()
	void StopElectricCharge();

	/**
	 * Spawns a Bullet Particle that will travel from Start Location to End Location.
	 * @param StartLocation 
	 * @param EndLocation 
	 */
	UFUNCTION()
	void SpawnBulletParticle(const FVector& StartLocation, const FVector& EndLocation);

	UFUNCTION()
	void SpawnElectricProjectile(const FVector& EndLocation, APawn* BulletInstigator);
	
	/**
	 * Spawns a bullet Particle that will travel from the Weapon's Muzzle to the End Location.
	 * If the bHitUmbrella bool is set, it will also let the bullet rebound off the umbrella and set a timer to make the Umbrella actor react to the bullet hit.
	 * @param EndLocation The location that the bullet will travel to.
	 * @param bHitsUmbrella Whether the Bullet's EndLocation is located at an Umbrella Actor and thus it should trigger a reaction to it.
	 */
	UFUNCTION()
	void SpawnBulletParticleFromWeaponMuzzle(const FVector& EndLocation, const bool bHitsUmbrella);

	UFUNCTION()
	void HandleBulletHitUmbrella(const FVector& HitLocation);
	
private:
    bool CanReload() const;
    
protected:	
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

#pragma region DATA_DRIVEN
	
	UFUNCTION()
	virtual void LoadWeaponFromDataTable(){};
	
	/**
	 * Element to control Rows from DataTablePtr Type
	 * RowType="CH_Francesca", 
	 */
	UPROPERTY(EditAnywhere, Category = "Data Driven", DisplayName = "Row Type", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

#pragma endregion
};
