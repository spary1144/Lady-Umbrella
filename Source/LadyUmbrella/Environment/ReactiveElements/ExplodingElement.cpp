	// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplodingElement.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h" 
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"

#define GRENADE_EXPLOSION_ASSET TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_GrenadeExplosion.NS_GrenadeExplosion'")
#define GRENADE_EXPLOSION_SOUND TEXT("/Script/Engine.SoundWave'/Game/Sounds/SoundWaves/Explosions/SW_Explode02.SW_Explode02'")

// Sets default values
AExplodingElement::AExplodingElement()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ExplosionDamage = 20.f;
	ExplosionRadius = 200.f;
	
	NS_Explosion = LoadObject<UNiagaraSystem>(nullptr, GRENADE_EXPLOSION_ASSET, nullptr, LOAD_None, nullptr);

	ExplosionAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Explosion Sound Component"));
	ExplosionAudioComponent->SetupAttachment(RootComponent);
	
	ConstructorHelpers::FObjectFinder<USoundCue> ExplosionCueFinder(TEXT("/Script/Engine.SoundCue'/Game/Sounds/SoundCues/SC_GrenadeExplosion.SC_GrenadeExplosion'"));
	if (ExplosionCueFinder.Succeeded())
	{
		ExplosionAudioComponent->SetSound(ExplosionCueFinder.Object);
	}

	HittableBy.Add(HitSource::PLAYER_B);
	HittableBy.Add(HitSource::ENEMY);
	HittableBy.Add(HitSource::EXPLOSION);

#if WITH_EDITOR
	Visualizer = CreateDefaultSubobject<UShapeVisualizer>("Radius Visualizer");
	Visualizer->SetupAttachment(RootComponent);
	Visualizer->Shape = FShapeInformation(EShapeToVis::SPHERE, FVector::ZeroVector, ExplosionRadius, FColor::Red);
#endif
	
	CanRespawn = false;
}

// Called every frame
void AExplodingElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ExplosionAudioComponent->Stop();
}

#if WITH_EDITOR

void AExplodingElement::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{

	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetNameCPP() == "ExplosionRadius")
	{
		Visualizer->Shape.Radius = ExplosionRadius;
		Visualizer->MarkRenderStateDirty();
	}
}

void AExplodingElement::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	Visualizer->Shape.Radius = ExplosionRadius;
	Visualizer->MarkRenderStateDirty();
}

void AExplodingElement::PostEditUndo()
{
	Super::PostEditUndo();
	Visualizer->Shape.Radius = ExplosionRadius;
	Visualizer->MarkRenderStateDirty();
}

#endif

void AExplodingElement::Reaction(HitSource Source) 
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
		
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation();
		TArray<AActor*> ActorsToIgnore;
		TArray<FHitResult> HitResults;

		UKismetSystemLibrary::SphereTraceMulti
		(
			GetWorld(),
			Start,
			End,
			ExplosionRadius,
			UEngineTypes::ConvertToTraceType(ECC_Pawn),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResults,
			true,
			FLinearColor::White,
			FLinearColor::Red,
			2.f
		);

		if (HitResults.Num() > 0)
		{
			TArray<AActor*> DamagedActors;
			for (auto Result : HitResults)
			{
				if (!DamagedActors.Contains(Result.GetActor()))
				{
					
					if (Result.GetActor()->Implements<UHealthInterface>())
					{
						if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(Result.GetActor()))
						{
							HealthInterface->SubtractHealth(ExplosionDamage,EWeaponType::ExplosiveBarrier);
							DamagedActors.Add(Result.GetActor());
						}
					}
					else if (Result.GetActor()->GetClass()->ImplementsInterface(UHitReactiveInterface::StaticClass()))
					{
						Cast<AReactiveElement>(Result.GetActor())->Reaction(HitSource::EXPLOSION);
						DamagedActors.Add(Result.GetActor());
					}
				}
			}
		}
		// Instead of destroying, set barrel invisible
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

void AExplodingElement::LoadDataCheckPoint()
{
	// This if not needed?
	if (CanRespawn)
	{
		ObjectMesh->SetVisibility(true);
		BoxRoot->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ObjectMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Reacted = false;
	}
}
