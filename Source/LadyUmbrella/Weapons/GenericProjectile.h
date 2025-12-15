// 
// GenericProjectile.h
// 
// Main class for all the projectiles in the game. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class LADYUMBRELLA_API AGenericProjectile : public AActor
{
	GENERATED_BODY()
public:
	// CONSTANTS
	const FString START_POSITION_NIAGARA_SYSTEM_VARIABLE_NAME = FString("StartPosition");
	const FString END_POSITION_NIAGARA_SYSTEM_VARIABLE_NAME = FString("EndPosition");
	
private:
	/****************************/
	/*         GAMEPLAY         */
	/****************************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (AllowPrivateAccess = true))
	float ProjectileDamage;
	
	/****************************/
	/*         PHYSICS          */
	/****************************/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = true))
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> SphereComponent;
	
public:
	AGenericProjectile();

	UFUNCTION()
	virtual void ProcessCollision(  UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void ProcessHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE float GetProjectileDamage() const { return ProjectileDamage; }
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
	FORCEINLINE TObjectPtr<USphereComponent> GetSphereComponent() const { return SphereComponent; }

	FORCEINLINE void SetProjectileDamage(const float Value) { ProjectileDamage = Value; }
	FORCEINLINE void SetProjectileMovementComponent(UProjectileMovementComponent* Value) { ProjectileMovementComponent = Value; }
	FORCEINLINE void SetSphereComponent(const TObjectPtr<USphereComponent>& Value) { SphereComponent = Value; }

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
