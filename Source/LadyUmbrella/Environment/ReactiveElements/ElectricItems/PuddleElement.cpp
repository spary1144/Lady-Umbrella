#include "PuddleElement.h"
#include "Components/BoxComponent.h"

APuddleElement::APuddleElement()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshScale = FVector(1.f, 1.f, 0.1f);
	ObjectMesh->SetupAttachment(RootComp);
	ConstructorHelpers::FObjectFinder<UStaticMesh> ObjectMeshAsset(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder"));
	if (ObjectMeshAsset.Succeeded())
	{
		ObjectMesh->SetStaticMesh(ObjectMeshAsset.Object);
	}
	if (IsValid(BoxRoot))
	{
		BoxRoot->SetBoxExtent(FVector(50.f, 50.f, 100.f));
	}
}

void APuddleElement::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BoxRoot))
	{
		BoxRoot->OnComponentBeginOverlap.AddDynamic(this, &APuddleElement::OnOverlap);
	}
}

void APuddleElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APuddleElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void APuddleElement::PerformElectrification()
{
	Super::PerformElectrification();
}

void APuddleElement::ResetElectrifiedState()
{
	Super::ResetElectrifiedState();
	SetElectrified(false);
}

void APuddleElement::OnOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);
}