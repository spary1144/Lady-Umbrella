// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FallingElement.h"
#include "ReactiveElement.h"
#include "DetachableFallingElement.generated.h"


class USkeletalMeshComponent;

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ADetachableFallingElement : public AReactiveElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFallingElement> FallingClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (MakeEditWidget = true, AllowPrivateAccess = "true"))
	FTransform FallingStartingPointLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bChildSimulateFall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (EditCondition="bChildSimulateFall", EditConditionHides, AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	float ChildSimulationSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true", ClampMin = 0.1f, ClampMax = 50.f, UIMin = 0.1f, UIMax = 50.0f))
	float ChildLingeringSeconds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bChildCanDealDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	bool bChildHasToBeDestroyed;
	
protected:
	ADetachableFallingElement();

	virtual void Tick(float DeltaTime) override;
	virtual void Reaction(HitSource Source) override;
	virtual void BeginPlay() override;
	
	UPROPERTY() AFallingElement* FallingObject;
	bool bHasSpawned;
};


