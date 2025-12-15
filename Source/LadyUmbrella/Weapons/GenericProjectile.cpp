// 
// GenericProjectile.cpp
// 
// Implementation of the GenericProjectile class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "GenericProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

AGenericProjectile::AGenericProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileDamage = 10.0f;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGenericProjectile::ProcessCollision);
	SphereComponent->OnComponentHit.AddDynamic(this, &AGenericProjectile::ProcessHit);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent.Get());
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce            = true;
	ProjectileMovementComponent->Bounciness               = 0.3f;
	ProjectileMovementComponent->InitialSpeed             = 3000.0f;
	ProjectileMovementComponent->MaxSpeed                 = 3000.0f;
	ProjectileMovementComponent->ProjectileGravityScale   = 0.0f;
}

void AGenericProjectile::ProcessCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Destroy();
}

void AGenericProjectile::ProcessHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AGenericProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AGenericProjectile::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}
