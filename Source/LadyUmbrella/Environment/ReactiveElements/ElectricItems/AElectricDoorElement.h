// 
// AElectricDoorElement.cpp
// 
// Definition of the AElectricDoorElement class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "ElectricElement.h"
#include "AElectricDoorElement.generated.h"


UCLASS()
class LADYUMBRELLA_API AElectricDoorElement : public AElectricElement
{
	GENERATED_BODY()
public:
	AElectricDoorElement();
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void OpenDoors();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Pillar1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Pillar2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	FVector PillarScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	FVector DoorScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	FVector Pillar1Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	FVector Pillar2Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	FVector Door1Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	FVector Door2Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	float DoorOpeningAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Object", meta = (AllowPrivateAccess = "true"))
	float DoorOpenSpeed;


private:
	FTimerHandle DoorOpenTimerHandle;

	FRotator InitialRotPillar1;
	FRotator InitialRotPillar2;
	FRotator TargetRotPillar1;
	FRotator TargetRotPillar2;
	
	float DoorOpenAlpha;
	float AlphaTarget;

	void UpdateDoorRotation() const;
	virtual void PerformElectrification() override;
};