#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserElement.generated.h"

class UFModComponentInterface;
class UNiagaraSystem;
class UNiagaraComponent;

UENUM()
enum class ELaserState : uint8
{
	Deactivated UMETA(DisplayName = "Deactivated"),
	Activated   UMETA(DisplayName = "Activated")
};

UCLASS()
class LADYUMBRELLA_API ALaserElement : public AActor
{
	GENERATED_BODY()

public:
	
	ALaserElement();

	void ActivateLaser();
	
	void DeactivateLaser();
	
protected:
	
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser")
	UNiagaraSystem* LaserNiagaraSystem;

	UPROPERTY(VisibleAnywhere, Category = "Laser")
	UStaticMeshComponent* LaserBaseMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser Settings")
	FVector BoxLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser Settings")
	FVector BoxExtent;

	UPROPERTY(EditAnywhere, Category = "Laser Settings")
	float LaserRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser Settings")
	ELaserState CurrentState;
	
private:
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* DetectionBox;

	UPROPERTY()
	TObjectPtr<UFModComponentInterface> ImpactLasersFModComponentInterface;

	FTimerHandle DeactivateTimerHandle;

	void FireLaser() const;

	UFUNCTION()
	void OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float LaserDamage;

	UPROPERTY(Blueprintable, EditAnywhere, Category = "FMOD", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodLaserComponentInterface; 
	
};