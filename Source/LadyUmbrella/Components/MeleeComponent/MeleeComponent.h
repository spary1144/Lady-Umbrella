// 
// MeleeComponent.h
// 
// Allows the player to use the umbrella to melee attack. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "MeleeComponent.generated.h"

class UFModComponentInterface;
class UTimelineComponent;

DECLARE_DELEGATE(FOnMeleeAttack);

const uint8 KILLS_TO_ACHIEVEMENT = 5;

UCLASS()
class LADYUMBRELLA_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

	// +-------------------------------------------------+
	// |                                                 |
	// |                    VARIABLES                    |
	// |                                                 |
	// +-------------------------------------------------+
	// TODO: Do all these variables really need to be visible to unreal?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = true))
	bool bCanDetect;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Melee", meta = (AllowPrivateAccess = true))
	bool bIsAttacking;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee", meta = (AllowPrivateAccess = true))
	int32 Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = true))
	TObjectPtr<UParticleSystem> ImpactParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = true))
	FVector LaunchAddedVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = true))
	float LaunchFactor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee", meta = (AllowPrivateAccess = true))
	float Range;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> TimeLineComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee", meta = (AllowPrivateAccess = true))
	float TraceRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> MeleeMontages;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	int32 CurrentMeleeMontageIndex;

	UPROPERTY()
	FTimerHandle ComboTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodMeleComponentInterface;
	
public:

	// NOTE: Delegates CANNOT be private because otherwise they cant broadcast outwards.
	FOnMeleeAttack OnMeleeAttack;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	UMeleeComponent();

	virtual void BeginPlay() override;
	
	UFUNCTION() void Attack();

	TWeakObjectPtr<AActor> GetClosestEnemyInRange() const;
	
	UFUNCTION(BlueprintCallable)
	void ProcessCollision(const FVector& TraceStart, const FVector& ActorRightVector);

	// +-------------------------------------------------+
	// |                                                 |
	// |                GETTERS & SETTERS                |
	// |                                                 |
	// +-------------------------------------------------+
	FORCEINLINE float GetDamage() const  { return Damage; }
	FORCEINLINE float GetLaunchFactor() const { return LaunchFactor; }
	FORCEINLINE float GetTraceRadius() const  { return TraceRadius; }
	UFUNCTION(BlueprintCallable) FORCEINLINE bool IsAttacking() const { return bIsAttacking; }
	UFUNCTION() UAnimMontage* GetMeleeMontage();

	FORCEINLINE void SetDamage(const int32 Value)  { Damage = Value; }
	FORCEINLINE void SetIsAttacking(const bool bValue) { bIsAttacking = bValue; }
	FORCEINLINE void SetLaunchFactor(const float Value) { LaunchFactor = Value; }
	FORCEINLINE void SetTraceRadius(const float Value)  { TraceRadius = Value; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetCanDetect(const bool bValue) { bCanDetect = bValue; }
};
