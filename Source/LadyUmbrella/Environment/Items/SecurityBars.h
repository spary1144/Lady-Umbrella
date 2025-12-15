// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SecurityBars.generated.h"

class AArenaManager;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ESecurityBarState : uint8
{
	Blocked		UMETA(DisplayName = "Blocked"),
	Lifted		UMETA(DisplayName = "Lifted"),
};

UCLASS()
class LADYUMBRELLA_API ASecurityBars : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Security System", meta = (AllowPrivateAccess = "true"))
	ESecurityBarState SecurityBarState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	AArenaManager* ArenaManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ElevationCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	float HeightDiff;

	UPROPERTY()
	FTimeline ElevationTimeline;

	UPROPERTY()
	FVector InitialPosition;
	
public:
	ASecurityBars();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void ElevateSecurityBars();
	
	UFUNCTION()
	void GetElevation(float Value);

};
