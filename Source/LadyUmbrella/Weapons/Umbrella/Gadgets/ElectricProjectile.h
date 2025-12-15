// 
// ElectricProjectile.h
// 
// Umbrella gadget that stuns enemies and activates electric puzzles. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Weapons/GenericProjectile.h"

#include "ElectricProjectile.generated.h"

class UNiagaraComponent;

DECLARE_DELEGATE(FOnShockUniqueEnemy)

UCLASS()
class LADYUMBRELLA_API AElectricProjectile : public AGenericProjectile
{
	GENERATED_BODY()
	
	const FName PARTICLE_VELOCITY_NIAGARA_PARAMETER_NAME = FName(TEXT("ParticleVelocity"));
	const FName PARTICLE_SPHERE_SIZE_MULTIPLIER_PER_SECOND_NIAGARA_PARAMETER_NAME = FName(TEXT("SphereSizeMultiplierPerSec"));
	const FName PARTICLE_SPHERE_GROWN_SIZE_MULTIPLIER_NIAGARA_PARAMETER_NAME = FName(TEXT("GrownSphereSizeMultiplier"));
	const FName PARTICLE_MAX_LIFE_NIAGARA_PARAMETER_NAME = FName(TEXT("ParticleMaxLife"));
	const FVector PARTICLE_SPHERE_INITIAL_SIZE = FVector(0.035f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = true))
	UNiagaraComponent* ProjectileParticleComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float DamageToUmbrella;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float SphereSizeMultiplierPerSecond;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float ParticleSphereGrownSizeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float ProjectileLifespan;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = true))
	float ElectrificationDuration;

private:
	float TimeAlive;


public:
	AElectricProjectile();

	FOnShockUniqueEnemy OnShockUniqueEnemy;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void ProcessCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void ProcessHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
