// 
// EnemyCharacter.cpp
// 
// Implementation of the Enemy Character class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "EnemyCharacter.h"

#include "Algo/RandomShuffle.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/PickableObjects/Ammo.h"
#include "LadyUmbrella/UI/Basics/Icon/EnemyStateIcon.h"
#include "LadyUmbrella/Util/TransformUtil.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "GameFramework/NavMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Assets/AnimationAssets.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/GadgetsReactionComponent/GrappleReactionComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineActor.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineDTO.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineState.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/FunctionLibrary/FrameFreezeLibrary.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"


AEnemyCharacter::AEnemyCharacter()
	: bEnableDebugMessages(false)
	, ProjectileSpeed(12500.f)
	, LeadFactor(10.65f)
	, MaxOffset(600.f)
	, bHasToken(false)
	, StunDuration(2.f)
	, RagdollProbability(0.6)
	, bCanInteractWithReactiveElements(true)
	, FreezeTime(0.05f)
	, FreezeTimeDilation(0.01f)
	, ForgivenessTime(0.5f)
	, AimCoverForgivenessTime(2)
	, bPlayerLuckActive(false)
	, MissChance(DefaultMissChance)
	, bUseDefaultMissChanceSystem(true)
	, DefaultMissChance(0)
	, MinLeftRightOffsetLongDistance(100)
	, MaxLeftRightOffsetLongDistance(250)
	, MinLeftRightOffsetCloseDistance(250)
	, MaxLeftRightOffsetCloseDistance(350)
	, MinUpOffsetLateralBullet(-250)
	, MaxUpOffsetLateralBullet(250)
	, MinLeftRightOffsetOverhead(-150)
	, MaxLeftRightOffsetOverhead(150)
	, MinUpOffsetOverhead(50)
	, MaxUpOffsetOverhead(150)
	, bCanFireWeapon(true)
	, ParticleExplosionScale(FVector(0.1f,0.1f,0.1f))
	, ReactMontagePlayRate(1.f)
	, KickMontagePlayRate(1.f)
	, KickStunDuration(2.f)
	, bDebugSphere(false)
	, MinDistanceToConsiderFalling(300.f)
	, MinDistanceToListenAllyDeath(1000.f)
	, MaxMissChance(1.f)
	, MinMissChance(0.f)
	, MaxShootRange(1500.f)
    , MinShootRange(0.f)
    , bHasBeenHookedBefore(false)
	, bHasBeenShockedBefore(false)
{
	//PrimaryActorTick.bCanEverTick = true;

	GrappleReactionComponent = CreateDefaultSubobject<UGrappleReactionComponent>(TEXT("GrappleReactionComponent"));
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyStateIcon");
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->AddRelativeLocation(FVector(0, 0, 100));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // for the projectiles
	Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // for the hook
	Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block); // for the melee
	Capsule->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Block); // for the AimAssist
	
	AmmoDropAmountCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_AmmoDropAmount.C_AmmoDropAmount'")).Object;
	AmmoDropRateCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Systems/C_AmmoDropRate.C_AmmoDropRate'")).Object;
	DeadForceCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_DeadImpulseForce.C_DeadImpulseForce'")).Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Game/Particles/CascadeParticleSystems/CPS_Explosion.CPS_Explosion"));
	if (ParticleSystemAsset.Succeeded())
	{
		ParticleExplosionSystem = ParticleSystemAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load enemy particle system asset!"));
	}
	
	static ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(TEXT("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_EnemyData.DT_LadyUmbrellaSystems_EnemyData'"));
	if (DataFinder.Succeeded())
	{
		DataTableRowHandler.DataTable = DataFinder.Object;
	}
	
	SetCoverMovementComponent(CreateDefaultSubobject<UCharacterCoverMovementComponent>(TEXT("CharacterCoverMovementComponent")));

	//Voice Line Audio Component
	FmodComponentVoiceLines = CreateDefaultSubobject<UFModComponentInterface>("Fmod Audio VoiceLine Interface Component");
	static UFMODEvent* DeathEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/DeathShout/DEATH_EVENT.DEATH_EVENT'");
	FmodUtils::TryAddEvent(DeathEvent, FmodComponentVoiceLines);

	FmodWhooshComponentInterface = CreateDefaultSubobject<UFModComponentInterface>("Fmod Audio Whoosh Interface Component");

	static UFMODEvent* WhooshEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Mafia/BULLET_WHOOSH.BULLET_WHOOSH'");
	FmodUtils::TryAddEvent(WhooshEvent, FmodWhooshComponentInterface);
}

void AEnemyCharacter::TryEquipWeapon()
{
	//Si ya tiene arma, no equipar
	if (IsValid(EquippedWeapon))
	{
		return;
	}
	
	const FTransform WeaponSocket = GetMesh()->GetSocketTransform(socketName, ERelativeTransformSpace::RTS_World);
	EquippedWeapon = GetWorld()->SpawnActor<AGenericWeapon>(WeaponBP, WeaponSocket);

	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socketName);
		EquippedWeapon->SetOwner(this);
	}
}

void AEnemyCharacter::Initialize()
{
	if (!IsValid(FGlobalPointers::Umbrella))
	{
		FLogger::ErrorLog("Invalid Umbrella pointer in AEnemyCharacter.Initialize()");
		return;
	}

	if (!FGlobalPointers::Umbrella->OnIsCoveredByUmbrellaChanged.IsAlreadyBound(this, &AEnemyCharacter::ModifyMissChanceDependingOnUmbrellaState))
	{
		FGlobalPointers::Umbrella->OnIsCoveredByUmbrellaChanged.AddDynamic(this, &AEnemyCharacter::ModifyMissChanceDependingOnUmbrellaState);
	}

	UCharacterCoverMovementComponent* PlayerCoverMovementComponent = FGlobalPointers::PlayerCharacter->GetCoverMovementComponent();
	if (IsValid(PlayerCoverMovementComponent) && !PlayerCoverMovementComponent->OnIsCoveredChanged.IsAlreadyBound(this, &AEnemyCharacter::ChangeEnemiesMissChance))
	{
		PlayerCoverMovementComponent->OnIsCoveredChanged.AddDynamic(this, &AEnemyCharacter::ChangeEnemiesMissChance);
		if (PlayerCoverMovementComponent->IsMovementControlledByCover())
		{
			ChangeEnemiesMissChance(true);
		}
	}

	GrappleReactionComponent->Initialize();
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(EnemyStateWidget) && IsValid(WidgetComponent))
	{
		WidgetComponent->SetWidgetClass(EnemyStateWidget);
	}

	TryEquipWeapon();
	MissChance = CalculateMissChance();
	
	GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyCharacter::Initialize);

	if (IsValid(FGlobalPointers::Umbrella))
	{
		FGlobalPointers::Umbrella->OnBrokenUmbrella.AddUObject(this,&AEnemyCharacter::OnShieldDepleted);
	}

	MeshRef = GetMesh();
	if (IsValid(MeshRef))
	{
		AnimInstance = MeshRef->GetAnimInstance();
	}
}

// +-------------------------------------------------+
// |                                                 |
// |					   AI                        |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::PauseAILogic(const FString& Cause) const
{
	if (!IsValid(AIController))
	{
		return;
	}
	
	AIController->StopMovement();
	if (IsValid(AIController->BrainComponent))
	{
		AIController->BrainComponent->PauseLogic(Cause);
	}
}

void AEnemyCharacter::StopAILogic(const FString& Cause) const
{
	if (!IsValid(AIController))
	{
		return;
	}
	
	AIController->StopMovement();
	if (IsValid(AIController->BrainComponent))
	{
		AIController->BrainComponent->StopLogic(Cause);
	}
}

void AEnemyCharacter::ResumeAILogic(const FString& Cause) const
{
	if (!IsValid(AIController))
	{
		return;
	}
	
	if (IsValid(AIController->BrainComponent))
	{
		AIController->BrainComponent->ResumeLogic(Cause);
	}
}

bool AEnemyCharacter::IsInCloseCombat() const
{
	if (!IsValid(AIController))
	{
		return false;
	}
	
	AAIControllerGeneric* ControllerGeneric = Cast<AAIControllerGeneric>(AIController);
	if (!IsValid(ControllerGeneric))
	{
		return false;
	}
	
	return ControllerGeneric->IsInCloseCombat();
}

void AEnemyCharacter::TriggerVoiceLineFirstCombat(const uint8_t Index) 
{
	if (!IsValid(GetVoiceLineComponent()))
	{
		return;
	}

	GetVoiceLineComponent()->QueueVoiceLine(FVoiceLineDTO{EVoiceLineState::FIRST_COMBAT_AGENCY, EVoiceLineActor::AGENCY,  this,true,Index});
}


// +-------------------------------------------------+
// |                                                 |
// |					 Health                      |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::SubtractHealth(const float _HealthToSubtract, enum EWeaponType DamageType)
{
    Super::SubtractHealth(_HealthToSubtract,DamageType);
	
	if (!GetHealthComponent()->IsDead())
	{
		PlayReactMontage();
	}
}

void AEnemyCharacter::OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned)
{
	Super::OnStatusEffectChangeListener(StatusEffect, bStunned);
	
	if (!IsValid(GetStatusEffectComponent()))
	{
		FLogger::ErrorLog("Invalid StatusEffectComponent pointer in EnemyCharacter.OnStatusEffectChangeListener()");
	}
	if (GetStatusEffectComponent()->IsStunned())
	{
		PauseAILogic("Electrified");
	}
	if (StatusEffect == EStatusEffect::NORMAL)
	{
		ResumeAILogic("Electrified");
	}
}

void AEnemyCharacter::KillCharacter()
{
	UHealthComponent* HealthComp = GetHealthComponent();
	if (IsValid(HealthComp))
	{
		HealthComp->SubtractHealth(HealthComp->GetMaxHealth());
	}
}

void AEnemyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (!IsValid(GrappleReactionComponent))
	{
		return;
	}

	float DistanceInZ = FMath::Abs(LocationBeforeFall.Z - Hit.Location.Z);
	if (DistanceInZ < MinDistanceToConsiderFalling)
	{
		return;
	}
	
	if (GrappleReactionComponent->GetStartsFallingAfterBeingHooked() && GetMovementComponent()->IsFalling())
	{
		KillCharacter();
		return;
	}

	if (bStartsFallingAfterBeingPushedByMelee && GetMovementComponent()->IsFalling())
	{
		KillCharacter();
	}
}

void AEnemyCharacter::OnDetectPlayer()
{
}

void AEnemyCharacter::OnAllyDeath(const FVector& DeadLocation) const
{
	float DistanceToAlly = FVector::Dist(GetActorLocation(),DeadLocation);
	
	if (DistanceToAlly > MinDistanceToListenAllyDeath)
	{
		return;
	}

	AAIControllerGeneric* GenericController = Cast<AAIControllerGeneric>(GetController());
	if (!IsValid(GenericController))
	{
		return;
	}
	
	GenericController->SetRushPlayerBlackboard();
}

void AEnemyCharacter::ActivateRagdoll()
{
	UCharacterMovementComponent* CharacterComp = GetCharacterMovement();
	if (IsValid(CharacterComp))
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}
	
	SetReplicateMovement(false);
	DetachFromControllerPendingDestroy();

	//Disable collision in capsule
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionProfileName(TEXT("RagDoll"));
	SetActorEnableCollision(true);
	
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
}

void AEnemyCharacter::AddImpulseDependingOnDamageType() const
{
	EWeaponType LastDamageTypeRef = GetLastDamageType();

	FVector FinalImpulse = FVector::ZeroVector;

	FVector EnemyPlayerVector = (GetActorLocation() - FGlobalPointers::PlayerCharacter->GetActorLocation());
	EnemyPlayerVector.Z = 0.0f; 
	FVector ForceDirection = EnemyPlayerVector.GetSafeNormal();
	
	const float DistanceEnemyPlayer = FVector::Dist2D(GetActorLocation(), FGlobalPointers::PlayerCharacter->GetActorLocation());
	const float HorizontalForce = DeadForceCurve->GetFloatValue(DistanceEnemyPlayer);
	
	if (LastDamageTypeRef == EWeaponType::Grenade)
	{
		FinalImpulse = ForceDirection * HorizontalForce;
		FinalImpulse += GetActorUpVector() * RAGDOLL_VERTICAL_FORCE;
	}
	if (LastDamageTypeRef == EWeaponType::Umbrella || LastDamageTypeRef == EWeaponType::FallingObject)
	{
		FinalImpulse = ForceDirection * HorizontalForce;
	}
	
	//Apply force
	GetMesh()->AddImpulse(FinalImpulse, NAME_None, true);

	//Force down to avoid fly
	if (LastDamageTypeRef != EWeaponType::Grenade)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
		if (GetMesh())
		{
			GetMesh()->AddImpulseToAllBodiesBelow(FVector(0, 0, FORCE_DOWN), NAME_None, true);
		}
		});
	}
}

void AEnemyCharacter::OnDeathMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == FAnimationAssets::EnemyDeath1)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnDeathMontageCompleted);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AEnemyCharacter::OnMontageNotifyBegin);
	}
}

void AEnemyCharacter::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "Ragdoll")
	{
		ActivateRagdoll();
				
		if (IsValid(AnimInstance))
		{
			AnimInstance->StopAllMontages(KINDA_SMALL_NUMBER);
		}
	}
}

void AEnemyCharacter::SetDead()
{
    Super::SetDead();

	//stop voice lines
	if (IsValid(FmodComponentVoiceLines))
	{
		FmodComponentVoiceLines->StopEvent();
	}

	if (FmodComponentVoiceLines->SetCurrentEvent("DEATH_EVENT"))
	{
		FmodComponentVoiceLines->PlayEvent();
	}
	
	if (GetEquippedWeapon())
	{
		GetEquippedWeapon()->SimulatePhysicsBeforeDestroy();
	}

	if (IsValid(WidgetComponent))
	{
		WidgetComponent->DestroyComponent();
	}
	
	AAIControllerGeneric* AIControllerGeneric = Cast<AAIControllerGeneric>(GetController());
	if (IsValid(AIControllerGeneric))
	{
		if (IsValid(AIControllerGeneric->GetBrainComponent()))
		{
			AIControllerGeneric->TryReduceCloseCombatCounter();
			AIControllerGeneric->GetBrainComponent()->StopLogic("DEAD");
		}
		
		AIControllerGeneric->UnPossess();
	}

	UFrameFreezeLibrary::FrameFreeze(this, FreezeTimeDilation, FreezeTime);
	
	DropAmmo();

	if (FMath::RandRange(0.f, 1.f) <= RagdollProbability)
	{
		if (!GetHitPositions().IsEmpty() && GetActorLocation().Z > GetHitPositions().Last().Z)
		{
			PlayMontage(FAnimationAssets::EnemyDeath2, 1);
		}
		else
		{
			PlayMontage(FAnimationAssets::EnemyDeath1, 1);
		}
		if (IsValid(AnimInstance))
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnDeathMontageCompleted);
			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AEnemyCharacter::OnMontageNotifyBegin);
		}
		return;
	}

	ActivateRagdoll();
	AddImpulseDependingOnDamageType();
	
}

void AEnemyCharacter::DestroyEquippedWeapon()
{
	if (!IsValid(EquippedWeapon))
	{
		return;
	}
	
	if (GetWeaponType() == EWeaponType::Shield)
	{
		GetShield()->Destroy();
	}
	
	EquippedWeapon->Destroy();
}

void AEnemyCharacter::StopVoiceLineActive() const
{
	UVoiceLineComponent* VoiceLineComp = GetVoiceLineComponent();
	if (!IsValid(VoiceLineComp))
	{
		return;
	}
	VoiceLineComp->ClearSubtitle();
}


// +-------------------------------------------------+
// |                                                 |
// |				Kick Interface                   |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::KickActor(const FVector& Direction)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (IsValid(MovementComponent))
	{
		MovementComponent->Launch(Direction * ENEMY_KICKED_IMPULSE + GetActorUpVector() * KICK_UP_FORCE);
		OnKickEvent();
	}

	FGlobalPointers::GameInstance->AddVaultStuns();

	PlayKickedMontage();
	
	if (FmodComponentVoiceLines->SetCurrentEvent("DEATH_EVENT"))
	{
		FmodComponentVoiceLines->PlayEvent();
	}
}

void AEnemyCharacter::PrepareKnockBackActor()
{
	PauseAILogic("Kicked");

	if (bEnableDebugMessages)
	{
		FLogger::DebugLog("Stun started");
	}
	
	if (IsValid(EnemyStunTexture))
	{
		GetWidget()->SetImageState(EnemyStunTexture);
	}
	
	GetCoverMovementComponent()->NotifyExitCover();
}


// +-------------------------------------------------+
// |                                                 |
// |				   Drop Ammo                     |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::DropAmmo()
{
	// 
	// The drop system follows an exponential drop rate, in which getting an ammo drop gets more likely
	// the less ammo you currently have.
	//
	// This is done by checking the percentage of missing ammo, and plugging them into the two curve
	// assets requested by Design. These curves then give two additional percentage values, which tell
	// how much ammo should be dropped, and the probability of dropping the ammo.
	// 
	// Finally, the drop amount is calculated by taking this drop amount percentage and converting it
	// back into a discrete value, making sure we don't exceed the allowed limit.
	//
	const int32 AmmoTotal = FGlobalPointers::Umbrella->GetAmmoCurrent()    + FGlobalPointers::Umbrella->GetAmmoReserve();
	const int32 AmmoMax   = FGlobalPointers::Umbrella->GetAmmoCurrentMax() + FGlobalPointers::Umbrella->GetAmmoReserveMax();

	const float AmmoDeltaPercentage = 1 - static_cast<float>(AmmoTotal) / AmmoMax;
	
	const int32 AmmoDropAmount = AmmoDropAmountCurve->GetFloatValue(AmmoDeltaPercentage) * AmmoMax;
	const float AmmoDropRate = AmmoDropRateCurve->GetFloatValue(AmmoDeltaPercentage);

	if (AmmoDropAmount > MIN_DROP_AMOUNT && FMath::RandRange(MIN_AMMO_DROP_RATE, MAX_AMMO_DROP_RATE) <= AmmoDropRate)
	{
		const FTransform SpawnTransform = TransformUtils::TransformLocation(GetActorLocation());
		AAmmo* SpawnedAmmo = GetWorld()->SpawnActor<AAmmo>(AmmoBP, SpawnTransform);
		if (IsValid(SpawnedAmmo))
		{
			SpawnedAmmo->SetAmountToPickup(AmmoDropAmount);
			AAIControllerGeneric* ControllerGeneric = Cast<AAIControllerGeneric>(AIController);
			if (!IsValid(ControllerGeneric))
			{
				return;
			}
			AArenaManager* OwnerArena = ControllerGeneric->GetArena();
			if (!IsValid(OwnerArena))
			{
				return;
			}

			OwnerArena->AddAmmoBoxToDropArray(SpawnedAmmo);
		}
	}
}

// +-------------------------------------------------+
// |                                                 |
// |					Grapple                      |
// |                                                 |
// +-------------------------------------------------+
bool AEnemyCharacter::WillFallOffLedge(FVector& LaunchDirection)
{
	const UPawnMovementComponent* MovementComponent = GetMovementComponent();
	if (!IsValid(MovementComponent))
	{
		return false;
	}
	
    const FVector& EnemyFeetLocation = MovementComponent->GetFeetLocation();

	//Project the direction in horizontal plane
    FVector LaunchDirFlat = LaunchDirection;
    LaunchDirFlat.Z = 0;
    LaunchDirFlat = LaunchDirFlat.GetSafeNormal();

    const FVector FuturePosition = EnemyFeetLocation + LaunchDirFlat * DistanceToCheckFalling;
    
    const FVector StartTraceLocation = FuturePosition + OffsetOnStartSphereTraceLocation;
    const FVector EndTraceLocation = FuturePosition + OffsetOnEndSphereTraceLocation; 
    
    // Sphere to see if we are hiting something
    FHitResult HitResult;
    HitResult.Init();
    
    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this);

	UWorld* World = GetWorld();
	if (!IsValid(World)) return false;
	
    const bool bHitSomething = UKismetSystemLibrary::SphereTraceSingle(
        World,
        StartTraceLocation,
        EndTraceLocation,
        SPHERE_RADIUS,
        TraceTypeQuery1,
        false,
        IgnoredActors,
        EDrawDebugTrace::None,
        HitResult,
        true,
        FLinearColor::Red,
        FLinearColor::Blue,
        DRAWTIME
    );

    if (bHitSomething && IsValid(HitResult.GetActor()))
    {
        // Dispalcement from sphere center and impact point
        const FVector TraceCenter = HitResult.TraceStart + (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal() * HitResult.Distance;
        const float DistFromCenter = FVector::Dist(HitResult.ImpactPoint, FVector(TraceCenter.X, TraceCenter.Y, HitResult.ImpactPoint.Z));

        // If the impact is far from the center, we assume that half goes out
        if (DistFromCenter > DISTANCE_CENTER_SPHERE_TO_HIT_LOCATION)
        {
            return true;
        }

        // line trace to see falling distance
        FHitResult DownHit;
        const FVector DownStart = FVector(TraceCenter.X,TraceCenter.Y,TraceCenter.Z + 20.f);
        const FVector DownEnd = DownStart - MAX_SAFE_HEIGHT; // max safe height

        const bool bGroundBelow = UKismetSystemLibrary::LineTraceSingle(
            World,
            DownStart,
            DownEnd,
            TraceTypeQuery1,
            false,
            IgnoredActors,
            EDrawDebugTrace::None,
            DownHit,
            true,
            FLinearColor::Yellow,
            FLinearColor::Green,
            DRAWTIME
        );

        if (!bGroundBelow) //no ground found
        {
            return true;
        }
    	
    	const float FallingDist = FVector::Dist(DownHit.ImpactPoint, DownStart);
    	if (FallingDist > MIN_DISTANCE_TO_CONSIDER_FALLING)
    	{
    		return true;
    	}
    }

    return false;
}

void AEnemyCharacter::EndGrapplingPosition() const
{
	if (!IsValid(GrappleReactionComponent))
	{
		return;
	}

	GrappleReactionComponent->EndGrapplingPosition();
}

void AEnemyCharacter::ContactWithCable(FVector& SocketHookLocation)
{
	if (!IsValid(GrappleReactionComponent))
	{
		return;
	}

	GrappleReactionComponent->ContactWithCable(SocketHookLocation);

	LocationBeforeFall = GetActorLocation();
}

// +-------------------------------------------------+
// |                                                 |
// |                   Melee						 |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::PlayFallingAnimationOnPushedByMelee() const
{
	if (!IsValid(FallingOnPushedByMeleeMontage))
	{
		return;
	}

	if (!IsValid(MeshRef))
	{
		return;
	}
	
	if (!IsValid(AnimInstance) || AnimInstance->Montage_IsPlaying(FallingOnPushedByMeleeMontage))
	{
		return;
	}
	
	AnimInstance->Montage_Play(FallingOnPushedByMeleeMontage);
	AnimInstance->Montage_SetPlayRate(FallingOnPushedByMeleeMontage, FallingOnPushedByMeleePlayRate);
}

void AEnemyCharacter::ContactWithMeleeAttack(FVector& Direction)
{
	UCharacterCoverMovementComponent* CoverMovement = GetCoverMovementComponent();
	if (IsValid(CoverMovement) && CoverMovement->IsMovementControlledByCover())
	{
		CoverMovement->NotifyExitCover();
	}
	
	if (WillFallOffLedge(Direction))
	{
		const FVector LaunchVelocity = Direction * LaunchSpeedFactorOnFallByMelee;
		LaunchCharacter(LaunchVelocity, true, true);

		LocationBeforeFall = GetActorLocation() +  Direction * LaunchSpeedFactorOnFallByMelee;
		
		PlayFallingAnimationOnPushedByMelee();
		
		bStartsFallingAfterBeingPushedByMelee = true;
	}
}

// +-------------------------------------------------+
// |                                                 |
// |                   Miss Chance                   |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::ChangeEnemiesMissChance(const bool NewState)
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(EquippedWeapon))
	{
		return;
	}
	
	if (!NewState)
	{
		TimerManager.ClearTimer(MissChanceTimer);
		TimerManager.ClearTimer(CoverMissChanceTimer);
    	
		if (bPlayerLuckActive)
		{
			bUseDefaultMissChanceSystem = false;
			if (!TimerManager.IsTimerActive(PlayerLuckTimer))
			{
				TimerManager.SetTimer(PlayerLuckTimer, this, &AEnemyCharacter::LosePlayerLuck, ForgivenessTime, false);
			}
		}
		else
		{
			bUseDefaultMissChanceSystem = true;
			SetMissChance(CalculateMissChance());
		}
	}
	else
	{
		bPlayerLuckActive = true;
		TimerManager.ClearTimer(PlayerLuckTimer);
		if (FGlobalPointers::PlayerCharacter->IsAiming())
		{
			TimerManager.SetTimer(MissChanceTimer, this, &AEnemyCharacter::WaitToChangeEnemiesMissChance, AimCoverForgivenessTime, false);
			TimerManager.ClearTimer(CoverMissChanceTimer);
		}
		else
		{
			if (!IsValid(EquippedWeapon))
			{
				FLogger::ErrorLog("Invalid EquippedWeapon pointer in EnemyCharacter.ChangeEnemyMissChance()");
				return;
			}
			
			TimerManager.ClearTimer(MissChanceTimer);
			TimerManager.SetTimer(CoverMissChanceTimer, this, &AEnemyCharacter::CheckLineOfSightWithPlayer, EquippedWeapon->GetMinWeaponCadence() / 2, true, 0);
		}
		
		if (!IsValid(GetVoiceLineComponent()))
		{
			return;
		}
		
		GetVoiceLineComponent()->QueueVoiceLine(
			FVoiceLineDTO
			{
				EVoiceLineState::ENEMY_IN_COVER, EVoiceLineActor::ENEMY,  this,false
			}
		);
	}
}

void AEnemyCharacter::ModifyMissChanceDependingOnUmbrellaState(bool IsShieldOpen)
{
	if (IsShieldOpen)
	{
		bUseDefaultMissChanceSystem = false;
		SetMissChance(0);
	}
	else
	{
		if (bPlayerLuckActive && GetWorldTimerManager().IsTimerActive(PlayerLuckTimer))
		{
			return;
		}
		
		bUseDefaultMissChanceSystem = true;
		SetMissChance(CalculateMissChance());
	}
}

void AEnemyCharacter::WaitToChangeEnemiesMissChance()
{
	bUseDefaultMissChanceSystem = true;
	SetMissChance(CalculateMissChance());
}

void AEnemyCharacter::CheckLineOfSightWithPlayer()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		FLogger::ErrorLog("[EnemyCharacter::CheckLineOfSightWithPlayer]: FGlobalPointers::PlayerCharacter NULL");
		return;
	}

	
	FHitResult OutHit;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(FGlobalPointers::Umbrella);
	
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* Child : AttachedActors)
	{
		IgnoredActors.Add(Child);
	}
	
	FVector EnemyViewLocation;
	FRotator EnemyRotation;
	GetActorEyesViewPoint(EnemyViewLocation, EnemyRotation);

	const FVector LineTraceEnd = FGlobalPointers::PlayerCharacter->GetActorLocation() + VERTICAL_LINE_TRACE_OFFSET;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), EnemyViewLocation, LineTraceEnd, ETraceTypeQuery::TraceTypeQuery2, false, IgnoredActors, EDrawDebugTrace::None, OutHit, true, FColor::Yellow))
	{
		const AActor* ManagingActor = OutHit.HitObjectHandle.GetManagingActor();

		if (!IsValid(ManagingActor))
		{
			return;
		}
		
		if (ManagingActor->IsA(APlayerCharacter::StaticClass()))
		{
			SetMissChance(CalculateMissChance());
			bUseDefaultMissChanceSystem = true;
		}
		else
		{
			bUseDefaultMissChanceSystem = false;
			SetMissChance(1);
		}
	}
}

void AEnemyCharacter::LosePlayerLuck()
{
	bPlayerLuckActive = false;
	bUseDefaultMissChanceSystem = true;
	SetMissChance(CalculateMissChance());
}


// +-------------------------------------------------+
// |                                                 |
// |                   Fire Weapon                   |
// |                                                 |
// +-------------------------------------------------+

bool AEnemyCharacter::TryToFire()
{
	if (!IsValid(EquippedWeapon) || !IsValid(FGlobalPointers::PlayerCharacter))
	{
		return false;
	}
	EBulletType BulletType =  EquippedWeapon->GetWeaponBulletType();
	
	if (BulletType == EBulletType::Normal && !bCanFireWeapon)
	{
		return false;
	}
	
	if (IsAiming() && GetCoverMovementComponent()->IsMovementControlledByCover())
	{
		const FVector AimDirection = FGlobalPointers::PlayerCharacter->GetActorLocation() - GetActorLocation();
		GetCoverMovementComponent()->SetInCoverAimingDirection(AimDirection);
	}

	
	const float RandomValue = FMath::FRandRange(0.f, 1.f);
	
	if (bUseDefaultMissChanceSystem)
	{
		MissChance = CalculateMissChance();
	}

	const bool bMiss = RandomValue <= MissChance;

	FHitResult OutHit;
	GetFireHitResult(OutHit, bMiss);

	if (!IsReloading())
	{
		if (EquippedWeapon->Fire())
		{
			//Voice Line Throwing
			if (IsValid(GetVoiceLineComponent()))
			{
				GetVoiceLineComponent()->QueueVoiceLine(
					FVoiceLineDTO
					{
						EVoiceLineState::SHOOTING, EVoiceLineActor::ENEMY,  this	
					}
				);
			}
			bCanFireWeapon = false;
			GetWorld()->GetTimerManager().SetTimer(FireWeaponTimerHandler, [this] () -> void
			{
				bCanFireWeapon = true;
			}, EquippedWeapon->GetWeaponCadence(), false);

			return true;
		}
	}
	return false;
}

FVector AEnemyCharacter::GetPawnTraceHitPoint(const FCollisionQueryParams& CollisionParams) const
{
	FVector EnemyLinetraceStart;
	FRotator EnemyRotation;
	if (IsValid(GetController()))
	{
		GetController()->GetPlayerViewPoint(EnemyLinetraceStart, EnemyRotation);
	}

	FVector EnemyLinetraceEnd;
	FRotator PlayerRotation;
	FGlobalPointers::PlayerCharacter->GetActorEyesViewPoint(EnemyLinetraceEnd, PlayerRotation);
	FHitResult EnemyTraceOutHit;

	GetWorld()->LineTraceSingleByChannel(EnemyTraceOutHit, EnemyLinetraceStart, EnemyLinetraceEnd, ECC_Camera, CollisionParams);
	
	return EnemyTraceOutHit.bBlockingHit ? EnemyTraceOutHit.ImpactPoint : EnemyLinetraceEnd;
}

bool AEnemyCharacter::HandleLandedShot(FHitResult& OutHit, const FCollisionQueryParams& CollisionParams, const FVector& WeaponTraceStart, const FVector& WeaponTraceEnd)
{
	GetWorld()->LineTraceSingleByChannel(OutHit, WeaponTraceStart, WeaponTraceEnd, ECC_Camera, CollisionParams);
	
	BulletInteraction(OutHit);

	if (FGlobalPointers::PlayerCharacter->GetHealth() <= 0)
	{
		EquippedWeapon->SpawnBulletParticleFromWeaponMuzzle(WeaponTraceEnd, false);
		return true;
	}
	
	if (FGlobalPointers::Umbrella->IsOpen())
	{
		const FVector  PlayerLoc = FGlobalPointers::PlayerCharacter->GetActorLocation();
		const FVector  Vel       = FGlobalPointers::PlayerCharacter->GetVelocity();
		const float    Dist      = FVector::Dist(PlayerLoc, OutHit.Location);
		
		const float TimeToHit = Dist / FMath::Max(ProjectileSpeed, 1.f); //in case speed isn't set somehow
		FVector POffset = Vel * LeadFactor * TimeToHit;
		POffset = POffset.GetClampedToMaxSize(MaxOffset);

		EquippedWeapon->SpawnBulletParticleFromWeaponMuzzle(OutHit.Location + POffset, true);
		return true;
	}
	
	return false;
}

void AEnemyCharacter::HandleMissedShot(FHitResult& OutHit, const FCollisionQueryParams& CollisionParams, const FVector& WeaponTraceStart, const FVector& WeaponTraceDirection, const FVector& WeaponTraceEnd, const FVector& PawnTraceHitPoint)
{
	const FVector WeaponTraceEndMiss = GetBulletMissedDirection(WeaponTraceDirection, WeaponTraceEnd, PawnTraceHitPoint);

	GetWorld()->LineTraceSingleByChannel(OutHit, WeaponTraceStart, WeaponTraceEndMiss, ECC_Camera, CollisionParams);
	
	if (OutHit.bBlockingHit && OutHit.GetActor())
	{
		BulletInteractionWithReactiveElements(OutHit);
		EquippedWeapon->SpawnBulletParticleFromWeaponMuzzle(OutHit.Location, false);
	}
	else
	{
		EquippedWeapon->SpawnBulletParticleFromWeaponMuzzle(WeaponTraceEndMiss, false);
	}
}

void AEnemyCharacter::GetFireHitResult(FHitResult &OutHit, const bool bMiss)
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return;
	}
	
	USkeletalMeshComponent* WeaponMesh = GetEquippedWeapon()->GetSkeletalMeshComponent().Get();
	
	if (!IsValid(WeaponMesh))
	{
		return;
	}
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (bHasShield)
	{
		CollisionParams.AddIgnoredActor(GetShield());
	}
	
	const FVector PawnTraceHitPoint = GetPawnTraceHitPoint(CollisionParams);
	
	if (!WeaponMesh->DoesSocketExist(TEXT("MuzzleSocket")))
	{
		return;
	}
	
	const FVector WeaponTraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	const FVector WeaponTraceDirection = (PawnTraceHitPoint - WeaponTraceStart).GetSafeNormal();
	const FVector WeaponTraceEnd = PawnTraceHitPoint + WeaponTraceDirection * EquippedWeapon->GetWeaponRange();
	
	if (!bMiss && HandleLandedShot(OutHit, CollisionParams, WeaponTraceStart, WeaponTraceEnd))
	{
		return;
	}
	
	HandleMissedShot(OutHit, CollisionParams, WeaponTraceStart, WeaponTraceDirection, WeaponTraceEnd, PawnTraceHitPoint);
}

FVector AEnemyCharacter::GetBulletMissedDirection(const FVector& WeaponTraceDirection, const FVector& WeaponTraceEnd, const FVector& PawnTraceHitPoint) const
{
	const FVector PlayerRight = FVector::CrossProduct(WeaponTraceDirection, FVector::UpVector);
	float RandomOffsetLeftRight;
	float RandomOffsetUp;
			
	if (FMath::RandRange(0, 1) < BULLET_MISS_LATERAL_PROBABILITY)
	{
		RandomOffsetLeftRight = FVector::Distance(PawnTraceHitPoint, GetActorLocation()) > IAConstants::PLAYER_DISTANCE_CHANGE_MISS_BULLET_LATERAL_OFFSET
			? FMath::RandRange(MinLeftRightOffsetLongDistance, MaxLeftRightOffsetLongDistance)
			: FMath::RandRange(MinLeftRightOffsetCloseDistance, MaxLeftRightOffsetCloseDistance);

		if (FMath::RandBool())
		{
			RandomOffsetLeftRight = -RandomOffsetLeftRight;
		}

		RandomOffsetUp = FMath::RandRange(MinUpOffsetLateralBullet, MaxUpOffsetLateralBullet);
	}
	else
	{
		RandomOffsetLeftRight = FMath::RandRange(MinLeftRightOffsetOverhead, MaxLeftRightOffsetOverhead);

		RandomOffsetUp = FMath::RandRange(MinUpOffsetOverhead, MaxUpOffsetOverhead);
	}
	
	if (IsValid(FmodWhooshComponentInterface))
	{
		constexpr uint8 SectionsToDivide = 6;
		const double Step = (MaxLeftRightOffsetOverhead - MinLeftRightOffsetOverhead) / SectionsToDivide;
		const double Shifted = RandomOffsetLeftRight - MinLeftRightOffsetOverhead;
		
		int32 Section = static_cast<int32>(Shifted / Step);
		Section = std::min(Section, SectionsToDivide - 1);

		const double SectionStart = MinLeftRightOffsetOverhead + Section * Step;
		const double SectionCenter = SectionStart + Step / 2.0;
		
		FmodWhooshComponentInterface->SetParameter("BulletPosition", SectionCenter);
		FmodWhooshComponentInterface->PlayEvent();
	}
		
	const FVector RandomLeftOrRightVector = PlayerRight * RandomOffsetLeftRight;
	const FVector RandomUpVector = FVector::UpVector * RandomOffsetUp;
			
	return WeaponTraceEnd + RandomUpVector + RandomLeftOrRightVector;
}

float AEnemyCharacter::CalculateMissChance() const
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return 1;
	}

	const float Distance = FVector::Distance(FGlobalPointers::PlayerCharacter->GetActorLocation(), GetActorLocation());
	if (Distance > MaxShootRange)
	{
		return 1;
	}
	const float DistanceAimingRelation = (Distance - MinShootRange) / (MaxShootRange - MinShootRange) * (MaxMissChance - MinMissChance) + MinMissChance;
	return FMath::Clamp(DistanceAimingRelation, MinMissChance, MaxMissChance);
}


// +-------------------------------------------------+
// |                                                 |
// |			   Bullet Interaction                |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::BulletInteraction(const FHitResult& OutHit) const
{
	if (!IsValid(OutHit.GetActor()))
	{
		return;
	}
	BulletInteractionWithReactiveElements(OutHit);
	if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(OutHit.GetActor()))
	{
		if (OutHit.GetActor()->IsA(APlayerCharacter::StaticClass()))
		{
			HealthInterface->SubtractHealth(GetEquippedWeapon()->CalculateWeaponDamage(OutHit.Distance),GetWeaponType(), this->GetActorLocation());
		}

		if (AUmbrella* UmbrellaObj = Cast<AUmbrella>(OutHit.GetActor()))
		{
			UmbrellaObj->SubtractHealth(GetEquippedWeapon()->CalculateWeaponDamage(OutHit.Distance), GetWeaponType());
			
			if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
			{
				FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::IMPACT, OutHit.Location, 0, 180);
			}
		}
	}
}

void AEnemyCharacter::BulletInteractionWithReactiveElements(const FHitResult& OutHit) const
{
	if (!bCanInteractWithReactiveElements || !IsValid(OutHit.GetActor()))
	{
		return;
	}
	if (OutHit.GetActor()->GetClass()->ImplementsInterface(UHitReactiveInterface::StaticClass()))
	{
		Cast<AReactiveElement>(OutHit.GetActor())->Reaction(HitSource::ENEMY);
	}
}


// +-------------------------------------------------+
// |                                                 |
// |                Electric Bullet                  |
// |                                                 |
// +-------------------------------------------------+

// void AEnemyCharacter::OnElectricShoot()
// {
// 	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(GetEquippedWeapon()))
// 	{
// 		return;
// 	}
//
// 	GetEquippedWeapon()->SpawnElectricCharge();
// 	FTimerDelegate Delegate;
// 	Delegate.BindUObject(this,&AEnemyCharacter::SpawnElectricProjectile);
// 	GetWorld()->GetTimerManager().SetTimer(ActivatingElectricDardDelegate,Delegate, GetEquippedWeapon()->GetElectricBulletChargeTime(), false);
// }

// void AEnemyCharacter::SpawnElectricProjectile()
// {
// 	if (GetStatusEffectComponent()->IsElectrified())
// 	{
// 		return;
// 	}
//
// 	const FVector PlayerLocation = FGlobalPointers::PlayerCharacter->GetActorLocation() + PLAYER_CHEST_OFFSET_FROM_ORIGIN;
// 	if (!IsValid(EquippedWeapon))
// 	{
// 		return;
// 	}
// 	
// 	// if (IsEnemyInFrontOf())
// 	// {
// 	// 	OnElectricBulletTokenRequest.Broadcast(false, this);
// 	// 	AbortElectricShoot();
// 	// }
// 	
// 	EquippedWeapon->SpawnElectricProjectile(PlayerLocation, this);
// 	ResetElectricShootLogic();
// }

// void AEnemyCharacter::ResetElectricShootLogic() const
// {
// 	EquippedWeapon->StopElectricCharge();
// 	EquippedWeapon->SetElectricBulletShooted(true);
// 	EquippedWeapon->SetElectricBulletActivated(false);
// 	EquippedWeapon->SetWeaponBulletType(EBulletType::Normal);
// }

// void AEnemyCharacter::AbortElectricShoot()
// {
// 	ResetElectricShootLogic();
// 	GetWorld()->GetTimerManager().ClearTimer(ActivatingElectricDardDelegate);
// }

// +-------------------------------------------------+
// |                                                 |
// |                     Montages                    |
// |                                                 |
// +-------------------------------------------------+

void AEnemyCharacter::Reload()
{
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance) && ReloadMontage)
	{
		SetIsReloading(true);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnReloadCompleted);
		AnimInstance->Montage_Play(ReloadMontage);
		if (IsValid(GetVoiceLineComponent()))
		{
			GetVoiceLineComponent()->QueueVoiceLine(
				FVoiceLineDTO
				{
					EVoiceLineState::RELOADING, EVoiceLineActor::ENEMY,  this
				}
			);
		}
	}
	else
	{
		EquippedWeapon->Reload(false);
	}
}

void AEnemyCharacter::PlayReactMontage()
{
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!IsValid(AnimInstance) || AnimInstance->Montage_IsPlaying(CurrentDamageAnimMontage) || AnimInstance->OnMontageEnded.IsBound())
	{
		return;
	}

	if (DamagedAnimMontages.IsEmpty())
	{
		return;
	}
	
	Algo::RandomShuffle(DamagedAnimMontages);
	CurrentDamageAnimMontage = DamagedAnimMontages.Last();
	
	if (IsValid(AnimInstance) && IsValid(CurrentDamageAnimMontage))
	{
		PauseAILogic("Damaged");
		AnimInstance->Montage_Play(CurrentDamageAnimMontage);
		AnimInstance->Montage_SetPlayRate(CurrentDamageAnimMontage, ReactMontagePlayRate);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnDamagedMontageCompleted);
	}
}

void AEnemyCharacter::PlayKickedMontage()
{
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (IsValid(AnimInstance) && IsValid(FAnimationAssets::EnemyKickKnockbackMontage))
	{
		AnimInstance->Montage_Play(FAnimationAssets::EnemyKickKnockbackMontage);
		AnimInstance->Montage_SetPlayRate(FAnimationAssets::EnemyKickKnockbackMontage, KickMontagePlayRate);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyCharacter::OnKickedMontageCompleted);

		Steam::UnlockAchievement(ESteamAchievement::KICKSTARTING);
		
		if (FGlobalPointers::GameInstance->GetVaultStuns() >= VAULT_STUNS_TO_ACHIEVEMENTS)
			Steam::UnlockAchievement(ESteamAchievement::THIS_IS_ITALIA);
		
		FTimerHandle KickTimerHandle;
		FTimerDelegate KickTimerDelegate;
		KickTimerDelegate.BindLambda([this]()
		{
			if (bEnableDebugMessages)
			{
				FLogger::DebugLog("Stun Ended");
			}
			if (IsValid(AIController) && IsValid(AIController->BrainComponent))
			{
				AIController->BrainComponent->ResumeLogic(TEXT("EndStun"));
			}
		});

		GetWorld()->GetTimerManager().SetTimer(KickTimerHandle, KickTimerDelegate, KickStunDuration, false);
	}
}

void AEnemyCharacter::OnReloadCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ReloadMontage)
	{
		SetIsReloading(false);
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnReloadCompleted);
		EquippedWeapon->Reload(false);
	}
}

void AEnemyCharacter::OnDamagedMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == CurrentDamageAnimMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnDamagedMontageCompleted);
		ResumeAILogic("Damaged");
	}
}

void AEnemyCharacter::OnKickedMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == FAnimationAssets::EnemyKickKnockbackMontage)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AEnemyCharacter::OnKickedMontageCompleted);
	}
}

void AEnemyCharacter::OnShieldDepleted()
{
	if (!IsValid(GetVoiceLineComponent()))
	{
		return;
	}
	GetVoiceLineComponent()->QueueVoiceLine(
		FVoiceLineDTO
		{
			EVoiceLineState::SHIELD_DEPLETED, EVoiceLineActor::ENEMY,  this,true
		}
	);
}
