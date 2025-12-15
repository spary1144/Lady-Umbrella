// Fill out your copyright notice in the Description page of Project Settings.

#include "AICoverEntryPoint.h"
#include "CoverSpline.h"
#include "CoverParent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

// Sets default values for this component's properties
AAICoverEntryPoint::AAICoverEntryPoint()
{
	// Set this component to not be ticked every frame to improve performance.
	PrimaryActorTick.bCanEverTick = false;

	SetActorHiddenInGame(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Root->SetMobility(EComponentMobility::Stationary);
	
	StaticMesh = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("Child Static Mesh"));
	if (IsValid(StaticMesh))
	{
		StaticMesh->SetMobility(EComponentMobility::Stationary);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		StaticMesh->SetRelativeLocation(FVector(0,0,10));
		StaticMesh->SetupAttachment(Root);

		static ConstructorHelpers::FObjectFinder<UStaticMesh> ArrowStaticMeshRef (TEXT("/Game/Meshes/StaticMeshes/EditorOnly/CoverSystem/SM_CoverEntryWidget.SM_CoverEntryWidget"));
		if (ArrowStaticMeshRef.Object != nullptr)
		{
			StaticMesh->SetStaticMesh(ArrowStaticMeshRef.Object);
		}
	}
}

void AAICoverEntryPoint::UpdateCoverPointBasedOnSection()
{
	// If the Owning Spline got deleted by the fcking editor, we try to get it re-applied.
	EnsureOwningSplineIsSet();
	
	if (OwningSpline != nullptr)
	{
		OwningSpline->OrderOnConstructUpdate();

		auto* CoverParent = OwningSpline->TryGetParent();
#if WITH_EDITOR
		if (IsValid(CoverParent))
		{
			CoverParent->RerunConstructionScripts();
		}
#endif
	}
	else
	{
#if UE_EDITOR
		FLogger::ErrorLog("Couldn't find the CoverSpline that owns this IaEntryPoint. Delete this point and reset the Spline instead.");
#endif
	}
}

UCoverSpline* AAICoverEntryPoint::EnsureOwningSplineIsSet()
{
	if (OwningSpline == nullptr)
	{
		ACoverParent* CoverParent = Cast<ACoverParent>(GetAttachParentActor());

		if (IsValid(CoverParent))
		{
			CoverParent->ResetIaEntryPointsOwningSplines();
		}
	}

	return OwningSpline;
}
