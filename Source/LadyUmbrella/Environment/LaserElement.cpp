#include "LaserElement.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

ALaserElement::ALaserElement()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	LaserBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserBaseMesh"));
	LaserBaseMesh->SetupAttachment(RootComponent);
	LaserBaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserBaseMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	LaserBaseMesh->SetRelativeLocation(FVector::ZeroVector);
	LaserBaseMesh->SetRelativeRotation(FRotator(90.0f, 90.0f, 90.0f));

	DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
	DetectionBox->SetupAttachment(LaserBaseMesh);
	DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ALaserElement::OnPlayerDetected);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cylinder.SM_Cylinder"));
	if (CubeMesh.Succeeded())
	{
		LaserBaseMesh->SetStaticMesh(CubeMesh.Object);
		LaserBaseMesh->SetWorldScale3D(FVector(0.5f));
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LaserNiagaraSystemFinder(TEXT("/Game/Particles/NiagaraSystems/NS_Beam.NS_Beam"));
	if (LaserNiagaraSystemFinder.Succeeded())
	{
		LaserNiagaraSystem = LaserNiagaraSystemFinder.Object;
	}
	LaserDamage = 1.0f;
	LaserRange = 10000.0f;
	BoxLocation = FVector(0.0f, 0.0f, -500.0f);
	BoxExtent = FVector(100.0f, 100.0f, 500.0f);
	CurrentState = ELaserState::Deactivated;

	ImpactLasersFModComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Laser Impact Sound Component Interface");
	
	UFMODEvent* LaserImpactEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/LASERS_ON_UMBRELLA.LASERS_ON_UMBRELLA'");
	FmodUtils::TryAddEvent(LaserImpactEvent, ImpactLasersFModComponentInterface);
	
	FmodLaserComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Laser Sound Component Interface");

	UFMODEvent* LaserEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/LASERS.LASERS'");
	FmodUtils::TryAddEvent(LaserEvent, FmodLaserComponentInterface);
	
}

void ALaserElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (DetectionBox)
	{
		DetectionBox->SetRelativeLocation(BoxLocation);
		DetectionBox->SetBoxExtent(BoxExtent);
	}
}

void ALaserElement::BeginPlay()
{
	Super::BeginPlay();
	
	// Design Wanted a constantly firing laser, so ActivateLaser on begin play
	// ActivateLaser();
}

void ALaserElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == ELaserState::Activated)
	{
		FireLaser();
	}
}

void ALaserElement::OnPlayerDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Design Wanted a constantly firing laser, so logic for the detection can be ignored
	// Not deleted in case they want the detection back
	//if (OtherActor && OtherActor->IsA<APlayerCharacter>())
	//{
		//if (CurrentState == ELaserState::Deactivated)
		//{
		//	ActivateLaser();
		//}
	//}
}

void ALaserElement::ActivateLaser()
{
	CurrentState = ELaserState::Activated;
	FmodLaserComponentInterface->PlayEvent();
	
	// Design Wanted a constantly firing laser, so logic for the detection can be ignored
	// Not deleted in case they want the detection back
	//GetWorld()->GetTimerManager().SetTimer(DeactivateTimerHandle, this, &ALaserElement::DeactivateLaser, 2.0f, false);
}

void ALaserElement::DeactivateLaser()
{
	CurrentState = ELaserState::Deactivated;
	FmodLaserComponentInterface->StopEvent();
}

void ALaserElement::FireLaser() const
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	
	const FVector Start		= GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	FVector End				= Start + (Rotation.Vector() * LaserRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, CollisionParams);

	FColor TraceColor = bHit ? FColor::Red : FColor::Green;

	if (bHit)
	{
		End = HitResult.ImpactPoint;
	}
	FVector ParticlePoint = End;

	if (LaserNiagaraSystem)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			LaserNiagaraSystem,
			Start,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true
		);
		if (NiagaraComponent)
		{
			NiagaraComponent->SetVariableVec3(FName("beam end"), ParticlePoint);
		}
	}
	
	if (!bHit)
	{
		return;
	}
	
	float DamageToUmbrella = LaserDamage;
	if (HitResult.GetActor()->IsA(AUmbrella::StaticClass()))
	{
		DamageToUmbrella = LaserDamage * 0.2;
		if (!ImpactLasersFModComponentInterface->IsEventPlaying())
		{
			ImpactLasersFModComponentInterface->PlayEvent();
		}
	}
	else
	{
		ImpactLasersFModComponentInterface->StopEvent();
	}
	
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitResult.GetActor()))
	{
		ParticlePoint = HitResult.ImpactPoint;
		HealthInterface->SubtractHealth(DamageToUmbrella, EWeaponType::Laser);
	}
}
