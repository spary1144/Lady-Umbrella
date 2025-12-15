// 
// Ammo.cpp
// 
// Implementation of the Ammo class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Ammo.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "LadyUmbrella/UI/Basics/Icon/InteractiveIcon.h"

#define M_AMMO TEXT("/Script/Engine.MaterialInstance'/Game/Materials/Instancias/Editor/ReactiveElements/MI_LDGrid_Local_Interactable.MI_LDGrid_Local_Interactable'")
#define SM_AMMO TEXT("StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cube.SM_Cube'")
#define WG_PROMPT TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons'")

//TODO: Remove Umbrella Reference from this class but one thing at a time @PALEXUTAD
AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterialInstance>(M_AMMO).Object);
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(SM_AMMO).Object);
	StaticMeshComponent->SetupAttachment(GetSphereComponent());
	StaticMeshComponent->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	ConstructorHelpers::FClassFinder<UInteractiveIcon> WidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Widgets/WidgetBlueprints/Components/WBP_InteractiveIcons.WBP_InteractiveIcons_C'"));

	if (WidgetFinder.Succeeded())
	{
		SetWidgetClass(WidgetFinder.Class);
	}

	UmbrellaBindRetryCount = 0;
	MaxUmbrellaBindRetries = 10;
	SetAmountToPickup(5);

	TypeOfPickable = false;
	
}

void AAmmo::OnPlayerAmmoChanged()
{
	Umbrella = PlayerCharacter->GetWeapon();
	if (Umbrella)
	{
		SetCanInteract(!Umbrella->IsAmmoReserveAtMaxValue());
	}
}

void
AAmmo::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AAmmo::TryBindToUmbrella);
}

void AAmmo::TryBindToUmbrella()
{
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter) return;

	Umbrella = PlayerCharacter->GetWeapon();
	if (Umbrella)
	{
		if (!Umbrella->OnMultiCurrentBulletsChanged.IsAlreadyBound(this, &AAmmo::OnPlayerAmmoChanged))
		{
			UE_LOG(LogTemp, Display, TEXT("Bound to umbrella: %s"), *Umbrella->GetName());
			Umbrella->OnMultiCurrentBulletsChanged.AddDynamic(this, &AAmmo::OnPlayerAmmoChanged);
		}
		OnPlayerAmmoChanged();
	}
	else
	{
		UmbrellaBindRetryCount++;
		if (UmbrellaBindRetryCount < MaxUmbrellaBindRetries)
		{
			UE_LOG(LogTemp, Warning, TEXT("Umbrella still null, retrying. (%d/%d)"), UmbrellaBindRetryCount, MaxUmbrellaBindRetries);
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AAmmo::TryBindToUmbrella);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to bind to umbrella after %d attempts."), MaxUmbrellaBindRetries);
		}
	}
}


void
AAmmo::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32
AAmmo::Interacting()
{
	Super::Interacting();

	if (!IsValid(PlayerCharacter))
	{
		// FLogger::ErrorLog("Could not find the PlayerCharacter instance");
		return 0;
	}
	
	Umbrella = PlayerCharacter->GetWeapon();

	if (!IsValid(Umbrella))
	{
		// FLogger::ErrorLog("Could not find the Umbrella instance in Ammo.Tick()");
		return 0;
	}	

	if (!CanInteract())
	{
		return 0;
	}
	
	const float MissingReserve = Umbrella->GetAmmoReserveMax() - Umbrella->GetAmmoReserve();

	if (GetAmountToPickup() > MissingReserve)
	{
		SetAmountToPickup(MissingReserve);
	}
	Umbrella->SetAmmoReserve(Umbrella->GetAmmoReserve() + GetAmountToPickup());
	(void) Umbrella->OnTotalBulletsChanged.ExecuteIfBound(Umbrella->GetAmmoReserve());
	(void) Umbrella->OnCurrentBulletsChanged.ExecuteIfBound(FGlobalPointers::PlayerCharacter->IsReloading(), Umbrella->GetAmmoCurrent(), Umbrella->GetAmmoCurrentMax(), Umbrella->GetAmmoReserve());

	SetActorHiddenInGame(true);
	
	GetPickUpSoundComponent()->SetParameter(PickUpEventParameterName, TypeOfPickable);
	GetPickUpSoundComponent()->PlayEvent();
	
	SetCanInteract(false);
	GetWorldTimerManager().SetTimer(PickUpTimerHandle, this, &AGenericPickable::DestroyInteractable, 2.f, false);
	
	return GetAmountToPickup();
}

int32 AAmmo::Interacting(APlayerCharacter* Interactor)
{
	Super::Interacting(Interactor);
	
	return 0;
}
