#include "ElectricPlatform.h"

AElectricPlatform::AElectricPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	ObjectMesh->SetupAttachment(RootComp);
	MoveSpeed = 400.0f;
	MinTargetThreshold = 0.1f;
	StartLocation = FVector::ZeroVector;
	EndLocation = FVector::ZeroVector;
	MeshScale = FVector(1.f, 1.0f, 0.1f);
	bShowSparkEffect = false;
}

void AElectricPlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
}

void AElectricPlatform::PerformElectrification()
{
	Super::PerformElectrification();

	TargetLocation = StartLocation + EndLocation;
	bIsMoving = true;
}

void AElectricPlatform::ResetElectrifiedState()
{
	Super::ResetElectrifiedState();

	TargetLocation = StartLocation;
	bIsMoving = true;
}

void AElectricPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

		SetActorLocation(NewLocation);

		// Stop movement when close enough
		if (FVector::DistSquared(NewLocation, TargetLocation) < MinTargetThreshold)
		{
			bIsMoving = false;
		}
	}
}
