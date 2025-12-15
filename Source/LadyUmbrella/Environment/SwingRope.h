// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/InteractiveMovement.h"
#include "SwingRope.generated.h"

class UBoxComponent;
class APlayerCharacter;
class UBillboardComponent;
class UFModComponentInterface;
class UTextRenderComponent;
class UCharacterMovementComponent;
class UCurveFloat;
class UTimelineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwingRopeStateChanged, EUmbrellaLocation, UmbrellaLocation, bool, bIsImmediate, bool, bPlayAnim);

UCLASS()
class LADYUMBRELLA_API ASwingRope : public AActor, public IInteractiveMovement
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwingRope();

	virtual bool Interact(AActor* OtherActor) override;

	static FOnSwingRopeStateChanged OnSwingRopeStateChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	USceneComponent* Root;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element")
	USceneComponent* VisibleRoot;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBoxComponent* BoxCollider1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBoxComponent* BoxCollider2;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* SwingLocation1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* SwingLocation2;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* SwingLocation1Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* SwingLocation2Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* TargetPoint1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UBillboardComponent* TargetPoint2;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* TargetPoint1Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	UTextRenderComponent* TargetPoint2Label;

	UPROPERTY(VisibleDefaultsOnly, Category = "Swing Element")
	TObjectPtr<UFModComponentInterface> FmodSwingSoundComponent;  
	
	UPROPERTY(EditInstanceOnly, Category = "Swing Element")
	bool bShowDebugLines;

private:

	UPROPERTY()
	UCharacterMovementComponent* MovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwingMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	float SwingAnimationPlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	float SwingTimeLinePlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	float TravelTimeToSwingLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	float TravelTimeToTargetLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	FVector TargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	FRotator StaticMeshOriginalRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	FRotator StaticMeshFinalRotationA;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	FRotator StaticMeshFinalRotationB;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SwingRopeMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	float MinFacingDotThreshold;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	float TimelineLaunchMoment;

	UPROPERTY()
	bool bEnterPrimarySide;

	UPROPERTY()
	FTimerHandle ParabolicLaunchTimerHandler;

	UPROPERTY()
	FTimerHandle MoveToSwingLocationTimerHandler;
	
	UPROPERTY()
	FTimerHandle CooldownTimerHandler;
	
	UPROPERTY(EditAnywhere, Category = "Swing Element")
	float Cooldown;
	
	UPROPERTY(VisibleAnywhere, Category = "Swing Element")
	bool bIsInCooldown;
	
	UPROPERTY()
	float OriginalBrakingFrictionFactor;
	
	UPROPERTY()
	float OriginalBrakingDecelerationFlying;
	
	UPROPERTY()
	float OriginalBrakingDecelerationFalling;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swing Element", meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* SwingTimeLine;
	

	UFUNCTION()
	void Swing() const;

	UFUNCTION()
	void OnSwingCompleted(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void LaunchParabolicTo(const FVector& Start, const FVector& End, float Time) const;

	UFUNCTION()
	void TryToClearParabolicLaunchTimer();

	UFUNCTION()
	void MoveComponentToSwingTargetFinished() const;

	UFUNCTION()
	void SwingTimelineProgress(float Value);

	UFUNCTION()
	void SwingTimelineFinish() const;

	UFUNCTION()
	void CooldownReset();

	UFUNCTION()
	bool CheckIfPlayerFacingBar(const FVector& RefLocation);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
