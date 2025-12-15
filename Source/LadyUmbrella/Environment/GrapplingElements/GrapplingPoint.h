// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "GrapplingPoint.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;

class AUmbrella;


UCLASS()
class LADYUMBRELLA_API AGrapplingPoint : public AActor, public IHookInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplingPoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrapplingPoint")
	UBoxComponent* CollisionBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrapplingPoint")
	UStaticMeshComponent* SM_StaticMeshComponent;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ContactWithCable(FVector& Direction) override;
	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{}
	virtual GrabType GetGrabType() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
