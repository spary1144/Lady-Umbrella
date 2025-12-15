// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FEnemyShieldDataDriven.h"
#include "AgencyShield.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UHealthComponent;
class UPrimitiveComponent;

DECLARE_DELEGATE(FOnShieldDestroyed);

UCLASS()
class LADYUMBRELLA_API AAgencyShield : public AActor, public IHealthInterface, public IHookInterface	
{
	GENERATED_BODY()
	
	FTimerHandle TimerHandle_SpawnDelay;
	
	UPROPERTY(EditAnywhere,Category = "Components",meta=(AllowPrivateAccess=true))
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere,Category = "Components",meta=(AllowPrivateAccess=true))
	UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, Category = "General")
	UHealthComponent* HealthComponent;
	
#pragma region DATA_DRIVEN

	UFUNCTION()
	void LoadShieldFromDataTable();

	/**
	 * Element to control Rows from DataTablePtr Type
	 */
	UPROPERTY(EditAnywhere, Category = "Data Driven", DisplayName = "Row Type", meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle DataTableRowHandler;

#pragma endregion
	
public:
	FOnShieldDestroyed OnShieldDestroyedDelegate;
	// Sets default values for this actor's properties
	AAgencyShield();
	
	virtual void SubtractHealth(const float HealthToSubtract,EWeaponType DamageType) override;
	virtual void SetHealth(const float) override;
	virtual const float GetHealth() const override;
	
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
	virtual void ContactWithCable(FVector& Direction) override;
	virtual GrabType GetGrabType() override;
	void SimulatePhysics();
	
	UFUNCTION()
	void OnDestroy();

	UFUNCTION()
	void DeactivatePhysics();
	
	FORCEINLINE UHealthComponent* GetHealthComponent() { return HealthComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
