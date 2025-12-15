// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MontageTrigger.generated.h"

class UArrowComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementCompleted);

UCLASS()
class LADYUMBRELLA_API AMontageTrigger : public AActor
{
	GENERATED_BODY()

	const float ACCEPTANCE_RADIUS = 50.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* TargetLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	bool bShouldMoveToLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	float MontageStartDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	float MovePlayerDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	bool bMovingToTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MontageTrigger", meta = (AllowPrivateAccess = "true"))
	bool bIsActive;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UFUNCTION()
	bool TryToPlayMontage();
	
	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
public:	
	AMontageTrigger();
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMovementCompleted OnMovementCompleted;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TryToMovePlayerToLocation();

	UFUNCTION()
	void PlayDelayedMontage();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, 
						AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex, 
						bool bFromSweep, 
						const FHitResult & SweepResult);
};
