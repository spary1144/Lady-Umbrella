// Main class for electric thingies
#include "ElectricElement.h"

#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Gadgets/ElectricProjectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

AElectricElement::AElectricElement()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);
	ObjectMesh->SetupAttachment(RootComp);
	
	PrimaryActorTick.bCanEverTick = true;
	ElectrificationDuration = 2.5f;
	ElectrifiedDelaySeconds = 2.5f;

	MeshPosition = FVector::ZeroVector;
	MeshRotation = FRotator::ZeroRotator;
	MeshScale	 = FVector::OneVector;

	bShowSparkEffect = true;

	FmodElectricSound = CreateDefaultSubobject<UFModComponentInterface>(TEXT("FMod Electric Sound"));
	FmodReactionSound = CreateDefaultSubobject<UFModComponentInterface>(TEXT("FMod Reaction Sound"));

	UFMODEvent* ElectricSoundEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/Electric/ELECTRIC_ELEMENTS.ELECTRIC_ELEMENTS'");
	FmodUtils::TryAddEvent(ElectricSoundEvent, FmodElectricSound);

	UFMODEvent* ReactionSoundEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Gameplay_Environment/ELECTRIC_CRANE.ELECTRIC_CRANE'");
	FmodUtils::TryAddEvent(ReactionSoundEvent, FmodReactionSound);
	
	//FmodElectricSound->bAutoActivate = false;
	SparksEffect = AssetUtils::FindObject<UParticleSystem>(TEXT("/Game/Particles/CascadeParticleSystems/CPS_Sparks.CPS_Sparks"));
}

void AElectricElement::BeginPlay()
{
	Super::BeginPlay();
	/*
		FmodReactionComponentInterface->InitializeMap();
		FmodUtils::AttachFmodComponentToComponent(FmodReactionComponentInterface, GetRootComponent());
	*/
}

void AElectricElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsValid(ObjectMesh))
	{
		ObjectMesh->SetRelativeLocation(MeshPosition);
		ObjectMesh->SetRelativeRotation(MeshRotation);
		ObjectMesh->SetRelativeScale3D(MeshScale);
	}
}


void AElectricElement::SetElectrified(const bool bNewElectrified)
{
	if (bElectrified == bNewElectrified)
	{
		return;
	}

	bElectrified = bNewElectrified;
	
	if (ElectrifiedDelaySeconds > 0.f) // We never know, designers might want negative delays.
	{
		GetWorldTimerManager().SetTimer(
			ElectrifiedDelayTimerHandle,
			this,
			&AElectricElement::PerformElectrification,
			ElectrifiedDelaySeconds,
			false
		);
	}
	else
	{
		PerformElectrification();
	}
}

void AElectricElement::PerformElectrification()
{
	if (!bElectrified)
	{
		ResetElectrifiedState();
		return;
	}
	OnElectrified();
	
	FmodElectricSound->SetParameter(ElectricSoundParameterName,ElectricSoundParameterOn);
	FmodElectricSound->PlayEvent();
	
	FmodReactionSound->PlayEvent();
	
	if (IsValid(SparksEffect) && bShowSparkEffect)
	{
		SparksComponent = UGameplayStatics::SpawnEmitterAttached(
			SparksEffect,
			RootComponent,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
	
	for (AElectricElement* Connected : ConnectedElectricElements)
	{
		if (IsValid(Connected) && !Connected->bElectrified)
		{
			Connected->SetElectrified(true);
		}
	}
	
	GetWorldTimerManager().SetTimer(ElectrifiedTimerHandle, this, &AElectricElement::ResetElectrifiedState, ElectrificationDuration, false);
}

void AElectricElement::ResetElectrifiedState()
{
	bElectrified = false;
	OffElectrified();
	if (IsValid(SparksComponent))
	{
		// Set sound to stop
		
		SparksComponent->DeactivateSystem();
		SparksComponent->DestroyComponent();
		SparksComponent = nullptr;
	}
	
	if (!FmodReactionSound->IsEventPlaying())
	{
		FmodReactionSound->PlayEvent();
	}

	if (FmodElectricSound->IsEventPlaying())
	{
		FmodElectricSound->SetParameter(ElectricSoundParameterName, ElectricSoundParameterOff);
	}
}

void AElectricElement::OnOverlap(UPrimitiveComponent* OverlappedComp,
							 AActor* OtherActor,
							 UPrimitiveComponent* OtherComp,
							 int32 OtherBodyIndex,
							 bool bFromSweep,
							 const FHitResult& SweepResult)
{
	SetElectrified(true);

	if (OtherActor->IsA(AElectricProjectile::StaticClass()))
	{
		OtherActor->Destroy();
	}
}