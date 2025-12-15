// 
// MeleeComponent.h
// 
// Implementation of the MeleeComponent class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "MeleeComponent.h"

#include "MotionWarpingComponent.h"
#include "RootMotionModifier.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Assets/AnimationAssets.h"
#include "LadyUmbrella/Assets/VFXAssets.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "Particles/ParticleSystem.h"

UMeleeComponent::UMeleeComponent()
{	
	bCanDetect          = true;
	Damage              = 30.0f;
	LaunchFactor        = 200.0f;
	LaunchAddedVelocity = FVector(0, 0, 100.0f);
	Range               = 350.0f;
	TargetRotation      = FRotator::ZeroRotator;
	TraceRadius         = 30.0f;
	CurrentMeleeMontageIndex	= 0;
	
	FmodMeleComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Fmod Melee Sound Component");		

	UFMODEvent* MeleeEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_MELEE.UMBRELLA_MELEE'");
	FmodUtils::TryAddEvent(MeleeEvent, FmodMeleComponentInterface);
}

void UMeleeComponent::BeginPlay()
{
	Super::BeginPlay();
	ImpactParticleSystem = FVFXAssets::UmbrellaMeleeNiagaraSystem;
	
	MeleeMontages.Add(FAnimationAssets::PlayerMeleeMontage);
	MeleeMontages.Add(FAnimationAssets::PlayerMeleeMontage2);
	MeleeMontages.Add(FAnimationAssets::PlayerMeleeMontage3);
}

void UMeleeComponent::Attack()
{
	const TWeakObjectPtr<AActor> HitActor = GetClosestEnemyInRange();

	(void) OnMeleeAttack.ExecuteIfBound();
	SetIsAttacking(true);

	UWorld* World = GetWorld();
	if (!IsValid(World)) return;

	FTimerManager& TimerManager = World->GetTimerManager();
	
	if (TimerManager.TimerExists(ComboTimerHandle) && TimerManager.IsTimerActive(ComboTimerHandle))
	{
		TimerManager.ClearTimer(ComboTimerHandle);
	}
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]()
	{
		CurrentMeleeMontageIndex = 0;
	});
	TimerManager.SetTimer(ComboTimerHandle, TimerDel, 1.f, false);
	
	if (IsValid(HitActor.Get()) && HitActor->IsA(AGenericCharacter::StaticClass()) && !Cast<AGenericCharacter>(HitActor.Get())->GetHealthComponent()->IsDead())
	{
		
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FGlobalPointers::PlayerCharacter->GetActorLocation(), HitActor->GetActorLocation());
		const FRotator OwnerRotation = FGlobalPointers::PlayerCharacter->GetActorRotation();
		const FRotator DesiredRotation = FRotator(OwnerRotation.Pitch, LookAtRotation.Yaw, OwnerRotation.Roll);
		const FVector DesiredLocation = HitActor->GetActorLocation();
		FMotionWarpingTarget WarpingTarget;
		WarpingTarget.Name = "Melee";
		WarpingTarget.Location = DesiredLocation;
		WarpingTarget.Rotation = DesiredRotation;
		FGlobalPointers::PlayerCharacter->PlayMotionWarpingMontage(GetMeleeMontage(), 1, WarpingTarget);
	}
	else
	{
		FGlobalPointers::PlayerCharacter->GetMotionWarpingComponent()->RemoveWarpTarget("Melee");
		FGlobalPointers::PlayerCharacter->PlayMontage(GetMeleeMontage(), 1);
	}
}

TWeakObjectPtr<AActor> UMeleeComponent::GetClosestEnemyInRange() const
{
	FHitResult HitResult;
	
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(FGlobalPointers::PlayerCharacter);
	
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation(),
		Range,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		HitResult,
		true
	);

	return HitResult.GetActor();
}


void UMeleeComponent::ProcessCollision(const FVector& TraceStart, const FVector& ActorRightVector)
{
	if (!bCanDetect)
	{
		return;
	}

	FHitResult HitResult;

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceStart,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4),
		false,
		IgnoredActors,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (!bHit)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (HitActor->IsA(AGenericCharacter::StaticClass()))
	{
		AEnemyCharacter* Character = Cast<AEnemyCharacter>(HitActor);
		if (!IsValid(Character))
		{
			return;
		}
		
		Character->SubtractHealth(Damage, EWeaponType::Melee);
		
		if (!IsValid(FGlobalPointers::PlayerCharacter))
		{
			return;
		}

		if (Character->IsDead())
			FGlobalPointers::GameInstance->AddMeleeKill();

		if (FGlobalPointers::GameInstance->GetMeleeKills() >= KILLS_TO_ACHIEVEMENT)
			Steam::UnlockAchievement(ESteamAchievement::SMASH_GOONS_MELEE);
		
		const FVector PlayerForward = FGlobalPointers::PlayerCharacter->GetActorForwardVector();
		FVector Direction = PlayerForward.GetSafeNormal();//ActorRightVector.GetSafeNormal();
		Character->LaunchCharacter(Direction * LaunchFactor + LaunchAddedVelocity, true, true);

		Character->ContactWithMeleeAttack(Direction);
	}
	else if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitActor))
	{
		HealthInterface->SubtractHealth(Damage, EWeaponType::Melee);
	}
	if (IsValid(FmodMeleComponentInterface))
	{
		FmodMeleComponentInterface->PlayEvent();
	}
	bCanDetect = false;
	
	if (IsValid(ImpactParticleSystem.Get()))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticleSystem.Get(), HitResult.ImpactPoint, FRotator::ZeroRotator, true);
	}
}

UAnimMontage* UMeleeComponent::GetMeleeMontage()
{
	if (CurrentMeleeMontageIndex >= MeleeMontages.Num())
	{
		CurrentMeleeMontageIndex = 0;
	}
	return MeleeMontages[CurrentMeleeMontageIndex++];
}
