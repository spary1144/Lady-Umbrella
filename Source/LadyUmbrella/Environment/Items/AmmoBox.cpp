#include "AmmoBox.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/PickableObjects/Ammo.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AAmmoBox::AAmmoBox()
{
	PrimaryActorTick.bCanEverTick = true;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	ScaleVector = FVector(1.0f, 1.0f, 1.0f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cube.SM_Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");

	ZOffset = FVector(0.0f, 0.0f, 50.0f);
}

void AAmmoBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CubeMesh->SetWorldScale3D(ScaleVector);
	CollisionBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));
}

void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind to the OnDeathDelegate
	if (HealthComponent)
	{
		HealthComponent->SetHealth(100.0f);
		HealthComponent->OnDeathDelegate.AddDynamic(this, &AAmmoBox::OnDeath);
	}
}

// Called every frame
void AAmmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmoBox::OnDeath()
{
	float RandomValue = FMath::RandRange(0.0f, 100.0f);
	if (RandomValue <= DropRate)
	{
		FVector SpawnLocation = GetActorLocation() + ZOffset;
		FRotator SpawnRotation = FRotator::ZeroRotator;
		if (const AAmmo* SpawnedAmmo = GetWorld()->SpawnActor<AAmmo>(AAmmo::StaticClass(), SpawnLocation, SpawnRotation))
		{
			if (IsValid(PickupWidget))
			{
				// SpawnedAmmo->GetPickupWidgetComponent()->SetWidgetClass(PickupWidget);
			}
		}
	}
	Destroy();
}

#pragma region HEALTH INTERFACE FUNCTIONS IMPLEMENTATION

void AAmmoBox::SubtractHealth(const float _HealthToSubtract,enum EWeaponType DamageType)
{
	HealthComponent->SubtractHealth(_HealthToSubtract);
}

void AAmmoBox::SetHealth(const float _NewHealth)
{
	HealthComponent->SetHealth(_NewHealth);
}

const float AAmmoBox::GetHealth() const
{
	return HealthComponent->GetHealth();	
}

#pragma endregion
