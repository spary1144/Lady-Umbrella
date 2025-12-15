// Fill out your copyright notice in the Description page of Project Settings.


#include "ReactiveElement.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

// Sets default values
AReactiveElement::AReactiveElement()
{
	PrimaryActorTick.bCanEverTick = true;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StMesh"));
	SetRootComponent(ObjectMesh);
	
	ObjectMesh->bCastDynamicShadow = true;
	ObjectMesh->bAffectDynamicIndirectLighting = true;
	ObjectMesh->PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	ObjectMesh->SetGenerateOverlapEvents(false);
	ObjectMesh->SetCanEverAffectNavigation(true);
	ObjectMesh->SetSimulatePhysics(false);
	ObjectMesh->SetCollisionProfileName(TEXT("ReactItem"));
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkMesh"));
	SkeletalMesh->bCastDynamicShadow = true;
	SkeletalMesh->bAffectDynamicIndirectLighting = true;
	SkeletalMesh->PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	SkeletalMesh->SetupAttachment(ObjectMesh);
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetSimulatePhysics(false);
	SkeletalMesh->SetCanEverAffectNavigation(true);
	SkeletalMesh->SetCollisionProfileName(TEXT("ReactItem"));
	
	BoxRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxRoot->CanCharacterStepUpOn = ECB_No;
	BoxRoot->SetShouldUpdatePhysicsVolume(true);
	BoxRoot->SetCanEverAffectNavigation(false);
	BoxRoot->SetupAttachment(ObjectMesh);

	FmodReactionComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Reaction Sound");
	
}

// Called when the game starts or when spawned
void AReactiveElement::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(FmodReactionComponentInterface))
	{
		FmodReactionComponentInterface->InitializeMap();
		FmodUtils::AttachFmodComponentToComponent(FmodReactionComponentInterface, GetRootComponent());
	}
}

// Called every frame
void AReactiveElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AReactiveElement::Reaction(HitSource Source)
{
	ShouldReact(Source);
}

bool AReactiveElement::ShouldReact(HitSource Source)
{
	if (Reacted || HittableBy.Num() < 1 || Source == HitSource::NONE)
	{
		return false;
	}

	if (HittableBy.Contains(Source)) 
	{
		Reacted = true; 
		OnReactedDelegate.Broadcast();
		if (IsValid(FmodReactionComponentInterface))
		{
			FmodReactionComponentInterface->PlayEvent();
		}
		return true; 
	}

	return false;
}
