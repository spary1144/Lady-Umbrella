// 
// SplinePath.cpp
// 
// Set-up Spline
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "SplinePath.h"
#include "Components/BillboardComponent.h"
#include "Components/SplineComponent.h"

ASplinePath::ASplinePath()
{
	SetActorTickEnabled(false);
	
	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	SetRootComponent(Billboard);
	Billboard->SetHiddenInGame(true);
	Billboard->SetVisibility(true);
	Billboard->SetMobility(EComponentMobility::Static);
	Billboard->SetComponentTickEnabled(false);

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(Billboard);
	Spline->SetMobility(EComponentMobility::Static);
	Spline->SetComponentTickEnabled(false);
}