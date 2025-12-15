// Fill out your copyright notice in the Description page of Project Settings.

#include "HookableAnimation.h"
#include "CableComponent.h"
#include "HookImpactPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

AHookableAnimation::AHookableAnimation()
{
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->SetupAttachment(GetRootComponent());

	AnimationTimerCallingRate = 0.1f;
	bDisableHookPoint = false;
	
	ImpactPoint = CreateDefaultSubobject<UChildActorComponent>("ImpactPoint");
	ImpactPoint->SetupAttachment(SkeletalMeshComponent);
	
	static ConstructorHelpers::FClassFinder<AHookImpactPoint> ImpactPointClass(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/LevelElements/Puzzle/HookableElements/BP_HookImpactPoint.BP_HookImpactPoint_C'"));
	if (ImpactPointClass.Succeeded())
	{
		HookImpactPointClass = ImpactPointClass.Class;
		ImpactPoint->SetChildActorClass(HookImpactPointClass);
	}
}

void AHookableAnimation::RotateFromChildDynamic(const float DegreesOfRotation, const float SpeedOfRotation, const FRotator& RotatorForParent)
{
	if (!IsValid(SkeletalMeshComponent) || !IsValid(SkeletalMeshComponent->AnimationData.AnimToPlay))
	{
		return;
	}

	SkeletalMeshComponent->PlayAnimation(SkeletalMeshComponent->AnimationData.AnimToPlay, false);
	
	DirectionOfHook = GetActorForwardVector();

	const float AnimationTimeLength = SkeletalMeshComponent->AnimationData.AnimToPlay->GetPlayLength();
	
	if (IsValid(GetFmodSoundComponentInterface()))
	{
		GetFmodSoundComponentInterface()->PlayEvent();
	}
	
	GetWorld()->GetTimerManager().SetTimer(FallingHandler,this, &AHookableAnimation::OnAnimationEnded,
		AnimationTimerCallingRate, false, AnimationTimeLength);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUmbrella::StaticClass(), FoundActors);

	const AUmbrella* Umbrella = FGlobalPointers::Umbrella;

	if (!IsValid(Umbrella))
	{
		return;		
	}

	Umbrella->SetCableEndLocation
	(
		UKismetMathLibrary::InverseTransformLocation
		(
			Umbrella->GetActorTransform(), GetActorLocation()
		)
	);
	
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

void AHookableAnimation::OnAnimationEnded()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	TimerManager.ClearTimer(FallingHandler);
	
	SkeletalMeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (bDisableHookPoint)
	{
		DisableHookPointMaterial();
	}

	if (!bHasToEmulatePhysics)
	{
		return; 
	}

	SkeletalMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	SkeletalMeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	SkeletalMeshComponent->SetSimulatePhysics(true);

	const FVector Force = ArrowComponent->GetForwardVector();
	SkeletalMeshComponent->AddImpulse(  Force * ForceImpulseMultiplier );
	
	TimerManager.SetTimer(DisappearHandler,this, &AHookableAnimation::DestroyObject, AnimationTimerCallingRate, false , TimeToSimulatePhysics);


	
}
// find any hook point class in the instance, because obviously, they're not always added to the arrays in HookableParent.
// and remove the overlay material.
void AHookableAnimation::DisableHookPointMaterial() const
{
	TArray<UChildActorComponent*> ChildComps;
	this->GetComponents<UChildActorComponent>(ChildComps);
	for (UChildActorComponent* Comp : ChildComps)
	{
		AActor* Child = Comp->GetChildActor();
		AHookImpactPoint* ImpactPtn = Cast<AHookImpactPoint>(Child);
		if (IsValid(ImpactPtn) && IsValid(ImpactPtn->GetStaticMeshComp()))
		{
			ImpactPtn->GetStaticMeshComp()->SetOverlayMaterial(nullptr);
		}
	}
}

void AHookableAnimation::DestroyObject()
{
	Destroy();
}