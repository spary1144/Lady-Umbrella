// Fill out your copyright notice in the Description page of Project Settings.


#include "DetachableFallingElement.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Util/AssetUtil.h"

ADetachableFallingElement::ADetachableFallingElement()
{
	ObjectMesh->SetNotifyRigidBodyCollision(true);
	ObjectMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	Root = ObjectMesh;
	
	FallingStartingPointLocation.SetLocation(FVector(0.f,0.f,-1250.f));

	// Falling
	bHasSpawned = false;
	bChildSimulateFall = false;
	bChildHasToBeDestroyed = false;
	bChildCanDealDamage = false;
	ChildLingeringSeconds = 1.f;
	ChildSimulationSeconds = 1.f;
}

void ADetachableFallingElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADetachableFallingElement::BeginPlay()
{
	Super::BeginPlay();
	if (!bHasSpawned)
	{
		FallingObject = GetWorld()->SpawnActor<AFallingElement>(FallingClass);
		bHasSpawned = true;
	}
	if (IsValid(FallingObject))
	{
		FallingObject->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		FallingObject->HittableBy.Empty();
		FallingObject->HittableBy.Add(HitSource::ENVIROMENT);
		FallingObject->SetActorRelativeLocation(FallingStartingPointLocation.GetLocation());
		
		FallingObject->SetSimulateFall(bChildSimulateFall);
		FallingObject->SetDestroy(bChildHasToBeDestroyed);
		FallingObject->SetCanDealDamage(bChildCanDealDamage);
		FallingObject->SetStepSeconds(ChildSimulationSeconds);
		FallingObject->SetLingeringSeconds(ChildLingeringSeconds);
	}
}

void ADetachableFallingElement::Reaction(HitSource Source)
{
	if(Super::ShouldReact(Source))
	{
		FallingObject->Reaction(HitSource::ENVIROMENT);
		ObjectMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	}
}