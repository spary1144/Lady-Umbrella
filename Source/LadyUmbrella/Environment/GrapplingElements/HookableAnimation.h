// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HookableParent.h"
#include "HookableAnimation.generated.h"

class UArrowComponent;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AHookableAnimation : public AHookableParent
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Hook Point should disable material after Animation", AllowPrivateAccess = "true"))
	bool bDisableHookPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Has to emulate physics after Animation", AllowPrivateAccess = "true"))
	bool bHasToEmulatePhysics;
 
	float AnimationTimerCallingRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Time to emulate physics after Animation", AllowPrivateAccess = "true"))
	float TimeToSimulatePhysics;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Force of Impulse", AllowPrivateAccess = "true"))
	float ForceImpulseMultiplier;
	
	FVector DirectionOfHook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMeshComponent;

	// The front vector of the arrow will be used to give the direction of the impulse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Component", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Impact Point", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ImpactPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact Point", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHookImpactPoint> HookImpactPointClass;
	
public:
	
	AHookableAnimation();
	
	virtual void RotateFromChildDynamic(const float DegreesOfRotation, const float SpeedOfRotation, const FRotator& RotatorForParent) override;
	
	FTimerHandle FallingHandler;
    FTimerHandle DisappearHandler;
	
	UFUNCTION()
	void OnAnimationEnded();
	void DisableHookPointMaterial() const;

	UFUNCTION()
	void DestroyObject();

};
