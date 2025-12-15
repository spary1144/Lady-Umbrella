// 
// GrapplingObject.cpp
// 
// Implementation of the GrapplingObject class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//
#include "GrapplingObject.h"
#include "CableComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Components/SkeletalMeshComponent.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

AGrapplingObject::AGrapplingObject()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent			  = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	FallingDirection		  = CreateDefaultSubobject<UArrowComponent>(TEXT("End Falling Location"));
	SceneRootComponent		  = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SkeletalMeshComponent	  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	StaticMeshComponent		  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	PhysicsCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Physics Collision Component"));

	bIsSolved				  = false;
	
	BoxComponent->SetBoxExtent(FVector(200, 200, 200));
	PhysicsCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision );
	PhysicsCollisionComponent->SetGenerateOverlapEvents(false);
	PhysicsCollisionComponent->SetBoxExtent(FVector(27.4,2.8,268.3));
	PhysicsCollisionComponent->AddLocalRotation(FRotator(0, 0, 90));


	StaticMeshComponent->SetVisibility(false);
	StaticMeshComponent->SetHiddenInGame(true);
	
	// Setup Hierarchy
	SetRootComponent(SceneRootComponent);
	
	StaticMeshComponent->SetupAttachment(SceneRootComponent);
	BoxComponent->SetupAttachment(SceneRootComponent);
	PhysicsCollisionComponent->SetupAttachment(SceneRootComponent);
	FallingDirection->SetupAttachment(SceneRootComponent);
	SkeletalMeshComponent->SetupAttachment(PhysicsCollisionComponent);

	AnimationTimerCallingRate = 0.01;
	AnimationTimerFirstCallOffset = 0.5f;

	TimeToSimulatePhysics = 1.f;
	ForceImpulseMultiplier = 600.f;
	RemoveOverlayOnFinished = false;
}

void AGrapplingObject::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AGrapplingObject::OnFallingAnimationEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(FallingHandle);

	PhysicsCollisionComponent->SetGenerateOverlapEvents(true);
	PhysicsCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PhysicsCollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	
	if (!bHasToEmulatePhysics)
	{
		return; 
	}
	SkeletalMeshComponent->SetVisibility(false);
	StaticMeshComponent->SetHiddenInGame(false);
	StaticMeshComponent->SetVisibility(true);
	StaticMeshComponent->SetSimulatePhysics(true);
	
	const FVector Force = DirectionOfHook - StaticMeshComponent->GetComponentLocation();
	StaticMeshComponent->AddImpulse(  Force * ForceImpulseMultiplier );
	
	GetWorld()->GetTimerManager().SetTimer(DisappearHandler,this, &AGrapplingObject::DestroyObject, AnimationTimerCallingRate, false , TimeToSimulatePhysics);

}

void AGrapplingObject::DestroyObject()
{
	/*
	if (IsValid(SkeletalMeshComponent))
	{
		SkeletalMeshComponent->DestroyComponent();
	}
	if (IsValid(StaticMeshComponent))
	{
		StaticMeshComponent->DestroyComponent();
	}
	*/
	Destroy();
}

void AGrapplingObject::ContactWithCable(FVector& Direction)
{
	if (bIsSolved)
	{
		return;
	}
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}
	DirectionOfHook = Direction;
	SkeletalMeshComponent->PlayAnimation(SkeletalMeshComponent->AnimationData.AnimToPlay, false);
	// 0.2 is InFirstDelay and will be replaced with AnimLength declared right on top of this comment.
	// Right now AnimLength returns 1.6 bc animation movement and timeline are not synced
	const float AnimLenght = SkeletalMeshComponent->AnimationData.AnimToPlay->GetPlayLength();
	GetWorld()->GetTimerManager().SetTimer(FallingHandle,this, &AGrapplingObject::OnFallingAnimationEnd, AnimationTimerCallingRate, false, 0.2 );

	bIsSolved		= true;	
	SolvedTransform = GetActorTransform();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUmbrella::StaticClass(), FoundActors);
	AUmbrella* Umbrella = Cast<AUmbrella>(FoundActors[0]);

	if (!IsValid(Umbrella))
	{
		return;		
	}

	//FLogger::DebugLog("Setting cable positions");
	Umbrella->SetCableEndLocation( UKismetMathLibrary::InverseTransformLocation(Umbrella->GetActorTransform(), GetActorLocation()) );
	UCableComponent* CableComponent = Umbrella->GetCableComponent();
	if(!IsValid(CableComponent))
	{
		return;
	}
	CableComponent->SetVisibility(true);
	CableComponent->SetComponentTickEnabled(true);
	CableComponent->EndLocation = FVector::ZeroVector;
	CableComponent->SetVisibility(false);
	CableComponent->SetComponentTickEnabled(false);
	
}

IHookInterface::GrabType AGrapplingObject::GetGrabType()
{
	return PULL;
}

void AGrapplingObject::LoadGame()
{
	if (const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		bIsSolved = GameInstance->GI_SavePuzzle.bIsSolved;
		//SetActorTransform(GameInstance->GI_SavePuzzle.WorldTransform);
	}
}

void AGrapplingObject::SaveGame()
{
	if (const ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetGameInstance()))
	{
		bIsSolved = GameInstance->GI_SavePuzzle.bIsSolved;
		//SetActorTransform(GameInstance->GI_SavePuzzle.WorldTransform);
	}
}

void AGrapplingObject::SaveDataCheckPoint()
{
	if (ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance)
	{
		GameInstance->GI_SavePuzzle.bIsSolved	   = bIsSolved;
		GameInstance->GI_SavePuzzle.WorldTransform = GetActorTransform();
	}
}

void AGrapplingObject::LoadDataCheckPoint()
{
	if (const ULU_GameInstance* GameInstance = FGlobalPointers::GameInstance)
	{
		bIsSolved = GameInstance->GI_SavePuzzle.bIsSolved;
	//	SetActorTransform(GameInstance->GI_SavePuzzle.WorldTransform);
	}
}
