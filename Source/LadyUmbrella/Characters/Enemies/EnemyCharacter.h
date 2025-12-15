// 
// EnemyCharacter.h
// 
// Parent class for all Enemy Character in the game
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Interfaces/KickInterface.h"
#include "LadyUmbrella/UI/Basics/Icon/EnemyStateIcon.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/Weapons/WeaponTypes.h"

#include "EnemyCharacter.generated.h"

class AAmmo;
class UGrappleReactionComponent;
class AAIController;
class AGenericWeapon;
class AIController;
class ASplinePath;
class AWayPoint;
class UEnemyStateIcon;
class UInteractiveMovementComponent;
class UTexture2D;
class UWidgetComponent;
class UAnimInstance;
class USkeletalMeshComponent;

DECLARE_DELEGATE(FOnElectricShootFinished)

const uint8 VAULT_STUNS_TO_ACHIEVEMENTS = 15;

UCLASS()
class LADYUMBRELLA_API AEnemyCharacter : public AGenericCharacter, public IHookInterface, public IKickInterface
{
	GENERATED_BODY()
public:
	FOnElectricShootFinished OnElectricShootFinished;

	AEnemyCharacter();
	
	void TryEquipWeapon();

	void Initialize();

	virtual void BeginPlay() override;
	virtual void SetDead() override;
	void DestroyEquippedWeapon();
	void StopVoiceLineActive() const;
	virtual void SubtractHealth(const float _HealthToSubtract, const EWeaponType DamageType) override;
	virtual void OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned) override;
	void KillCharacter();
	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnDetectPlayer();

	bool WillFallOffLedge(FVector& LaunchDirection);
	
	void EndGrapplingPosition() const;
	
	UFUNCTION()
	void OnAllyDeath(const FVector& DeadLocation) const;
	
	UFUNCTION(Category = "Weapon")
	virtual bool TryToFire();

	UFUNCTION(Category = "Weapon")
	void Reload();

	UFUNCTION()
	void ContactWithMeleeAttack(FVector& Direction);

	UFUNCTION()
	void PauseAILogic(const FString& Cause) const;

	UFUNCTION()
	void StopAILogic(const FString& Cause) const;

	UFUNCTION()
	void ResumeAILogic(const FString& Cause) const;

	// UFUNCTION()
	// void SpawnElectricProjectile();
	bool IsInCloseCombat() const;
	
	FORCEINLINE ASplinePath* GetSplinePath() const { return Splinepath; };
	FORCEINLINE AGenericWeapon* GetEquippedWeapon() const { return EquippedWeapon; };
	FORCEINLINE float GetMissChance() const { return MissChance; };
	// TODO: Review
	FORCEINLINE UEnemyStateIcon* GetWidget() const { UEnemyStateIcon* EnemyStateIcon = Cast<UEnemyStateIcon>(WidgetComponent->GetUserWidgetObject()); return EnemyStateIcon; };
	FORCEINLINE bool GetHasToken() const { return bHasToken; };
	FORCEINLINE virtual AActor* GetShield() const { return nullptr; }
	FORCEINLINE virtual GrabType GetGrabType() override { return GrabType::PULL; }
	FORCEINLINE uint32 GetUnlockID() const{ return UnlockKey; }
	FORCEINLINE FVector GetLastHitPosition() const { return LastHitPosition; }
	FORCEINLINE bool HasBeenHookedBefore() const { return bHasBeenHookedBefore; }
	FORCEINLINE bool HasBeenShockedBefore() const { return bHasBeenShockedBefore; }
	
	FORCEINLINE void SetAIController(AAIController* NewAIController) { AIController = NewAIController; };
	FORCEINLINE void SetSplinePath(ASplinePath* NewSplinePath) { Splinepath = NewSplinePath;};
	FORCEINLINE void SetHasToken(const bool NewTokenState) { bHasToken = NewTokenState; };
	FORCEINLINE void SetMissChance(float NewMissChance) { MissChance = NewMissChance; };
	FORCEINLINE void SetWeaponBP(const TSubclassOf<AGenericWeapon>& NewWeaponBP) { WeaponBP = NewWeaponBP; };
	FORCEINLINE void SetUnlockID(uint32 KeyId) { UnlockKey = KeyId; }
	FORCEINLINE void SetLastHitPosition(const FVector InLastHitPosition) { LastHitPosition = InLastHitPosition; }
	void TriggerVoiceLineFirstCombat(const uint8_t Index);
	FORCEINLINE void SetHasBeenHookedBefore(bool bValue) { bHasBeenHookedBefore = bValue; }
	FORCEINLINE void SetHasBeenShockedBefore(bool bValue) { bHasBeenShockedBefore = bValue; }
	
private:
	const float LIFE_SPAN = 30.f;
	const float RAGDOLL_VERTICAL_FORCE = 200.f;
	const float KICK_UP_FORCE = 10.f;
	const int	MIN_DROP_AMOUNT = 1;
	const float MIN_AMMO_DROP_RATE = 0.f;
	const float MAX_AMMO_DROP_RATE = 1.f;
	const float IN_COVER_ELECTRIC_PROJECTILE_VERTICAL_OFFSET = -80;
	const FVector VERTICAL_LINE_TRACE_OFFSET = FVector(0, 0, -20);
	const float BULLET_MISS_LATERAL_PROBABILITY = 0.66f;
	const float ENEMY_KICKED_IMPULSE = 3000.f;
	const FVector OffsetOnStartSphereTraceLocation = FVector::ZeroVector;
	const FVector OffsetOnEndSphereTraceLocation = FVector(0.f, 0.f, -50.f);
	const float DISTANCE_CENTER_SPHERE_TO_HIT_LOCATION = 15.f;
	const float MIN_DISTANCE_TO_CONSIDER_FALLING = 500.f;
	const float SPHERE_RADIUS = 20.f;
	const float DRAWTIME = 5.f;
	const FVector MAX_SAFE_HEIGHT = FVector(0, 0, 1500);
	const float FORCE_DOWN = -1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = true))
	bool bEnableDebugMessages;
	
	UPROPERTY(EditAnywhere, Category = "Bullet Rebound")
	float ProjectileSpeed;
	UPROPERTY(EditAnywhere, Category = "Bullet Rebound")
	float LeadFactor;
	UPROPERTY(EditAnywhere, Category = "Bullet Rebound")
	float MaxOffset;
		
	UPROPERTY()
	AAIController* AIController;

	UPROPERTY(EditAnywhere, Category = "Falling")
	float DistanceToCheckFalling;

	UPROPERTY()
	FVector LaunchSpeedFactorOnFallByMelee;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	TSubclassOf<AGenericWeapon> WeaponBP;

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	FName socketName;
	
	UPROPERTY(EditAnywhere, Category = "CoordinationManager")
	bool bHasToken;

	UPROPERTY(EditAnywhere)
	bool bHasShield;
	
	UPROPERTY(VisibleAnywhere, Category = "AI | Path following")
	ASplinePath* Splinepath;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float StunDuration;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float SpeedCrouching;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = true))
	float RagdollProbability;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	bool bCanInteractWithReactiveElements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	float FreezeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	float FreezeTimeDilation;
	
	TWeakObjectPtr<UCurveFloat> AmmoDropAmountCurve;
	TWeakObjectPtr<UCurveFloat> AmmoDropRateCurve;
	TWeakObjectPtr<UCurveFloat> DeadForceCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = true))
	TSubclassOf<AAmmo> AmmoBP;
	
	UPROPERTY(EditAnywhere, Category = "Weapon | PlayerLuck", meta = (AllowPrivateAccess = true))
	float ForgivenessTime;

	UPROPERTY(EditAnywhere, Category = "Weapon | PlayerLuck", meta = (AllowPrivateAccess = true))
	float AimCoverForgivenessTime;

	UPROPERTY(VisibleAnywhere, Category = "Weapon | PlayerLuck")
	bool bPlayerLuckActive;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon | MissChance")
	float MissChance;

	UPROPERTY(VisibleAnywhere, Category = "Weapon | MissChance")
	bool bUseDefaultMissChanceSystem;

	UPROPERTY(EditAnywhere, Category = "Weapon | MissChance")
	float DefaultMissChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MinLeftRightOffsetLongDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MaxLeftRightOffsetLongDistance;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MinLeftRightOffsetCloseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MaxLeftRightOffsetCloseDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MinUpOffsetLateralBullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MaxUpOffsetLateralBullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MinLeftRightOffsetOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MaxLeftRightOffsetOverhead;
			
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MinUpOffsetOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | MissChance", meta = (AllowPrivateAccess = true))
	float MaxUpOffsetOverhead;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	bool bCanFireWeapon;

	//Animations
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage* FallingOnPushedByMeleeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	float FallingOnPushedByMeleePlayRate;
	
	UPROPERTY()
	FTimerHandle MissChanceTimer;
	UPROPERTY()
	FTimerHandle CoverMissChanceTimer;
	UPROPERTY()
	FTimerHandle PlayerLuckTimer;
	UPROPERTY()
	FTimerHandle FreezeFrameTimerHandler;
	UPROPERTY()
	FTimerHandle FireWeaponTimerHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGrappleReactionComponent> GrappleReactionComponent;

	//Voice Lines Fmod Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodComponentVoiceLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UFModComponentInterface> FmodWhooshComponentInterface;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	UParticleSystem* ParticleExplosionSystem;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	FVector ParticleExplosionScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage* ReloadMontage;

	UPROPERTY(VisibleAnywhere, Category = "Montage", meta = (AllowPrivateAccess = true))
	float ReactMontagePlayRate;

	UPROPERTY(EditAnywhere, Category = "Montage", meta = (AllowPrivateAccess = true))
	float KickMontagePlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Kick", meta = (AllowPrivateAccess = true))
	float KickStunDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	TArray<UAnimMontage*> DamagedAnimMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage* CurrentDamageAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics", meta = (AllowPrivateAccess = true))
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UEnemyStateIcon> EnemyStateWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	UTexture2D* EnemyStunTexture;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint32 UnlockKey;
	
	FVector LocationBeforeFall;
	
	bool bStartsFallingAfterBeingPushedByMelee;

	UPROPERTY(EditAnywhere, Category = "Graphics", meta = (AllowPrivateAccess = "true"))
	bool bDebugSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling", meta = (AllowPrivateAccess = true))
	float MinDistanceToConsiderFalling;
	
	UPROPERTY(EditAnywhere, Category = "Detection", meta = (AllowPrivateAccess = "true"))
	float MinDistanceToListenAllyDeath;

	UPROPERTY(VisibleAnywhere, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	FVector LastHitPosition;

	bool bHasBeenHookedBefore;
	bool bHasBeenShockedBefore;

	// +-------------------------------------------------+
	// |                                                 |
	// |                   FUNCTIONS                     |
	// |                                                 |
	// +-------------------------------------------------+
	UFUNCTION()
	void ActivateRagdoll();

	UFUNCTION()
	void AddImpulseDependingOnDamageType() const;

	UFUNCTION()
	void OnDeathMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	virtual void KickActor(const FVector& Direction) override;
	virtual void PrepareKnockBackActor() override;
	
	void DropAmmo();
	
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
	virtual void ContactWithCable(FVector& Direction) override;

	void PlayFallingAnimationOnPushedByMelee() const;

	UFUNCTION()
	void ChangeEnemiesMissChance(bool NewState);

	UFUNCTION()
	void ModifyMissChanceDependingOnUmbrellaState(bool IsShieldOpen);

	UFUNCTION()
	void WaitToChangeEnemiesMissChance();
	
	UFUNCTION()
	void CheckLineOfSightWithPlayer();

	UFUNCTION()
	void LosePlayerLuck();
	
	FVector GetPawnTraceHitPoint(const FCollisionQueryParams& CollisionParams) const;
	bool HandleLandedShot(FHitResult& OutHit, const FCollisionQueryParams& CollisionParams, const FVector& WeaponTraceStart, const FVector& WeaponTraceEnd);
	void HandleMissedShot(FHitResult& OutHit, const FCollisionQueryParams& CollisionParams, const FVector& WeaponTraceStart, const FVector& WeaponTraceDirection, const FVector& WeaponTraceEnd, const FVector& PawnTraceHitPoint);
	
	UFUNCTION()
	FVector GetBulletMissedDirection(const FVector& WeaponTraceDirection, const FVector& WeaponTraceEnd, const FVector& PawnTraceHitPoint) const;
	
	UFUNCTION()
	float CalculateMissChance() const;
	
	UFUNCTION()
	virtual void BulletInteraction(const FHitResult& OutHit) const;
	
	UFUNCTION()
	void BulletInteractionWithReactiveElements(const FHitResult& OutHit) const;
	
	UFUNCTION()
	void PlayReactMontage();

	UFUNCTION()
	void PlayKickedMontage();
	
	UFUNCTION()
	void OnReloadCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDamagedMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnKickedMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnShieldDepleted();
protected:
	
	FName RowName;
	
	UFUNCTION()
	virtual void LoadEnemyFromDataTable(){};

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Temp")
	void OnKickEvent();

	UFUNCTION(Category = "Weapon")
	void GetFireHitResult(FHitResult &OutHit, const bool bMiss);
	
	UPROPERTY(VisibleInstanceOnly, Category = "Data Driven", DisplayName = "Row Type", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;
	
	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	USkeletalMeshComponent* MeshRef;
	
	UPROPERTY(EditAnywhere, Category = "Weapon | MissChance")
	float MaxMissChance;

	UPROPERTY(EditAnywhere, Category = "Weapon | MissChance")
	float MinMissChance;

	UPROPERTY(EditAnywhere, Category = "Weapon | MissChance")
	float MaxShootRange;

	UPROPERTY(EditAnywhere, Category = "Weapon | MissChance")
	float MinShootRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = true))
	AGenericWeapon* EquippedWeapon;

	FORCEINLINE TObjectPtr<UFModComponentInterface> GetFmodComponentVoiceLine() { return FmodComponentVoiceLines; }
	
};


