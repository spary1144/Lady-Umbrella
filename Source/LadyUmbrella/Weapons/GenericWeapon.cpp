// 
// GenericWeapon.cpp
// 
// Implementation of the GenericWeapon class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "GenericWeapon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "Umbrella/Umbrella.h"
#include "Umbrella/Gadgets/ElectricProjectile.h"

AGenericWeapon::AGenericWeapon()
{
	static const wchar_t* FireSoundAsset     = TEXT("/Script/Engine.SoundCue'/Game/Sounds/SoundCues/SC_GrenadeExplosion.SC_GrenadeExplosion'");
	//static const wchar_t* SkeletalMeshAsset  = TEXT("/Script/Engine.SkeletalMesh'/Game/Weapons/PlaceHolder/Rifle.Rifle'");

	AmmoCurrent    = 20;
	AmmoCurrentMax = 20;
	AmmoReserve    = 20;
	AmmoReserveMax = 20;
	WeaponCadence  = 0.4f;
	MaxWeaponCadence  = 1.f;
	MinWeaponCadence  = 0.2f;
	WeaponDamage   = 20.f;
	WeaponRange    = 1000;
	BulletReboundMaxDifference = 180.0f;
	WeaponBulletType = EBulletType::Normal;
	bIsElectricDartActivated = false;
	bIsElectricDartShooted = false;
	PartialChargingTimeElectricBall = 1.f;
	ChargingTimeElectricBullet = 2.f;
	LifeSpan = 10.f;
	MassKg = 4.f;
	MinAmmoToStartReloading = 0.f;
	ProjectileParticleSpeed = 1800.0f;
	TrailLifetimeAfterBulletIsDeactivated = 0.3f;
	RibbonID = 0;
	bDoBulletsReboundAfterHittingUmbrella = true;
	ReboundParticleRange = 500.0f;
	
	MaxWeaponDamage = 8;
    MinWeaponDamage = 1;
    CurveMiddlePoint = 750;
    DamageCurveLeftSlope = 0.08;
    DamageCurveRightSlope = 0.0035;
	ElectricChargeDisplacementFromMuzzlePosition = FVector(2.5f, 0.f, 1.f);
	MuzzleRotationOffset = FRotator(0.0f, 90, 90);

	FireSoundBase = ConstructorHelpers::FObjectFinder<USoundBase>(FireSoundAsset).Object;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("Capsule")));
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetRootComponent(Capsule);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	DataTableRowHandler.DataTable = AssetUtils::FindObject<UDataTable>("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_WeaponData.DT_LadyUmbrellaSystems_WeaponData'");

	MuzzleSmokeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleSmokeNiagaraComponent"));
	MuzzleSmokeNiagaraComponent->SetAutoActivate(false);

	ProjectileEffectsNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffectsNiagaraComponent"));
	ProjectileEffectsNiagaraComponent->SetAutoActivate(false);

	ElectricChargeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricChargeNiagaraComponent"));
	ElectricChargeNiagaraComponent->SetAutoActivate(false);

	MuzzleFlashNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlashNiagaraComponent"));
	MuzzleFlashNiagaraComponent->SetAutoActivate(false);

	MuzzleFlashNiagaraComponent->SetAsset(AssetUtils::FindObject<UNiagaraSystem>(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_Muzzleflash.NS_Muzzleflash'")));

	FireSoundFModComponent = CreateDefaultSubobject<UFModComponentInterface>("FMod Component Interface");
	
}

void AGenericWeapon::SimulatePhysicsBeforeDestroy()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	GetCapsule()->DestroyComponent();
	
	if (IsValid(SkeletalMeshComponent))
	{
		GetSkeletalMeshComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		SetRootComponent(SkeletalMeshComponent);	

		GetSkeletalMeshComponent()->BodyInstance.bLockZRotation = true;
		
		GetSkeletalMeshComponent()->SetAngularDamping(20.f);
		GetSkeletalMeshComponent()->SetLinearDamping(3.f);
		
		GetSkeletalMeshComponent()->SetMassOverrideInKg(NAME_None, MassKg, true);
		GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetSkeletalMeshComponent()->SetCollisionObjectType(ECC_PhysicsBody);
		GetSkeletalMeshComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	
		GetSkeletalMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
		GetSkeletalMeshComponent()->WakeAllRigidBodies();
		GetSkeletalMeshComponent()->SetEnableGravity(true);
		GetSkeletalMeshComponent()->SetSimulatePhysics(true);
	}
}

void AGenericWeapon::SpawnElectricCharge()
{
	if (IsValid(ElectricChargeNiagaraComponent))
	{
		ElectricChargeNiagaraComponent->ResetSystem();
		ElectricChargeNiagaraComponent->ActivateSystem();
		ElectricChargeNiagaraComponent->SetFloatParameter(FName("MaxChargingSphereScale"),0.3f);
		ElectricChargeNiagaraComponent->SetFloatParameter(FName("MinChargingSphereScale"),0.1f);

		FireSoundFModComponent->SetParameter(PARAMETER_NAME_DART_CHARGE, 0);
		FireSoundFModComponent->PlayEvent();
	}
}

void AGenericWeapon::StopElectricCharge()
{
	if (IsValid(ElectricChargeNiagaraComponent))
	{
		ElectricChargeNiagaraComponent->DeactivateImmediate();
	}
}

void AGenericWeapon::SpawnBulletParticle(const FVector& StartLocation, const FVector& EndLocation)
{
	if (!IsValid(ProjectileEffectsNiagaraComponent) || !IsValid(SkeletalMeshComponent))
	{
		return;
	}
	
	ProjectileEffectsNiagaraComponent->SetVariablePosition(BULLET_START_POSITION_VARIABLE_NAME, StartLocation);
	ProjectileEffectsNiagaraComponent->SetVariableFloat(BULLET_PROJECTILE_SPEED_VARIABLE_NAME, ProjectileParticleSpeed);
	ProjectileEffectsNiagaraComponent->SetVariableFloat(LIFETIME_AFTER_BULLET_DEACTIVATION_NIAGARA_VARIABLE_NAME, TrailLifetimeAfterBulletIsDeactivated);
	ProjectileEffectsNiagaraComponent->SetVariableInt(RIBBON_ID_NIAGARA_VARIABLE_NAME, RibbonID);

	// Setting a limit of how far bullets can travel.
	// This is useful in case a bullet is shot into the stratosphere, we don't want its trail to last forever.
	if (!(FVector::DistSquared(StartLocation, EndLocation) > FMath::Square(MAX_BULLET_PARTICLE_DISTANCE_TRAVELLED)))
	{
		ProjectileEffectsNiagaraComponent->SetVariablePosition(BULLET_END_POSITION_VARIABLE_NAME, EndLocation);
	}
	else
	{
		const FVector& NewEnd = StartLocation + (EndLocation - StartLocation).GetSafeNormal() * MAX_BULLET_PARTICLE_DISTANCE_TRAVELLED;
		ProjectileEffectsNiagaraComponent->SetVariablePosition(BULLET_END_POSITION_VARIABLE_NAME, NewEnd);
	}
	
	++RibbonID;

	ProjectileEffectsNiagaraComponent->ResetSystem();
	ProjectileEffectsNiagaraComponent->ActivateSystem();
}

void AGenericWeapon::SpawnElectricProjectile(const FVector& EndLocation, APawn* BulletInstigator)
{
	if (!IsValid(ElectricProjectileClass))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	if (IsValid(BulletInstigator))
	{
		SpawnParameters.Owner		= BulletInstigator;
		SpawnParameters.Instigator	= BulletInstigator;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	}
	
	const FVector ProjectileTraceStart = GetMuzzleSocketLocation();
	const FRotator ProjectileRotation = (EndLocation - ProjectileTraceStart).Rotation();
	
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		FLogger::DebugLog("Invalid World");
	}
	AElectricProjectile* SpawnedElectricProjectile = World->SpawnActor<AElectricProjectile>(ElectricProjectileClass, ProjectileTraceStart, ProjectileRotation, SpawnParameters);
	if (!IsValid(SpawnedElectricProjectile))
	{
		FLogger::DebugLog("Electric Projectile can be spawned");
	}

	FireSoundFModComponent->SetParameter(PARAMETER_NAME_DART_SHOOT, 0);
	FireSoundFModComponent->PlayEvent();
}

void AGenericWeapon::SpawnBulletParticleFromWeaponMuzzle(const FVector& EndLocation, const bool bHitsUmbrella)
{
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}

	if (IsValid(MuzzleFlashNiagaraComponent))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		MuzzleFlashNiagaraComponent->GetAsset(),
		SkeletalMeshComponent->GetSocketLocation(MUZZLE_SOCKET_NAME),
		this->GetActorRotation()+MuzzleRotationOffset,
		FVector(1.0f),
		true,
		true,
		ENCPoolMethod::AutoRelease,
		true
	);
	}
	
	SpawnBulletParticle(SkeletalMeshComponent->GetSocketLocation(MUZZLE_SOCKET_NAME), EndLocation);

	// If we hit the umbrella, we need to trigger logic when that happens.
	if (bHitsUmbrella)
	{
		FTimerDelegate TimerDelegate;
		FTimerHandle TemporaryTimerHandle;

		TimerDelegate.BindUFunction(this, "HandleBulletHitUmbrella", EndLocation);
		const float DistanceTravelledByBullet = FVector::Distance(EndLocation, SkeletalMeshComponent->GetSocketLocation(MUZZLE_SOCKET_NAME));
		const float TimeTakenToReachUmbrella = DistanceTravelledByBullet / ProjectileParticleSpeed;
		
		GetWorld()->GetTimerManager().SetTimer(TemporaryTimerHandle, TimerDelegate, TimeTakenToReachUmbrella, false);
	}
}

void AGenericWeapon::HandleBulletHitUmbrella(const FVector& HitLocation)
{
	float Difference = FVector::Distance(HitLocation,Umbrella->GetActorLocation());
	if (Difference > BulletReboundMaxDifference)
	{
		bDoBulletsReboundAfterHittingUmbrella = false;
	}
	
	if (bDoBulletsReboundAfterHittingUmbrella)
	{
		if (IsValid(Umbrella) && IsValid(Umbrella->GetBulletImpactParticle()) && Umbrella->IsOpen())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				Umbrella->GetBulletImpactParticle(),
				HitLocation,
				FRotationMatrix::MakeFromZ(Umbrella->GetActorRightVector()).Rotator(),
				FVector(0.15f),
				true);
		}

		
		const FVector RandomVector = FVector(
		FMath::FRandRange(-ReboundParticleRange, ReboundParticleRange),
		FMath::FRandRange(-ReboundParticleRange, ReboundParticleRange),
		FMath::FRandRange(-ReboundParticleRange, ReboundParticleRange));
		
		SpawnBulletParticle(HitLocation, HitLocation + RandomVector);
		
	}
}

void AGenericWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MuzzleSmokeNiagaraComponent) && IsValid(SkeletalMeshComponent))
	{
		MuzzleSmokeNiagaraComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
		MuzzleSmokeNiagaraComponent->SetWorldLocation(SkeletalMeshComponent->GetSocketLocation(MUZZLE_SOCKET_NAME));
		MuzzleSmokeNiagaraComponent->SetWorldRotation(SkeletalMeshComponent->GetSocketRotation(MUZZLE_SOCKET_NAME));
	}

	if (IsValid(SkeletalMeshComponent) && IsValid(ElectricChargeNiagaraComponent))
	{
		ElectricChargeNiagaraComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
		ElectricChargeNiagaraComponent->SetWorldRotation(SkeletalMeshComponent->GetSocketRotation(MUZZLE_SOCKET_NAME));
		ElectricChargeNiagaraComponent->SetWorldLocation(SkeletalMeshComponent->GetSocketLocation(MUZZLE_SOCKET_NAME));
		ElectricChargeNiagaraComponent->SetWorldScale3D(FVector::One());
		ElectricChargeNiagaraComponent->AddRelativeLocation(ElectricChargeDisplacementFromMuzzlePosition);
		ElectricChargeNiagaraComponent->SetVariableFloat(CHARGING_ELECTRIC_DART_SPHERE_LENGTH, PartialChargingTimeElectricBall);
		ElectricChargeNiagaraComponent->SetVariableFloat(CHARGING_ELECTRIC_DART_TOTAL_LENGTH, ChargingTimeElectricBullet);
	}
	
	// TODO: Fix this because this looks insane.
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(PlayerCharacter))
	{
		Umbrella = PlayerCharacter->GetWeapon();
	}
	
	//DataTableRowHandler.DataTable = AssetUtils::FindObject<UDataTable>("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_WeaponData.DT_LadyUmbrellaSystems_WeaponData'");
	//SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
}

void AGenericWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AGenericWeapon::Fire()
{
	if (!HasBullets())
	{
		return false;
	}

	AmmoCurrent--;
	
	if (IsValid(MuzzleSmokeNiagaraComponent))
	{
		MuzzleSmokeNiagaraComponent->ResetSystem();
	}

	const int32 RandParameter = FMath::RandRange(0, 9);

	if (!IsValid(FireSoundFModComponent) || !IsValid(FireSoundFModComponent->GetFmodEvent()))
	{
		FLogger::ErrorLog("AGenericWeapon::Fire FireSoundFModComponent is not valid");
		return false;
	}

	FireSoundFModComponent->SetParameter(*FmodEventParameterName, RandParameter);
	FireSoundFModComponent->PlayEvent();

	return true;
}

bool AGenericWeapon::Reload(const bool ConsumeAmmo)
{
	if (!CanReload())
	{
		return false;
	}

	// Calculates amount to load into the gun by using the formula:
	// 
	//     AmountToLoad = AmmoCurrentMax - AmmoCurrent
	// 
	// However, if the amount to load exceeds the AmmoReserve, then only load as
	// many bullets as the weapon has left.
	// 
	// For example:
	//     - If the weapon is currently at 10, and the max is 32, then the weapon
	//       needs to load 22 bullets, but the weapon only has 10 bullet reserve
	//       remaining. In this case the amount to load gets clamped to 10, and
	//       updates the current bullet amount to 20.
	const int32 Amount = AmmoCurrentMax - AmmoCurrent;
	const int32 AmountClamped = FMath::Clamp(Amount, 0, AmmoReserve);

	AmmoCurrent += AmountClamped;

	if (ConsumeAmmo)
	{
		AmmoReserve -= AmountClamped;
	}
	
	return true;
}

bool AGenericWeapon::CanReload() const
{
	const bool HasReserve = AmmoReserve > 0;
	const bool IsAtCurrentMax = AmmoCurrent >= AmmoCurrentMax;
	
	return HasReserve && !IsAtCurrentMax;
}

float AGenericWeapon::GetRandomWeaponCadence()
{
	WeaponCadence = FMath::RandRange(MinWeaponCadence, MaxWeaponCadence);
	return WeaponCadence;
}

float AGenericWeapon::CalculateWeaponDamage(const float Distance)
{
	if (Distance < CurveMiddlePoint)
	{
		WeaponDamage = MinWeaponDamage + (MaxWeaponDamage - MinWeaponDamage) * (1.f - (1.f / (1 + FMath::Exp(-DamageCurveLeftSlope * (Distance - CurveMiddlePoint)))));
	}
	else
	{
		WeaponDamage = MinWeaponDamage + (MaxWeaponDamage - MinWeaponDamage) * (1.f - (1.f / (1 + FMath::Exp(-DamageCurveRightSlope * (Distance - CurveMiddlePoint)))));
	}
	
	WeaponDamage = FMath::RoundHalfFromZero(WeaponDamage);
	/*FLogger::DebugLog("DamageCurveLeftSlope: %f", DamageCurveLeftSlope);
	FLogger::DebugLog("DamageCurveRightSlope: %f", DamageCurveRightSlope);
	FLogger::DebugLog("Weapon Damage: %f", WeaponDamage);
	FLogger::DebugLog("Distance: %f", Distance);*/
	return WeaponDamage;
}
