// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "GrapplingObject.generated.h"

class UMovementComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UArrowComponent;
class UTimelineComponent;
class UPrimitiveComponent;

UCLASS()
class LADYUMBRELLA_API AGrapplingObject : public AActor, public IHookInterface, public ISaveInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Material", meta = (AllowPrivateAccess = "true"))
	bool RemoveOverlayOnFinished;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook SceneRoot", meta = (DisplayName = "Scene Root", AllowPrivateAccess = "true"))
	USceneComponent* SceneRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook Skeletal Mesh",  meta = (DisplayName = "Skeletal Main mesh", AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMeshComponent;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook Skeletal Mesh",  meta = (DisplayName = "Static Main mesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box", meta = (DisplayName = "Impact Collision Box", AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box", meta = (DisplayName = "Physics Collision Box", AllowPrivateAccess = "true"))
	UBoxComponent* PhysicsCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = ( AllowPrivateAccess = "true"))
	UArrowComponent* FallingDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Has to emulate physics after Animation", AllowPrivateAccess = "true"))
	bool bHasToEmulatePhysics;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Settings", meta = (DisplayName = "Is Puzzle Solved", AllowPrivateAccess = "true"))
	bool bIsSolved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Time to emulate physics after animation", AllowPrivateAccess = "true"))
	float TimeToSimulatePhysics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Settings", meta = (DisplayName = "Force to be Applied When Hooked", AllowPrivateAccess = "true"))
	float ForceImpulseMultiplier;
	
	FTimerHandle FallingHandle;

	FTimerHandle DisappearHandler;

	float AnimationTimerCallingRate;

	float AnimationTimerFirstCallOffset;
	
	FVector DirectionOfHook;

public:	
	AGrapplingObject();
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnFallingAnimationEnd();
	void DestroyObject();

	UPROPERTY()
	FTransform SolvedTransform;

	// Hook interface inherited functions
	
    virtual void ContactWithCable(FVector& Direction) override;
    virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
    virtual GrabType GetGrabType() override;

	// Save

	virtual void SaveGame() override;
	virtual void LoadGame() override;
	virtual void SaveDataCheckPoint() override;
	virtual void LoadDataCheckPoint() override;
};
