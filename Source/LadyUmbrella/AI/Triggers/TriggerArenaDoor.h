// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerArenaDoor.generated.h"

class AArenaManager;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class LADYUMBRELLA_API ATriggerArenaDoor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* DoorRoot;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DoorHole;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Door;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BlockingBoxCollider;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* RotationCurve;
	
	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	float PlayRate;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	bool bKeepDoorOpen;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	bool bNotCheckIfHasKeyTheFirstTime;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	AArenaManager* OwnerArena;

	UPROPERTY(EditInstanceOnly, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	bool IsActive;
	
	bool bIsOpen;
	bool bStartClose;
	
	FRotator InitialRotation;
	FRotator FinalRotation;
	float FinalRotationValue;
	
	float RotationElapsed;
	
	FTimerHandle RotationTimer;
	
	const float InRate = 0.0167f;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void RotateActorDynamic();

	UFUNCTION()
	void RotateActorDynamicInverse();

	UFUNCTION()
	void SetNewRotation(const float Alpha,FRotator& CurrentRotation,FRotator& NewRotation) const;

	void Reset();
public:	
	// Sets default values for this actor's properties
	ATriggerArenaDoor();

	void SetOwnerArena(AArenaManager* NewOwnerArena);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	bool HasKey(AActor* OtherActor);
};
