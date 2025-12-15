// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerArenaWindow.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AArenaManager;

UCLASS()
class LADYUMBRELLA_API ATriggerArenaWindow : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WindowFrame;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShutterLeft;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShutterRight;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BlockingBoxCollider;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* RotationCurve;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	float PlayRate;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	bool bNotCheckIfHasKeyTheFirstTime;

	UPROPERTY(VisibleInstanceOnly, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	AArenaManager* OwnerArena;
	
	float FinalRotationValue;
	
	FRotator InitialRotationShutterRight;
	FRotator InitialRotationShutterLeft;
	FRotator FinalRotation;
	FTimerHandle RotationTimer;
	float RotationElapsed;
	bool bHasBegunClose;
	bool bIsOpen;
	const float InRate = 0.0167f;
	
	UFUNCTION()
	void RotateActorDynamic();

	UFUNCTION()
	void RotateActorDynamicInverse();
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Reset();
public:	
	// Sets default values for this actor's properties
	ATriggerArenaWindow();

	void SetOwnerArena(AArenaManager* NewOwnerArena);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool HasKey(AActor* OtherActor);
};
