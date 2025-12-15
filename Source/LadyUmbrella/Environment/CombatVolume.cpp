// 
// CombatVolume.h
// 
// Implementation of the CombatVolume class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "CombatVolume.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/InteractiveMovementComponent/InteractiveMovementComponent.h"

ACombatVolume::ACombatVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Cube1CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetBoxExtent(BoxExtent);
}

void ACombatVolume::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACombatVolume::OnOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ACombatVolume::OffOverlap);
}

void ACombatVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACombatVolume::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		PlayerCharacter->SetIsInCombatVolume(true);
		PlayerCharacter->GetInteractiveMovementComponent()->SetStrafing(true);

		if (UCharacterMovementComponent* UCharacterMovementComponent = PlayerCharacter->GetCharacterMovement())
		{
			UCharacterMovementComponent->bOrientRotationToMovement = false;
			UCharacterMovementComponent->bUseControllerDesiredRotation = true;
		}
	}
}

void ACombatVolume::OffOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		PlayerCharacter->SetIsInCombatVolume(false);

		if (!PlayerCharacter->IsAiming())
		{
			PlayerCharacter->ResetAiming();
		}
	}
}

