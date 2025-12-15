// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricFenceElement.h"
#include "Components/BoxComponent.h"

AElectricFenceElement::AElectricFenceElement()
{
	// Change the mesh to a cube
	ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Game/Meshes/StaticMeshes/Placeholder/Primitives/Shape_Cube.Shape_Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		ObjectMesh->SetStaticMesh(CubeMeshAsset.Object);
	}
	ObjectMesh->SetupAttachment(RootComp);
	MeshScale = FVector(1.f, 0.1f, 2.f);
	if (IsValid(BoxRoot))
	{
		BoxRoot->SetBoxExtent(FVector(100.f, 75.f, 50.f));
	}
}

void AElectricFenceElement::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BoxRoot))
	{
		BoxRoot->OnComponentBeginOverlap.AddDynamic(this, &AElectricFenceElement::OnOverlap);
	}
}

void AElectricFenceElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AElectricFenceElement::PerformElectrification()
{
	Super::PerformElectrification();
}

void AElectricFenceElement::ResetElectrifiedState()
{
	Super::ResetElectrifiedState();
	SetElectrified(false);
}

void AElectricFenceElement::OnOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);
}