// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleReactionComponent.generated.h"

class AEnemyCharacter;
class APlayerCharacter;
class UCharacterMovementComponent;
class UCharacterCoverMovementComponent;
class AUmbrella;
class UAnimInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrappleContactDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UGrappleReactionComponent : public UActorComponent
{
	GENERATED_BODY()

	const float GRAPPLING_POSITION_ERROR_TOLERANCE = 0.f;
	const float MIN_DISTANCE_TO_CONSIDER_BEHIND_COVER = 150.f;
	const float OFFSET_SHOOT_RANGE = 200.f;
	const FString CauseToResumeLogic = "StunByGrappleFinished";
	const FString CauseToPauseLogic = "StunByGrappleStarted";
	
	bool bStartsFallingAfterBeingHooked;
	bool bIsBehindCover;
	bool AlreadyResetGrapplePosition;
	
	float UmbrellaShotRange;
	
	UPROPERTY(EditAnywhere, Category = "Falling")
	float LaunchSpeedFactorOnFall;

	UPROPERTY(EditAnywhere, Category = "Falling")
	float LaunchSpeedFactor;
	
	UPROPERTY(EditAnywhere, Category = "Falling")
	float ExtraUpwardForce;
	
	//timers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple", meta = (AllowPrivateAccess = true))
	float GrapplingMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = true))
	float StunTime;
	
	UPROPERTY()
	FTimerHandle TimerHandle_GrapplingDelay;

	UPROPERTY()
	FTimerHandle TimerHandle_GrapplingStun;

	UPROPERTY()
	FTimerHandle TimerHandle_GrapplingMaxTime;
	
	//References
	UPROPERTY()
	AUmbrella* Umbrella;
	
	UPROPERTY()
	APlayerCharacter* PlayerRef;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY()
	UCharacterMovementComponent* MovementComponentRef;

	UPROPERTY()
	UCharacterCoverMovementComponent* CoverMovement;

	//Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage* StunnedStepsMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	UAnimMontage* FallingOnHookedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	float StunnedStepsMontagePlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = true))
	float FallingOnHookedMontagePlayRate;
public:	
	// Sets default values for this component's properties
	UGrappleReactionComponent();

	void ContactWithCable(FVector& SocketHookLocation);

	FORCEINLINE bool GetStartsFallingAfterBeingHooked() const { return bStartsFallingAfterBeingHooked; }

	UFUNCTION()
	void EndGrapplingPosition();

	UFUNCTION()
	void EndGrappleStun();

	UPROPERTY(BlueprintAssignable, Category="Aux")
	FOnGrappleContactDelegate OnGrappleContact;

	void Initialize();
protected:
	UPROPERTY()
	AEnemyCharacter* GenericCharacter;
	// Called when the game starts
	virtual void BeginPlay() override;

	
private:
	void CheckEnemyBehindCover(const FVector& SocketHookLocation);
	
	bool CheckIfWillFallOnHooked(FVector& LaunchDirection);
	
	UFUNCTION()
	void SetGrapplingPosition(FVector& EndPoint);

	
	void ManageGrapplePosition(FVector& SocketHookLocation);

	void PlayStunStepsAnimation();

	void PlayFallingAnimation(UAnimMontage* FallingAnimation, float PlayRate) const;

	float CalculateDynamicStunTime(const FVector& SocketHookLocation);
	
	UFUNCTION()
	void OnStunStepsMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
};

