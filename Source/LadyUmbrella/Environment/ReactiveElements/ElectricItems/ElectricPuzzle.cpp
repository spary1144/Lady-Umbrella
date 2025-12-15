// This is the little box generator


#include "ElectricPuzzle.h"
#include "LadyUmbrella/Weapons/Umbrella/Gadgets/ElectricProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AElectricPuzzle::AElectricPuzzle()
{
	
	PrimaryActorTick.bCanEverTick = true;
	// Mesh setup
	
	GeneratorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GeneratorMesh"));
	GeneratorMesh->SetupAttachment(RootComp);
	LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightMesh"));
	LightMesh->SetupAttachment(RootComp);
	BoxRoot->SetupAttachment(RootComp);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/Cube1.Cube1"));
	if (CubeMesh.Succeeded())
	{
		GeneratorMesh->SetStaticMesh(CubeMesh.Object);
		LightMesh->SetStaticMesh(CubeMesh.Object);
	}
	
	// Load the material for the LightMesh
	static ConstructorHelpers::FObjectFinder<UMaterial> LightMaterial(TEXT("/Game/Materials/Puzzles/M_LightOnOff.M_LightOnOff"));
	if (LightMaterial.Succeeded())
	{
		LightMesh->SetMaterial(0, LightMaterial.Object);
	}

	MeshPosition = FVector(0.f, 0.f, -50.0f);
	MeshScale = FVector(1.f, 1.f, 1.f);
	LightScale = FVector(0.2f, 0.2f, 0.2f);
	CollisionExtent = FVector(60.f,60.f,60.f);

	BoxRoot->SetBoxExtent(CollisionExtent);
	bShowSparkEffect = false;
}

void AElectricPuzzle::BeginPlay()
{
	Super::BeginPlay();

	// Setup dynamic material instance for the light mesh
	if (IsValid(LightMesh))
	{
		GeneratorMaterialInstance = UMaterialInstanceDynamic::Create(LightMesh->GetMaterial(0), this);
		LightMesh->SetMaterial(0, GeneratorMaterialInstance);
	}

	if (IsValid(BoxRoot))
	{
		if (!BoxRoot->OnComponentBeginOverlap.IsAlreadyBound(this, &AElectricPuzzle::OnOverlap))
		{
			BoxRoot->OnComponentBeginOverlap.AddDynamic(this, &AElectricPuzzle::OnOverlap);
		}
	}
}

void AElectricPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectricPuzzle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


void AElectricPuzzle::SetElectrified(bool bNewElectrified)
{
	Super::SetElectrified(bNewElectrified);
	if (IsValid(GeneratorMaterialInstance))
	{
		GeneratorMaterialInstance->SetVectorParameterValue("Color", bElectrified ? FLinearColor(0.f, 1.f, 0.f) : FLinearColor(1.f, 0.f, 0.f));
	}

	if (bElectrified)
	{
		GetWorld()->GetTimerManager().SetTimer(ElectrifiedTimerHandle, this, &AElectricPuzzle::ResetElectrifiedState, ElectrificationDuration, false);
		
	}
}

void AElectricPuzzle::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AElectricProjectile::StaticClass()) && (OverlappedComp == BoxRoot))
		Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AElectricPuzzle::ResetElectrifiedState()
{
	Super::ResetElectrifiedState();
	SetElectrified(false);
}