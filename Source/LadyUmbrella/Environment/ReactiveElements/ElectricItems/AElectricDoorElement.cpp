// 
// AElectricDoorElement.cpp
// 
// Implementation of the AElectricDoorElement class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "AElectricDoorElement.h"
#include "Components/BoxComponent.h"

AElectricDoorElement::AElectricDoorElement()
{
    ObjectMesh->SetVisibility(false);
    ObjectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    Pillar1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pillar1"));
    Pillar2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pillar2"));

    ObjectMesh3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door1"));
    ObjectMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door2"));

    // Attach pillars to root
    Pillar1->SetupAttachment(RootComponent);
    Pillar2->SetupAttachment(RootComponent);

    // Attach doors to their respective pillars
    ObjectMesh3->SetupAttachment(Pillar1);
    ObjectMesh2->SetupAttachment(Pillar2);

    // Load cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Game/Meshes/StaticMeshes/Placeholder/Primitives/Shape_Cube.Shape_Cube"));
    if (CubeMeshAsset.Succeeded())
    {
       Pillar1->SetStaticMesh(CubeMeshAsset.Object);
       Pillar2->SetStaticMesh(CubeMeshAsset.Object);
       ObjectMesh3->SetStaticMesh(CubeMeshAsset.Object);
       ObjectMesh2->SetStaticMesh(CubeMeshAsset.Object);
    }

    
    PillarScale = FVector(0.1f, 0.1f, 2.f);
    DoorScale = FVector(10.f, 0.7f, 1.f);
    Pillar1Location = FVector(-100.f, 0.f, -100.f);
    Pillar2Location = FVector(100.f, 0.f, -100.f);
    Door1Location = FVector(450.f, 0.f, 0.f);
    Door2Location = FVector(-550.f, 0.f, 0.f);
    DoorOpeningAngle = 90.0f;
    // Set scale
    Pillar1->SetRelativeScale3D(PillarScale);
    Pillar2->SetRelativeScale3D(PillarScale);
    ObjectMesh3->SetRelativeScale3D(DoorScale); 
    ObjectMesh2->SetRelativeScale3D(DoorScale);

    // Position pillars
    Pillar1->SetRelativeLocation(Pillar1Location);
    Pillar2->SetRelativeLocation(Pillar2Location);

    ObjectMesh3->SetRelativeLocation(Door1Location);
    ObjectMesh2->SetRelativeLocation(Door2Location);


    BoxRoot->SetBoxExtent(FVector(0.0f, 0.0f, 0.0f));
    PrimaryActorTick.bCanEverTick = true;
    DoorOpenAlpha = 1.f;
    DoorOpenSpeed = 1.f;
    AlphaTarget = 1.f;

    bShowSparkEffect = false;

    AActor::SetActorTickEnabled(false);

}

void AElectricDoorElement::OnConstruction(const FTransform &Transform)
{
    Super::OnConstruction(Transform);
    // Set scale
    Pillar1->SetRelativeScale3D(PillarScale);
    Pillar2->SetRelativeScale3D(PillarScale);
    ObjectMesh3->SetRelativeScale3D(DoorScale); 
    ObjectMesh2->SetRelativeScale3D(DoorScale);

    // Position pillars
    Pillar1->SetRelativeLocation(Pillar1Location);
    Pillar2->SetRelativeLocation(Pillar2Location);

    ObjectMesh3->SetRelativeLocation(Door1Location);
    ObjectMesh2->SetRelativeLocation(Door2Location);
}

void AElectricDoorElement::BeginPlay()
{
    Super::BeginPlay();
    AActor::SetActorTickEnabled(false);
    InitialRotPillar1 = Pillar1->GetRelativeRotation();
    InitialRotPillar2 = Pillar2->GetRelativeRotation();

    TargetRotPillar1 = InitialRotPillar1 + FRotator(0.f, -1*DoorOpeningAngle, 0.f);
    TargetRotPillar2 = InitialRotPillar2 + FRotator(0.f, DoorOpeningAngle, 0.f);

    Pillar1->SetRelativeRotation(InitialRotPillar1);
    Pillar2->SetRelativeRotation(InitialRotPillar2);
}

void AElectricDoorElement::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (DoorOpenAlpha < AlphaTarget)
    {
       DoorOpenAlpha += DeltaTime * DoorOpenSpeed;
       DoorOpenAlpha = FMath::Clamp(DoorOpenAlpha, 0.f, 1.f);

       UpdateDoorRotation();
    }
}

void AElectricDoorElement::OpenDoors()
{
    DoorOpenAlpha = 0.f;
    SetActorTickEnabled(true);
}

void AElectricDoorElement::UpdateDoorRotation() const
{
    FRotator NewRot1 = FMath::Lerp(InitialRotPillar1, TargetRotPillar1, DoorOpenAlpha);
    FRotator NewRot2 = FMath::Lerp(InitialRotPillar2, TargetRotPillar2, DoorOpenAlpha);

    Pillar1->SetRelativeRotation(NewRot1);
    Pillar2->SetRelativeRotation(NewRot2);
}

void AElectricDoorElement::PerformElectrification()
{
    //Super::PerformElectrification();
    OnElectrified(); 
    OpenDoors();
}