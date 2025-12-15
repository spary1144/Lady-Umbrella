// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityBars.h"

#include "LadyUmbrella/AI/Managers/ArenaManager.h"

#define SM_SECURITY_BARS TEXT("StaticMesh'/Game/Meshes/StaticMeshes/Museum/Props/SecurityRoom/SM_SecurityBars.SM_SecurityBars'")


// Sets default values
ASecurityBars::ASecurityBars()
{
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(SM_SECURITY_BARS).Object);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetCollisionProfileName(TEXT("BlockAll")); 
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetupAttachment(RootComponent);

	SecurityBarState = ESecurityBarState::Blocked;
	HeightDiff = 800.f;
}

// Called when the game starts or when spawned
void ASecurityBars::BeginPlay()
{
	Super::BeginPlay();
	InitialPosition = GetActorLocation();
	if (IsValid(ArenaManager))
	{
		ArenaManager->OnReinforcementsCalled.AddDynamic(this, &ASecurityBars::ElevateSecurityBars);
	}

	if (IsValid(ElevationCurve))
	{
		FOnTimelineFloat OnTimeline;
		OnTimeline.BindUFunction(this, FName("GetElevation"));

		ElevationTimeline.AddInterpFloat(ElevationCurve, OnTimeline);
		ElevationTimeline.SetLooping(false);
	}
	
}

void ASecurityBars::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElevationTimeline.TickTimeline(DeltaTime);
}

void ASecurityBars::ElevateSecurityBars()
{
	//StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (SecurityBarState == ESecurityBarState::Blocked)
	{
		ElevationTimeline.PlayFromStart();
		SecurityBarState = ESecurityBarState::Lifted;
	}
}

void ASecurityBars::GetElevation(const float Value)
{
	FVector Elevation = InitialPosition;
	Elevation.Z += HeightDiff * Value;

	SetActorLocation(Elevation);
}


