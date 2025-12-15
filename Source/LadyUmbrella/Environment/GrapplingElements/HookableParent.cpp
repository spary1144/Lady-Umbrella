
#include "HookableParent.h"
#include "HookImpactPoint.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

AHookableParent::AHookableParent()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsSolved = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(SceneComponent);

	static ConstructorHelpers::FClassFinder<AHookImpactPoint> HookImpactPointSubclassFinder(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/LevelElements/Puzzle/HookableElements/BP_HookImpactPoint.BP_HookImpactPoint_C'"));
	if (HookImpactPointSubclassFinder.Succeeded())
	{
		HookImpactPointSubClass = HookImpactPointSubclassFinder.Class;
	}
	FmodSoundComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Fmod Sound Component Interface");
}

void AHookableParent::BeginPlay()
{
	Super::BeginPlay();

	FmodSoundComponentInterface->InitializeMap();
	FmodUtils::AttachFmodComponentToComponent(FmodSoundComponentInterface, GetRootComponent());
}

#if WITH_EDITOR

void AHookableParent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
/*
	for ( const auto AttachedActor : AttachedActors )
	{
		AHookImpactPoint* AttachedActorImpactPoint = Cast<AHookImpactPoint>(AttachedActor);
		if (IsValid(AttachedActorImpactPoint) && !ImpactPoints.Contains(AttachedActorImpactPoint))
		{
			AttachedActorImpactPoint->Destroy();
		}
	}
*/
}

#endif

void AHookableParent::AddNewImpactPoint()
{
	FActorSpawnParameters Params;
	AHookImpactPoint* NewImpactPoint = GetWorld()->SpawnActor<AHookImpactPoint>(HookImpactPointSubClass);

	if (IsValid(NewImpactPoint) && NewImpactPoint->OnHookImpactDynamic.IsBound() == false)
	{
		ImpactPoints.Add(NewImpactPoint);
		NewImpactPoint->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewImpactPoint->OnHookImpactDynamic.AddDynamic(this, &AHookableParent::RotateFromChildDynamic);
		
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
