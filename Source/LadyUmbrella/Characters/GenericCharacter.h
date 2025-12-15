// 
// CoreCharacter.h
// 
// Generic character which is used as the common parent class of all characters in the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"

#include "GenericCharacter.generated.h"

class AMainController;
class UCharacterCoverMovementComponent;
class UHealthComponent;
class UInteractiveMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UStatusEffectComponent;
class UVoiceLineComponent;

enum class EStatusEffect : uint8;
enum class EVoiceLineState : uint8;
enum class EWeaponType : uint8;

struct FLocalizedStringRow;

// TODO: Move to HealthComponent.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathChanged, AGenericCharacter*, Character);

UCLASS()
class LADYUMBRELLA_API AGenericCharacter : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    VARIABLES                    |
	// |                                                 |
	// +-------------------------------------------------+
	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(VisibleAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = true))
	bool bInFreeRoamCover;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCharacterCoverMovementComponent> CoverMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractiveMovementComponent> InteractiveMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UVoiceLineComponent> VoiceLineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStatusEffectComponent> StatusEffectComponent;

	UPROPERTY()
	TArray<FVector_NetQuantize> HitPositions;
	
protected:

	// TODO: Move to Weapon?
	UPROPERTY(VisibleAnywhere,Category="HP Configuration")
	EWeaponType LastDamageType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AimOffsetPitchVal;
	
	UPROPERTY(VisibleAnywhere, Category = "Death")
	UNiagaraSystem* DeathSystem;

	float PrevYaw;
	bool  bHavePrevYaw;

	// Turn in place
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rotation|Speed", meta=(AllowPrivateAccess="true"))
	float YawSpeedRaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rotation|Speed", meta=(AllowPrivateAccess="true"))
	float YawSpeedSmoothed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rotation|Speed", meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	float YawDeadZoneDegPerSec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rotation|Speed", meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	float YawSmoothingSpeed;
public:
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	AGenericCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// TODO: WTF is this?
	bool UpdateAimState(const bool bIsAiming);
	
	UFUNCTION() virtual void OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned);
	UFUNCTION() virtual void OnVaultListener();

	UFUNCTION() bool PlayMontage(UAnimMontage* Montage, float PlayRate) const;
	
		// TODO: Move to HealthComponent.
		UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathChanged OnDeathChanged;
	
	virtual void SetHealth(const float NewHealth) override;
	virtual void SetDead();
	virtual void SubtractHealth(const float HealthToSubtract, EWeaponType DamageType) override;
	virtual const float GetHealth() const override;
	
	UFUNCTION() void OnDeath();
	UFUNCTION(CallInEditor, Category="HP Configuration") void MarkAsDead() { SetDead(); }
		bool IsDead() const;

	UFUNCTION() void CalculateRotationSpeed(float DeltaTime);
	void SmoothRotation(float DeltaTime);

	UFUNCTION(Blueprintcallable, Category = "Player Health")
	void KillEntity() const;

	// TODO: Move to weapon.
	FORCEINLINE void SetAiming(const bool bValue) { bAiming = bValue; }
	FORCEINLINE void SetIsReloading(const bool bValue) { bIsReloading = bValue; }
	FORCEINLINE void SetAimOffsetPitchVal(const float Value) { AimOffsetPitchVal = Value; }
	FORCEINLINE EWeaponType GetLastDamageType() const { return LastDamageType; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE void SetWeaponType(const EWeaponType Value) { WeaponType = Value; }
	


	// +-------------------------------------------------+
	// |                                                 |
	// |                GETTERS & SETTERS                |
	// |                                                 |
	// +-------------------------------------------------+
	FORCEINLINE TObjectPtr<UHealthComponent> GetHealthComponent() const { return HealthComponent; }
	FORCEINLINE TObjectPtr<UInteractiveMovementComponent> GetInteractiveMovementComponent() const { return InteractiveMovementComponent; }
	FORCEINLINE TObjectPtr<UVoiceLineComponent> GetVoiceLineComponent() const { return VoiceLineComponent; }
	FORCEINLINE TArray<FVector_NetQuantize>& GetHitPositions() { return HitPositions; }

	// NOTE: Cannot return TObjectPtr here because UFUNCTIONs require raw pointers.
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAiming() const { return bAiming; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsReloading() const { return bIsReloading; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsInFreeRoamCover() const { return bInFreeRoamCover; }
	UFUNCTION(BlueprintCallable) FORCEINLINE UCharacterCoverMovementComponent* GetCoverMovementComponent() const { return CoverMovementComponent; }
	UFUNCTION(BlueprintCallable) FORCEINLINE UStatusEffectComponent* GetStatusEffectComponent() const { return StatusEffectComponent; }
	
	FORCEINLINE void SetCoverMovementComponent(const TObjectPtr<UCharacterCoverMovementComponent>& Value) { CoverMovementComponent = Value; }
	FORCEINLINE void SetInFreeRoamCover(const bool bValue) { bInFreeRoamCover = bValue; }
};
