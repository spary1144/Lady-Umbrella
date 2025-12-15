// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReactiveElement.h"
#include "FallingElement.generated.h"


class USkeletalMeshComponent;
class UFModComponentInterface;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AFallingElement : public AReactiveElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bSimulateFall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (EditCondition="bSimulateFall", EditConditionHides, AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	float Step;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bHasToBeDestroyed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (EditCondition="bHasToBeDestroyed", EditConditionHides, AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	float LingeringSeconds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bCanDealDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (EditCondition="bCanDealDamage", EditConditionHides, AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	bool bCoverBox;

	UFUNCTION()
	void HandleCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:
	
	AFallingElement();
	
	virtual void Reaction(HitSource Source) override;
	FORCEINLINE void SetSimulateFall(const bool bSimulate) { bSimulateFall = bSimulate; }
	FORCEINLINE bool GetSimulateFall() const { return bSimulateFall; }
	FORCEINLINE void SetDestroy(const bool bDestroy) { bHasToBeDestroyed = bDestroy; }
	FORCEINLINE bool GetDestroy() const { return bHasToBeDestroyed; }
	FORCEINLINE void SetCanDealDamage(const bool bDamage) { bCanDealDamage = bDamage; }
	FORCEINLINE bool GetCanDealDamage() const { return bHasToBeDestroyed; }
	FORCEINLINE void SetStepSeconds(const float Seconds) { Step = Seconds; }
	FORCEINLINE float GetStepSeconds() const { return Step; }
	FORCEINLINE void SetLingeringSeconds(const float Seconds) { LingeringSeconds = Seconds; }
	FORCEINLINE float GetLingeringSecs() const { return LingeringSeconds; }
	FORCEINLINE FVector GetColliderSize() const { return ColliderSize; }
	
protected:
	
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:

	float FallingDistance;
	float GravityForce;
	float CurrentSpeed;
	float TargetHeight;
	FVector ColliderSize;

	const float SimulationInitialAnimPosition = 0.1665f;
	const FName SocketName = FName("FallingSocket");
	const FName PrimitiveName = FName("FallingPrimitive");
	const FName EndCollisionNotify = FName("EndHurtbox");
	const FVector CollisionBoxScaler =  {0.45f,0.5f,0.45f};
	const FVector AltCollisionBoxScaler =  {0.55f,0.55f,0.55f};
	const FVector CollisionBoxOffset =  {0.f,10.f,0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fmod Components", meta = (AllowPrivateAccess = "true"))
	UFModComponentInterface* FmodComponentInterface;
};


