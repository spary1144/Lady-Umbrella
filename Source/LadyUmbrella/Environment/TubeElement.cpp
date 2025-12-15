// 
// TubeElement.cpp
// 
// Implements the Tube Puzzle class 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "TubeElement.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Sound/SoundCue.h" 

#define TUBE_EXPLOSION_ASSET TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_GrenadeExplosion.NS_GrenadeExplosion'")
#define TUBE_EXPLOSION_SOUND TEXT("/Script/Engine.SoundWave'/Game/Sounds/SoundWaves/SW_Explode02'")

ATubeElement::ATubeElement()
{
	PrimaryActorTick.bCanEverTick = true;

	NS_Explosion = LoadObject<UNiagaraSystem>(nullptr, TUBE_EXPLOSION_ASSET, nullptr, LOAD_None, nullptr);

	ExplosionAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Explosion Sound Component"));
	ExplosionAudioComponent->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USoundCue> ExplosionCueFinder(TEXT("/Script/Engine.SoundCue'/Game/Sounds/SoundCues/SC_GrenadeExplosion.SC_GrenadeExplosion'"));
	if (ExplosionCueFinder.Succeeded())
	{
		ExplosionAudioComponent->SetSound(ExplosionCueFinder.Object);
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder'"));
	if (MeshAsset.Object)
	{
		ObjectMesh->SetStaticMesh(MeshAsset.Object);
	}
	
	CanRespawn = false;
	
	ParticleSpawnerComponent = CreateDefaultSubobject<UParticleSpawnerComponent>(TEXT("ParticleSpawnerComponent"));
	
	ConstructorHelpers::FClassFinder<AActor> BPActor(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/LevelElements/Puzzle/Electric/BP_ElectricPuddle.BP_ElectricPuddle_C'"));
	if (BPActor.Succeeded())
	{
		BlueprintActorClass = BPActor.Class;
	}
	
	SpawnType = ESpawnType::SMOKE;
	HittableBy.Add(HitSource::PLAYER_B);
	HittableBy.Add(HitSource::PLAYER_G);

	MeshRotation = FRotator(-90.f, 0.f, 0.f);
	CollisionOffset = FVector(0.f, 0.f, 50.f);
	MeshScale = FVector(0.2f, 0.2f, 2.0f);
	BoxExtent = FVector(70.f, 70.f, 50.f);
	SmokeTraceRadius = 500.f;
	SmokeSpawnRadius = 250.f;
	SmokeParticleCount = 4;
	PuddleTraceDepth = 1000.f;
	PuddleFallbackZOffset = -90.f;
	EnemyStunDuration = 2.0f;


	//ObjectMesh->SetRelativeTransform(FTransform(MeshRotation, MeshOffset, MeshScale));
	BoxRoot->SetBoxExtent(BoxExtent);
}

void ATubeElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ObjectMesh)
	{
		ObjectMesh->SetRelativeRotation(MeshRotation);
		ObjectMesh->SetRelativeScale3D(MeshScale);
	}

	if (BoxRoot)
	{
		BoxRoot->SetRelativeLocation(CollisionOffset);
		BoxRoot->SetBoxExtent(BoxExtent);
	}
}

void ATubeElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ExplosionAudioComponent->Stop();
}


void ATubeElement::Reaction(HitSource Source) 
{
	if (Super::ShouldReact(Source)){
		
		if (NS_Explosion)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_Explosion, GetActorLocation());
		}

		if (ExplosionAudioComponent)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionAudioComponent->Sound, GetActorLocation());
		}
		
		if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent())){
			if(!bUseCustomShakeParams)
			{
				FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::ENVIRONMENT, GetActorLocation());
				
			}
			else
			{
				FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::ENVIRONMENT, GetActorLocation(), InOutRadiusParams.X, InOutRadiusParams.Y);
			}
		}

		switch (SpawnType)
		{
			case ESpawnType::SMOKE: SpawnSmoke(); break;
			case ESpawnType::PUDDLE: SpawnPuddle();break;
			case ESpawnType::NONE: break;
		}
		
		if (CanRespawn)
		{
			ObjectMesh->SetVisibility(false);
			ObjectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			BoxRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			Destroy();
		}
	}
}

void ATubeElement::LoadDataCheckPoint()
{
	if (CanRespawn)
	{
		ObjectMesh->SetVisibility(true);
		BoxRoot->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ObjectMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Reacted = false;
	}
}


void ATubeElement::SpawnSmoke()
{
	FVector TubeLocation = BoxRoot->GetComponentLocation();
	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		TubeLocation,
		TubeLocation, 
		FQuat::Identity, 
		ECC_Visibility,
		FCollisionShape::MakeSphere(SmokeTraceRadius), 
		CollisionParams
	);
	
	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			if (AEnemyCharacter* EnemyActor = Cast<AEnemyCharacter>(HitResult.GetActor())) // use any other class
			{
				UE_LOG(LogTemp, Warning, TEXT("Detected Actor: %s"), *EnemyActor->GetName());
			}
		}
	}

	float AngleStep = 360.f / SmokeParticleCount;

	for (int32 i = 0; i < SmokeParticleCount; ++i)
	{
		float Angle = FMath::DegreesToRadians(i * AngleStep);
		FVector Offset = FVector(FMath::Cos(Angle) * SmokeSpawnRadius, FMath::Sin(Angle) * SmokeSpawnRadius, 0);
		FVector SpawnLocation = TubeLocation + Offset;

		if (IsValid(ParticleSpawnerComponent))
		{
			ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::SmokeExplosion,SpawnLocation, FRotator::ZeroRotator);
		}
	}
}

void ATubeElement::SpawnPuddle()
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -PuddleTraceDepth);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;
	FVector SpawnLocation = Start + FVector(0.f, 0.f, PuddleFallbackZOffset);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		SpawnLocation = HitResult.ImpactPoint;
	}
	
	if (BlueprintActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		GetWorld()->SpawnActor<AActor>(BlueprintActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}

void ATubeElement::ContactWithCable(FVector& Direction)
{
	Reaction(HitSource::PLAYER_G);
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerPawn))
	{
		PlayerCharacter->GetWeapon()->ResetGrapple();
	}
}

IHookInterface::GrabType ATubeElement::GetGrabType()
{
	return PULL;
}


	