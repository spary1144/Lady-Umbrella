// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericInteractable.h"
#include "VerticalHookMovement.generated.h"

class UFModComponentInterface;
class UCharacterMovementComponent;
class UCameraComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UArrowComponent;
class USplineComponent;
class APlayerCharacter;
class UAnimInstance;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVerticalHookStateChanged, EUmbrellaLocation, UmbrellaLocation, bool, bIsImmediate, bool, bPlayAnim);

UCLASS()
class LADYUMBRELLA_API AVerticalHookMovement : public AGenericInteractable
{
	GENERATED_BODY()

	AVerticalHookMovement();

	const float ACCEPTANCE_RADIUS = 60.f;

	UPROPERTY()
	bool bMovingToTarget;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void StartSplineMovement(APlayerCharacter* Interactor);
	
	UFUNCTION()
	void MovePlayerToStartOfSpline(APlayerCharacter* Interactor);

	UFUNCTION()
	void PlayVerticalHookAnimation(APlayerCharacter* Interactor);

	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnVerticalMovementCompleted(UAnimMontage* Montage, bool bInterrupted);

	float SplineProgress;

	FTimerHandle AnimationTimer;
	
	FTimerHandle SplineTimer;

	FTimerHandle MovementToSplineTimer;

	UAnimInstance* AnimInstance;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraMovingComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USplineComponent* PlayerMovementSplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SplineMoveSpeed;

	bool CheckPlayerComponentsAndVisibility(APlayerCharacter*& PlayerInteractor,
		UCharacterMovementComponent*& CharMovComp, APlayerController*& PlayerController);
	
	void ClearTimer();
	
	FVector CameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BlendTime; 

public:

	static FOnVerticalHookStateChanged OnVerticalHookStateChanged;
	
	virtual void BeginPlay() override;

	virtual int32 Interacting(APlayerCharacter* CharMovComp) override;

	FORCEINLINE virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return PLAYER_CHARACTER; };
	
};
