#include "ElectricProjectile.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Weapons/GenericProjectile.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"


AElectricProjectile::AElectricProjectile() :
	DamageToUmbrella { 20 },
	SphereSizeMultiplierPerSecond { 3.f },
	ParticleSphereGrownSizeMultiplier { 2.0f },
	ProjectileLifespan { 3.0f },
	ElectrificationDuration { 2.5f },
	TimeAlive { 0.f }
{
	ProjectileParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileParticleComponent"));
	ProjectileParticleComponent->SetAutoActivate(false);
	
	if (IsValid(GetProjectileMovementComponent()))
	{
		GetProjectileMovementComponent()->bShouldBounce = false;
		GetProjectileMovementComponent()->Bounciness = 0.0f;
		GetProjectileMovementComponent()->ProjectileGravityScale = 0.2f;
	}

	GetSphereComponent()->InitSphereRadius(1.0f);
	// GetSphereComponent()->SetWorldScale3D(PARTICLE_SPHERE_INITIAL_SIZE * ParticleSphereGrownSizeMultiplier);
	GetSphereComponent()->SetWorldScale3D(FVector::OneVector);
	GetSphereComponent()->SetSphereRadius(10.0f);
	GetSphereComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetSphereComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetSphereComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	GetSphereComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	GetSphereComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AElectricProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(ProjectileParticleComponent))
	{
		ProjectileParticleComponent->AttachToComponent(GetSphereComponent().Get(), FAttachmentTransformRules::KeepRelativeTransform);
		ProjectileParticleComponent->SetRelativeLocation(FVector::Zero());
		ProjectileParticleComponent->SetRelativeRotation(FRotator::ZeroRotator);
		ProjectileParticleComponent->SetRelativeScale3D(FVector::One());
		
		if (IsValid(GetProjectileMovementComponent()))
		{
			ProjectileParticleComponent->SetVariableVec3(PARTICLE_VELOCITY_NIAGARA_PARAMETER_NAME, GetProjectileMovementComponent()->Velocity);
		}
		
		// ProjectileParticleComponent->SetVariableFloat(PARTICLE_SPHERE_SIZE_MULTIPLIER_PER_SECOND_NIAGARA_PARAMETER_NAME, SphereSizeMultiplierPerSecond);
		// ProjectileParticleComponent->SetVariableFloat(PARTICLE_SPHERE_GROWN_SIZE_MULTIPLIER_NIAGARA_PARAMETER_NAME, ParticleSphereGrownSizeMultiplier);
		ProjectileParticleComponent->SetVariableFloat(PARTICLE_MAX_LIFE_NIAGARA_PARAMETER_NAME, ProjectileLifespan);
		ProjectileParticleComponent->ActivateSystem();
	}

	SetLifeSpan(ProjectileLifespan);
}

void AElectricProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TimeAlive += DeltaSeconds;

	// if (GetSphereComponent().IsValid())
	// {
	// 	 GetSphereComponent()->SetWorldScale3D(FMath::Lerp(PARTICLE_SPHERE_INITIAL_SIZE * ParticleSphereGrownSizeMultiplier,
	// 	 												  PARTICLE_SPHERE_INITIAL_SIZE * ParticleSphereGrownSizeMultiplier * SphereSizeMultiplierPerSecond,
	// 	 												  TimeAlive));
	// }
	
	if (IsValid(ProjectileParticleComponent) && IsValid(GetProjectileMovementComponent()))
	{
		ProjectileParticleComponent->SetVariableVec3(PARTICLE_VELOCITY_NIAGARA_PARAMETER_NAME, GetProjectileMovementComponent()->Velocity);
	}
}

void AElectricProjectile::ProcessCollision(UPrimitiveComponent* OverlappedComponent, 
                                           AActor* OtherActor, 
                                           UPrimitiveComponent* OtherComp, 
                                           int32 OtherBodyIndex, 
                                           bool bFromSweep, 
                                           const FHitResult& SweepResult)
{	
	if (!IsValid(OtherActor) || GetInstigator() == OtherActor)
	{
		return;
	}
	
	if (OtherActor->ActorHasTag(FName("AgencyShield")))
	{
		this->Destroy();
	}

	// Hitting the Umbrella.
	if (OtherActor->IsA(AUmbrella::StaticClass()) && Owner.IsA(AEnemyCharacter::StaticClass()))
	{
		Cast<AUmbrella>(OtherActor)->SubtractHealth(DamageToUmbrella,EWeaponType::ElectricProjectil);
		this->Destroy();
	}

	// Hitting a Character.
	AGenericCharacter* GenericCharacter = Cast<AGenericCharacter>(OtherActor);
	if (IsValid(GenericCharacter))
	{
		this->Destroy();

		UStatusEffectComponent* StatusEffectComponent = GenericCharacter->GetStatusEffectComponent();
		
		if (!IsValid(StatusEffectComponent))
		{
			FLogger::ErrorLog("Invalid StatusEffectComponent pointer in ElectricProjectile.ProcessCollision()");
			return;
		}
		
		StatusEffectComponent->ApplyElectricityEffect(ElectrificationDuration);

		TObjectPtr<AEnemyCharacter> Enemy = Cast<AEnemyCharacter>(OtherActor);
		
		if (IsValid(Enemy) && !Enemy->HasBeenShockedBefore())
		{
			Enemy->SetHasBeenShockedBefore(true);
			OnShockUniqueEnemy.ExecuteIfBound();
		}
	}

	Super::ProcessCollision(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AElectricProjectile::ProcessHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ProcessHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	if (OtherActor->IsA(AUmbrella::StaticClass()))
	{
		this->Destroy();
		if (!IsValid(FGlobalPointers::Umbrella))
		{
			return;
		}
		FGlobalPointers::Umbrella->SubtractHealth(DamageToUmbrella,EWeaponType::ElectricProjectil);
	}
}
