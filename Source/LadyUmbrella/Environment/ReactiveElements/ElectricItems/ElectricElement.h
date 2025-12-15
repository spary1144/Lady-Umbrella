#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "ElectricElement.generated.h"

class UFModComponentInterface;

UCLASS(Abstract)
class LADYUMBRELLA_API AElectricElement : public AReactiveElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodElectricSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodReactionSound;
	

	const FName ElectricSoundParameterName	 = FName("ELECTRIC_LOOP");
	const uint8 ElectricSoundParameterOn	 = 0;
	const uint8 ElectricSoundParameterOff    = 1;

public:
	AElectricElement();

protected:
	
	virtual void BeginPlay() override;
	
	void OnConstruction(const FTransform& Transform);

	UPROPERTY(BlueprintReadOnly, Category = "Electric State")
	bool bElectrified;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Electric State")
	bool bShowSparkEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State")
	float ElectrificationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State")
	UParticleSystem* SparksEffect;

	UPROPERTY()
	UParticleSystemComponent* SparksComponent;

	UPROPERTY()
	FTimerHandle ElectrifiedTimerHandle;

	UFUNCTION()
	virtual void ResetElectrifiedState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Electric State")
	TArray<AElectricElement*> ConnectedElectricElements;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Scene Root Object", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootComp;
	// Mesh position, rotation, and scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshPosition; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FRotator MeshRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector MeshScale;
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp,
	                       AActor* OtherActor,
	                       UPrimitiveComponent* OtherComp,
	                       int32 OtherBodyIndex,
	                       bool bFromSweep,
	                       const FHitResult& SweepResult);
public:
	
	UFUNCTION(BlueprintCallable, Category = "Electric State")
	virtual void SetElectrified(const bool bNewElectrified);
	
	UFUNCTION()
	virtual void PerformElectrification();

	UFUNCTION(BlueprintImplementableEvent, Category = "Electric State")
    void OnElectrified();

	UFUNCTION(BlueprintImplementableEvent, Category = "Electric State")
	void OffElectrified();
	
	UPROPERTY(EditAnywhere, Category = "Electric State")
	float ElectrifiedDelaySeconds;

	FTimerHandle ElectrifiedDelayTimerHandle;

	//FORCEINLINE TObjectPtr<UFModComponentInterface> GetFmodInterfaceComponent() const noexcept { return FmodReactionComponentInterface;}
	
};
