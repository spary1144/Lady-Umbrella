// Fill out your copyright notice in the Description page of Project Settings.


#include "FallingElement.h"

#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "Misc/MapErrors.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"

AFallingElement::AFallingElement()
{
	Step = 1.0f;
	LingeringSeconds = 1.0f;

	FmodComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Falling Element FMod Sound Component");
	FmodComponentInterface->InitializeMap();
	FmodComponentInterface->bAutoActivate = false;
	
	SkeletalMesh->SetAnimationMode(EAnimationMode::Type::AnimationSingleNode);
	SkeletalMesh->GlobalAnimRateScale = 0.35f;
	SkeletalMesh->SetRelativeLocation(FVector(0.f,-40.f,0.f));
	SkeletalMesh->CanCharacterStepUpOn = ECB_Yes;
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	SkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SkeletalMesh->SetGenerateOverlapEvents(true);
	SkeletalMesh->Play(false);
	SkeletalMesh->SetPosition(0.0335f, false);
	
	BoxRoot->SetupAttachment(SkeletalMesh, SocketName);
	BoxRoot->SetRelativeLocation(FVector(0.f,40.f,600.f));

	bSimulateFall = false;
	bHasToBeDestroyed = false;
	bCanDealDamage = true;
	bCoverBox = false;
	
	ColliderSize = FVector::OneVector;
	
	FallingDistance = 0.5f;
	GravityForce = 0.f;
	CurrentSpeed = 0.f;
	TargetHeight = 0.f;
}

void AFallingElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bSimulateFall && !IsValid(SkeletalMesh->AnimationData.AnimToPlay))
	{
		return;
	}
	
	if (bSimulateFall && Reacted && Step > 0.f) 
	{
		if(Step > 0.f)
		{
			CurrentSpeed += -(GravityForce * DeltaTime);
			FVector UpdatedPos = ObjectMesh->GetComponentLocation();
			UpdatedPos.Z = FMath::Max(ObjectMesh->GetComponentLocation().Z + CurrentSpeed * DeltaTime, TargetHeight);
			ObjectMesh->SetWorldLocation(UpdatedPos, false, nullptr, ETeleportType::None);
		}
		
		Step -= DeltaTime;
	}
	
	if (Reacted && ((bSimulateFall && Step <= 0.f) || !SkeletalMesh->IsPlaying()))
	{
		if (bSimulateFall)
		{
			SkeletalMesh->PlayAnimation(SkeletalMesh->AnimationData.AnimToPlay, false);
			SkeletalMesh->SetPosition(SimulationInitialAnimPosition);
		}
		Tags.Remove("NoClimb");
	
		if (IsValid(BoxRoot) && BoxRoot->OnComponentBeginOverlap.IsBound())
		{
			BoxRoot->DestroyComponent();
		}
		
		if (bHasToBeDestroyed) 
		{
			LingeringSeconds -= DeltaTime;
			if (LingeringSeconds <= 0.f)
			{
				
				if (BoxRoot->OnComponentBeginOverlap.IsBound())
				{
					BoxRoot->OnComponentBeginOverlap.RemoveDynamic(this, &AFallingElement::HandleCollision);
				}
				Destroy();
			}
		}
	}
}

void AFallingElement::BeginPlay()
{
	Super::BeginPlay();
	
	if (bCanDealDamage)
	{
		if (IsValid(BoxRoot) && !BoxRoot->OnComponentBeginOverlap.IsAlreadyBound(this, &AFallingElement::HandleCollision))
		{
			BoxRoot->OnComponentBeginOverlap.AddDynamic(this, &AFallingElement::HandleCollision);
		}
		
		if (!IsValid(SkeletalMesh))
		{
			return;
		}
		
		UPhysicsAsset* PhysAsset = SkeletalMesh->GetPhysicsAsset();
		for (USkeletalBodySetup* PhysBone: PhysAsset->SkeletalBodySetups)
		{
			if (!PhysBone)
			{
				continue;
			}
			
			for (const FKBoxElem& BoxPrimitive : PhysBone->AggGeom.BoxElems)
			{
				if (BoxPrimitive.GetName().IsEqual(PrimitiveName))
				{
					ColliderSize.X += BoxPrimitive.X;
					ColliderSize.Y += BoxPrimitive.Y;
					ColliderSize.Z += BoxPrimitive.Z;
				}
			}
		}
		ColliderSize *= bCoverBox ? AltCollisionBoxScaler : CollisionBoxScaler;
		FVector RelativeLocation = BoxRoot->GetRelativeLocation() + (bCoverBox ? FVector(0,0,0) : CollisionBoxOffset);
		
		BoxRoot->SetRelativeLocation(RelativeLocation);
	}
	
	if (bSimulateFall)
	{
		SkeletalMesh->SetPosition(SimulationInitialAnimPosition, false);
		SkeletalMesh->SetWorldLocation(BoxRoot->GetComponentLocation());
		if (bCanDealDamage)
		{
			BoxRoot->SetRelativeLocation(FVector::ZeroVector);
		}
	}
	
	if (IsValid(FmodComponentInterface))
	{
		FmodComponentInterface->InitializeMap();
	}
}

void AFallingElement::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	UE_LOG(LogTemp, Warning, TEXT("EndPlay - Falling Element %s with reason %d"), *GetName(), EndPlayReason);	
	if (!IsValid(BoxRoot))
	{
		Super::EndPlay(EndPlayReason);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("EndPlay - Falling Element (BoxRoot exist) %s"), *GetName());
	if (BoxRoot->OnComponentBeginOverlap.IsAlreadyBound(this, &AFallingElement::HandleCollision))
	{
		BoxRoot->OnComponentBeginOverlap.RemoveDynamic(this, &AFallingElement::HandleCollision);
	}
	Super::EndPlay(EndPlayReason);
}

void AFallingElement::BeginDestroy()
{
	Super::BeginDestroy();
}

void AFallingElement::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (!IsValid(SkeletalMesh) && !SkeletalMesh->DoesSocketExist(SocketName))
	{
		return;
	}
	BoxRoot->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	UE_LOG(LogActor, Display, TEXT("BoxRoot attached to socket: %s"), *SocketName.ToString());
}

void AFallingElement::Reaction(HitSource Source) 
{
	if(Super::ShouldReact(Source))
	{
		BoxRoot->SetBoxExtent(ColliderSize);
		Tags.Add("NoClimb");
		if (!bSimulateFall)
		{
			FAnimNotifyEvent OutNotify;
			float StartingPos = SkeletalMesh->GetPosition();
			SkeletalMesh->PlayAnimation(SkeletalMesh->AnimationData.AnimToPlay, false);
			SkeletalMesh->SetPosition(StartingPos);
			FmodComponentInterface->PlayEvent();
			return;
		}
		FHitResult Hit(ForceInit);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredComponent(Super::BoxRoot);
		QueryParams.AddIgnoredComponent(Super::ObjectMesh);
		if(GetWorld()->LineTraceSingleByChannel(Hit, Super::ObjectMesh->GetComponentLocation(), Super::ObjectMesh->GetComponentLocation() + FVector(0, 0, -10000.f), ECC_Visibility, QueryParams)) {
			
			FallingDistance = Hit.Distance;
			TargetHeight = ObjectMesh->GetComponentLocation().Z - FallingDistance; 
			GravityForce = 2.f * Hit.Distance/FMath::Pow(Step,2.f); // Derived from free fall formula (ULM) -> h = (gt^2)/2
			CurrentSpeed = 0.f;
		}
		FmodComponentInterface->PlayEvent();
	}
}

void AFallingElement::HandleCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsValid(OtherActor) || !bCanDealDamage || !OtherActor->Implements<UHealthInterface>())
	{
		return;
	}
	
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(OtherActor))
	{
		HealthInterface->SubtractHealth(100.f, EWeaponType::FallingObject);
	}
}