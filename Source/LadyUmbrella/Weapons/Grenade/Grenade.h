// 
// Grenade.h
// 
// A grenade is a projectile that explodes after a short amount of time.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FGrenadeDataDriven.h"
#include "Grenade.generated.h"

class AMafiaCharacter;
class UFModComponentInterface;
class APlayerCharacter;
class UNiagaraSystem;
class USoundBase;
class UAudioComponent;
class UProjectileMovementComponent;

DECLARE_DELEGATE_OneParam(FOnGrenadeThrown, const FVector&);
DECLARE_MULTICAST_DELEGATE(FOnGrenadeBounced)

const uint8 GRENADE_REBOUND_COUNT = 5;

UCLASS()
class LADYUMBRELLA_API AGrenade : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	bool bExplodeOnContact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Graphics, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* NS_Explosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ExplosionAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UFModComponentInterface* FModAudioComponentBounce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UFModComponentInterface* FModAudioComponentExplode;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Graphics, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DamageFalloff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AMafiaCharacter> MafiaCharacterClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float ExplosionRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float MaxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float MinDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Restitution, meta = (AllowPrivateAccess = "true"))
	float GrenadeRestitution;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Life, meta = (AllowPrivateAccess = "true"))
	float GrenadeTimeToExplode;
	
	UPROPERTY()
	FTimerHandle UnusedHandle;

	UPROPERTY()
	TArray<AActor*> DamagedActors;

	bool bHasBeenRebounded;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void Explode();
	void DestroyGrenade();

	virtual void BeginPlay() override;

	bool ShouldDamageActor(const AActor* HitActor) const;
	bool ShouldDamagePlayer(const APlayerCharacter* PlayerCharacter) const;
	static bool ShouldDamageGod(const AActor* HitActor);


#pragma region DATA_DRIVEN
protected:
	
//	const FName RowName = "BP_Grenade";
	
	UFUNCTION()
	void LoadGrenadeFromDataTable();

	/**
	 * Element to control Rows from DataTablePtr Type
	 * RowType="CH_Francesca", 
	 */
	UPROPERTY(VisibleInstanceOnly, Category = "Data Driven", DisplayName = "Row Type", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

	/**
	* Struct that represents the data that there is inside the DataTablePtr. Not needed for now, but will come in handy
	*/
	UPROPERTY(VisibleInstanceOnly, Category = "Data Driven", DisplayName = "Grenade Table Struct")
	FGrenadeDataDriven DataDrivenStruct;

	
#pragma endregion

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Temp")
	void OnGrenadeExplodedEvent();

public:
	AGrenade();

	//Delegate
	FOnGrenadeThrown OnGrenadeThrown;
	static FOnGrenadeBounced OnGrenadeBounced;
	
	UFUNCTION(BlueprintCallable)
	void Throw(const FVector& Impulse);

	FORCEINLINE UNiagaraSystem* GetExplosionEffect() const { return NS_Explosion; }
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
	FORCEINLINE USphereComponent* GetSphereComponent() const { return SphereComponent; }

private:
	float GetGrenadeDamage(const AActor* HitActor) const;

protected:
	virtual void Tick(float DeltaSeconds) override;
	
	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Temp")
	void OnPlayerDamaged();
};
