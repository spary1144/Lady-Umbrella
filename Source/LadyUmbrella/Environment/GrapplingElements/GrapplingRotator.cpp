// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingRotator.h"

#include "HookImpactPoint.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AGrapplingRotator::AGrapplingRotator()
{
	
	SecondHitPoint = CreateDefaultSubobject<UBoxComponent>("Second Hit Point");
	SecondHitPoint->SetupAttachment(RootComponent);
	SecondHitPoint->AddRelativeLocation(FVector( 0, 350, 55));
	SecondHitPoint->SetBoxExtent(FVector(60, 60, 60 ));
	
	BoxComponent->AddRelativeLocation(FVector(1, -360, 55));
	BoxComponent->SetBoxExtent(FVector(50, 50, 50));

	ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshFinder(TEXT("/Script/Engine.StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Cube.SM_Cube'"));
	if(StaticMeshFinder.Succeeded() && IsValid(StaticMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(StaticMeshFinder.Object);
	}
	
	StaticMeshComponent->SetRelativeScale3D(FVector(1, 8, 1));
	StaticMeshComponent->SetRelativeLocation(FVector(-50, -400, 0));
	StaticMeshComponent->SetupAttachment(SceneRootComponent);
	StaticMeshComponent->SetVisibility(true);
	StaticMeshComponent->SetHiddenInGame(false);

	PhysicalCollision = CreateDefaultSubobject<UBoxComponent>("Physical Collision");
	PhysicalCollision->SetRelativeLocation(FVector(0, 0, 50));
	PhysicalCollision->SetupAttachment(SceneRootComponent);
	PhysicalCollision->SetBoxExtent(FVector(50, 400, 50));
		
	static ConstructorHelpers::FClassFinder<AHookImpactPoint> HookImpactPointSubclassFinder(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/LevelElements/Puzzle/HookableElements/BP_HookImpactPoint.BP_HookImpactPoint_C'"));
	if (HookImpactPointSubclassFinder.Succeeded())
	{
		HookImpactPointSubClass = HookImpactPointSubclassFinder.Class;
	}
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("/Script/LadyUmbrella.RotationCurveFloat'/Game/DataStructures/Curves/Camera/C_Rotation.C_Rotation'"));
	if (CurveAsset.Succeeded())
	{
		RotationCurve= CurveAsset.Object;
	}
}

void AGrapplingRotator::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed - Grappling Rotator %s"), *GetName());	
	Super::Destroyed();
	for (int32 i = 0; i < ImpactPoints.Num(); ++i)
	{
		AHookImpactPoint* ImpactPoint = ImpactPoints[i];
		if (IsValid(ImpactPoint))
		{
			ImpactPoint->OnHookImpactDynamic.RemoveAll(this);
			ImpactPoint->OnHookImpactDynamic.Clear();
			ImpactPoint->Destroy();
		}
	}
}

#if WITH_EDITOR
void AGrapplingRotator::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for ( const auto AttachedActor : AttachedActors )
	{
		AHookImpactPoint* AttachedActorImpactPoint = Cast<AHookImpactPoint>(AttachedActor);
		if (IsValid(AttachedActorImpactPoint) && !ImpactPoints.Contains(AttachedActorImpactPoint))
		{
			AttachedActorImpactPoint->Destroy();
		}
	}
}
#endif


void AGrapplingRotator::RotateFromChildDynamic(const float DegreesOfRotationChild, const float SpeedOfRotationChild, const FRotator& RotatorForParent)
{
	GetWorld()->GetTimerManager().ClearTimer(RotationTimer);

	if (RotatorForParent.IsZero())
	{
		return;
	}
	//const float AngleDelta = FMath::RadiansToDegrees(InitialQuat.AngularDistance(TargetQuat));
	
	const FQuat RotationQuat = FQuat(RotatorForParent);
	const FQuat CurrentQuat  = GetActorQuat();
	FinalRotation			 = RotationQuat * CurrentQuat;

	const FRotator CurrentRelRot = GetRootComponent()->GetRelativeRotation();
	InitialQuat					 = CurrentRelRot.Quaternion();
	const float AngleDelta		 = FMath::RadiansToDegrees(InitialQuat.AngularDistance(FinalRotation));
	
	RotationDuration = AngleDelta / SpeedOfRotationChild;
	RotationElapsed  = 0.f;
	
	GetWorld()->GetTimerManager().SetTimer(
		RotationTimer,
		this,
		&AGrapplingRotator::RotateActorDynamic,
		RotateActorDynamicRate,
		true
	);
}
// This is not being called anymore
void AGrapplingRotator::ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit)
{
	if (!IsValid(ComponentHit))
	{
		return;
	}
	float DirectionSign = 0.f;
	if (ComponentHit == SecondHitPoint)
	{
		DirectionSign = -1.f;
	}
	else if (ComponentHit == BoxComponent)
	{
		DirectionSign = 1.f;
	}
	else
	{
		return;
	}

	const FQuat LocalRotationQuat = FQuat(FRotator(0.f, 90.f * DirectionSign, 0.f));

	const FQuat CurrentQuat = GetActorQuat();
	FinalRotation			= LocalRotationQuat * CurrentQuat;

	GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	GetWorld()->GetTimerManager().SetTimer(RotationTimer, this, &AGrapplingRotator::RotateActor, GetWorld()->GetDeltaSeconds(), true);
}

void AGrapplingRotator::RotateActorDynamic()
{

	const float DeltaTime	= GetWorld()->GetDeltaSeconds();
	RotationElapsed += DeltaTime;

	const float Alpha = FMath::Clamp(RotationElapsed / RotationDuration, 0.f, 1.f);
	float CurveValue  = Alpha;

	if (IsValid(RotationCurve))
	{
		CurveValue	= RotationCurve->GetFloatValue(Alpha);
	}
	
	const FQuat CurrentQuat = GetActorQuat();
	const FQuat NewQuat		= FQuat::Slerp(CurrentQuat, FinalRotation, CurveValue);

	RootComponent->SetRelativeRotation(NewQuat);
	//SetActorRotation(NewQuat);
	
	if (NewQuat.Equals(FinalRotation, 0.001f))
	{
		RootComponent->SetRelativeRotation(NewQuat);
		//SetActorRotation(FinalRotation);
		GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	}
}

void AGrapplingRotator::AddNewImpactPoint()
{
	FActorSpawnParameters Params;
	AHookImpactPoint* NewImpactPoint = GetWorld()->SpawnActor<AHookImpactPoint>(HookImpactPointSubClass);

	if (IsValid(NewImpactPoint) && NewImpactPoint->OnHookImpactDynamic.IsBound() == false)
	{
		ImpactPoints.Add(NewImpactPoint);
		NewImpactPoint->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewImpactPoint->OnHookImpactDynamic.AddDynamic(this, &ThisClass::RotateFromChildDynamic);

		#if WITH_EDITOR

			ImpactPointsAux.Add(NewImpactPoint);

			if (GEditor)
			{
				GEditor->SelectNone(false, true);
				GEditor->SelectActor(NewImpactPoint, true, true);
				GEditor->NoteSelectionChange();
			}
		#endif
	}
}

IHookInterface::GrabType AGrapplingRotator::GetGrabType()
{
	return PULLWITHCOMPONENT;
}

void AGrapplingRotator::RotateActor()
{
	const FQuat CurrentQuat		= GetActorQuat();
	const float DeltaTime		= GetWorld()->GetDeltaSeconds();
	const FQuat NewQuat			= FQuat::Slerp(CurrentQuat, FinalRotation, SpeedOfRotation * DeltaTime);

	SetActorRotation(NewQuat);
	
	if (NewQuat.Equals(FinalRotation, 0.001f))
	{
		SetActorRotation(FinalRotation);
		GetWorld()->GetTimerManager().ClearTimer(RotationTimer);
	}
}