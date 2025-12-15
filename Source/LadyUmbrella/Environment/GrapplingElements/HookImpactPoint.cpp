
#include "HookImpactPoint.h"
#include "Components/BoxComponent.h"

void AHookImpactPoint::ContactWithCable(FVector& Direction)
{
	OnHookImpactDynamic.Broadcast(0, SpeedOfRotationForParent, RotatorForParent);
}

// Sets default values
AHookImpactPoint::AHookImpactPoint()
{
	RootComponent		= CreateDefaultSubobject<USceneComponent>("Root");
	ImpactCollisionBox	= CreateDefaultSubobject<UBoxComponent>("ImpactCollisionBox");
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");

	ImpactCollisionBox->SetupAttachment(RootComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);

	//DegreesOfRotationForParent = 0.f;
	SpeedOfRotationForParent = 0.f;
	RotatorForParent = FRotator::ZeroRotator;	
}