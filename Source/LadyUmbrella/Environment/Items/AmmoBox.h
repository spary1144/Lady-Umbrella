#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"
#include "AmmoBox.generated.h"

class UHealthComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UInteractiveIcon;

UCLASS()
class LADYUMBRELLA_API AAmmoBox : public AActor, public IHealthInterface
{
	GENERATED_BODY()
	
public:	
	AAmmoBox();

#pragma region HEALTH INTERFACE FUNCTIONS DEFINITION
	virtual void SubtractHealth(const float _HealthToSubtract,EWeaponType DamageType) override;
	virtual void SetHealth(const float _NewHealth) override;
	virtual const float GetHealth() const override;
#pragma endregion
	
protected:
	UFUNCTION()
	void OnDeath();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHealthComponent* HealthComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Box", meta = (ClampMin = "0", ClampMax = "100"))
	float DropRate = 50.0f;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Scale")
	FVector ScaleVector;

	UPROPERTY(EditAnywhere, Category = "Ammo Spawn Offset")
	FVector ZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInteractiveIcon> PickupWidget;
	
};
