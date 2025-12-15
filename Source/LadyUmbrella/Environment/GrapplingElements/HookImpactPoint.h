// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "HookImpactPoint.generated.h"	

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHookImpactDynamic, const float, DegreesOfRotation, const float, SpeedOfRotation, const FRotator&, RotatorForParent);

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class LADYUMBRELLA_API AHookImpactPoint : public AActor, public IHookInterface
{
	GENERATED_BODY()

#pragma region HOOK INTERFACE FUNCTIONS
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override {};
	virtual void ContactWithCable(FVector& Direction) override;
#pragma endregion
	
public:	
	AHookImpactPoint();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnHookImpactDynamic OnHookImpactDynamic;

	UFUNCTION()
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComponent; }
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ImpactCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DegreesOfRotationForParent;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SpeedOfRotationForParent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator RotatorForParent;	
	
public:	

	//[[nodiscard]] FORCEINLINE float GetDegreesOfRotationForParent() const { return DegreesOfRotationForParent; }
	[[nodiscard]] FORCEINLINE float GetSpeedOfRotationForParent()   const { return SpeedOfRotationForParent; }
	
#pragma region HOOK INTERFACE GETTER
	virtual GrabType GetGrabType() override { return PULL; }
#pragma endregion
};
