// 
// Umbrella.h
// 
// Implementation of the Umbrella class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Umbrella.h"

#include "CableComponent.h"
#include "NiagaraComponent.h"
#include "UmbrellaLocation.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/CameraStateKey.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Components/MeleeComponent/MeleeComponent.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeComponent.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeDatatableRow.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Environment/GrapplingElements/HookImpactPoint.h"
#include "LadyUmbrella/Environment/ReactiveElements/DetachableFallingElement.h"
#include "LadyUmbrella/Environment/ReactiveElements/ElectricItems/ElectricPuzzle.h"
#include "LadyUmbrella/Interfaces/HitReactiveInterface.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "LadyUmbrella/Weapons/Umbrella/Gadgets/ElectricProjectile.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/TransformUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FUmbrellaDataDriven.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

AUmbrella::AUmbrella()
{
	static const wchar_t* SkeletalMeshAsset   = TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_Umbrella_New.SK_Umbrella_New'");
	
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SkeletalMeshAsset).Object);
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	// +-------------------------------------------------+
	// |                                                 |
	// |                  Sounds Init                    |
	// |                                                 |
	// +-------------------------------------------------+
	
	FmodShootingComponent			  = CreateDefaultSubobject<UFModComponentInterface>("Fmod Shooting Audio Component");
	FmodGadgetsComponent			  = CreateDefaultSubobject<UFModComponentInterface>("Fmod Gadgets Audio Component");
	FModImpactBulletsComponent		  = CreateDefaultSubobject<UFModComponentInterface>("FMod Impact Bullets Component");
	FmodFallingShellsEvent			  = CreateDefaultSubobject<UFModComponentInterface>("Fmod Falling Shells Event");
	FmodOpenCloseUmbrella			  = CreateDefaultSubobject<UFModComponentInterface>("Fmod Open Close Umbrella Event");
	FmodUmbrellaShieldHealthComponent = CreateDefaultSubobject<UFModComponentInterface>("Fmod Umbrella Shield Health Component");
	FmodHookImpactComponent			  = CreateDefaultSubobject<UFModComponentInterface>("Fmod Hook Impact Component");
	FmodShieldOpenedComponent		  = CreateDefaultSubobject<UFModComponentInterface>("Fmod Shield Opened Component");

	UFMODEvent* CannotOpenShield = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_BROKEN.UMBRELLA_BROKEN'");
	FmodUtils::TryAddEvent(CannotOpenShield, FmodUmbrellaShieldHealthComponent);
	
	UFMODEvent* EquipElectricGadgetEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_ELECTRIC_EQUIP.GADGET_ELECTRIC_EQUIP'");
	FmodUtils::TryAddEvent(EquipElectricGadgetEvent, FmodGadgetsComponent);
	
	UFMODEvent* HookImpactEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_HOOK_IMPACT.GADGET_HOOK_IMPACT'");
	FmodUtils::TryAddEvent(HookImpactEvent, FmodHookImpactComponent);
	
	UFMODEvent* ShootElectricGadgetEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_ELECTRIC_SHOOT.GADGET_ELECTRIC_SHOOT'");
	FmodUtils::TryAddEvent(ShootElectricGadgetEvent, FmodGadgetsComponent);

	UFMODEvent* UnlockElectricGadget = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_ELECTRIC_UNLOCK.GADGET_ELECTRIC_UNLOCK'");
	FmodUtils::TryAddEvent(UnlockElectricGadget, FmodGadgetsComponent);
	
	UFMODEvent* EquipHookGadget = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_HOOK_EQUIP.GADGET_HOOK_EQUIP'");
	FmodUtils::TryAddEvent(EquipHookGadget, FmodGadgetsComponent);

	UFMODEvent* ShootHookEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_HOOK_SHOOT.GADGET_HOOK_SHOOT'");
	FmodUtils::TryAddEvent(ShootHookEvent, FmodGadgetsComponent);

	UFMODEvent* UnlockHookEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/Gadgets/GADGET_HOOK_UNLOCK.GADGET_HOOK_UNLOCK'");
	FmodUtils::TryAddEvent(UnlockHookEvent, FmodGadgetsComponent);
	
	UFMODEvent* ShootEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_SHOOT.UMBRELLA_SHOOT'");
	FmodUtils::TryAddEvent(ShootEvent, FmodShootingComponent);
	
	UFMODEvent* ImpactOnBulletsEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_BULLET_IMPACTS.UMBRELLA_BULLET_IMPACTS'");
	FmodUtils::TryAddEvent(ImpactOnBulletsEvent, FModImpactBulletsComponent);
	
	UFMODEvent* FallingShells = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_SHELLS_FALLING.UMBRELLA_SHELLS_FALLING'");
	FmodUtils::TryAddEvent(FallingShells, FmodFallingShellsEvent);
	
	UFMODEvent* UmbrellaOpenCloseEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_OPEN_CLOSE_SHIELD.UMBRELLA_OPEN_CLOSE_SHIELD'"); 
	FmodUtils::TryAddEvent(UmbrellaOpenCloseEvent, FmodOpenCloseUmbrella);

	UFMODEvent* UmbrellaShieldOpened = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_SHIELD_OPEN.UMBRELLA_SHIELD_OPEN'"); 
	FmodUtils::TryAddEvent(UmbrellaShieldOpened, FmodShieldOpenedComponent);

	// Shield Health Fmod Events
	
	UFMODEvent* ShieldHealthReloadEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_REGENERATE.UMBRELLA_REGENERATE'");
	FmodUtils::TryAddEvent(ShieldHealthReloadEvent, FmodUmbrellaShieldHealthComponent);

	UFMODEvent* ShieldHealthBrokenEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Umbrella/UMBRELLA_BREAK.UMBRELLA_BREAK'");
	FmodUtils::TryAddEvent(ShieldHealthBrokenEvent, FmodUmbrellaShieldHealthComponent);
	
	// +-------------------------------------------------+
	// |                                                 |
	// |               End Sounds Init                   |
	// |                                                 |
	// +-------------------------------------------------+

	ProtectionCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ProtectionCollisionComponent"));
	ProtectionCollisionComponent->SetupAttachment(SkeletalMeshComponent);
	ProtectionCollisionComponent->SetRelativeTransform(TransformUtils::TransformLocation(0.0f, 127.0f, 0.0f));
	ProtectionCollisionComponent->SetBoxExtent(FVector(191, 17, 170));
	ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProtectionCollisionComponent->SetNotifyRigidBodyCollision(true);
	ProtectionCollisionComponent->SetHiddenInGame(true);
	
	bOpen    	= true;
	bOnTarget    = true;
	bIsShooting = false;
	bIsPumping = false;
	bCanShootElectric = true;
	bInfiniteAmmo = false;
	bCanShootGrapple = true;
	bDrawDebugBulletDispersion = false;
	bCantOpenBrokenUmbrellaDueToLowHealth = false;

	AmmoCurrent        					 = 4;
	AmmoCurrentMax     					 = 4;
	AmmoReserve        					 = 20;
	AmmoReserveMax     					 = 20;
	BulletDamage       					 = 2.0f;
	NotAimingBulletDispersion			 = 500;
	BulletRange							 = 4000.0f;
	MinReloadTime               		 = 0.1f;
	MaxReloadTime               		 = 10.0f;
	ReloadTime		   					 = 0.5f;
	StartReloadingAnimPlayRate			 = 1.0f;
	ReloadAnimPlayRate 					 = 1.0f;
	ReloadPumpAnimPlayRate				 = 1.0f;
	OpenCloseUmbrellaPlayRate			 = 1.0f;
	ShootingCadenceTime					 = 0.5f;
	MaxNrBulletPerShot 					 = 6;
	BulletPatternSize  					 = 3.0f;
	VolumeMultiplier   					 = 1.0f;
	WidgetTraceRadius  					 = 15.0f;
	HookRange          					 = 1000;
	SpawnForwardMargin 					 = 20.f;
	MinHealthPercentageToAllowShieldOpen = 0.2f;
	ElectricDartAimRange		= 8000.f;
	Bounciness					= 1.0f;
	HookSphereRadius = 30.f;

	GadgetTypeStructMap.Emplace(EGadgetType::Grapple , false);
	GadgetTypeStructMap.Emplace(EGadgetType::Electric, false);
	
	GadgetType		   = EGadgetType::Grapple;
	
	bCanShootGrapple   = true;
	bIsGrapplingCharacter = false;
	SpreadAngleX 		= 5.0f;
	SpreadAngleZ 		= 5.0f;
	PelletCylinderRadius = 30.0f;
	PelletDamageMultiplier 	= 1.0f;
	ElectricCooldownDuration = 5.0f;
	GrappleCooldownDuration = 5.0f;
	ClosingUmbrellaAnimationPlayRateMultiplier = 1.1f;
	MorphTarget1To2TransitionValue = 0.40f;
	MorphTarget2To3TransitionValue = 0.55f;
	MorphTarget3To4TransitionValue = 0.8f;
	DestroyedParticlesOffset = 100.0f;
	bHasHitGenericCharacter = false;
	MuzzleFlashRotationOffset = FRotator(0,0,-90.0f);
	
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	CableComponent->SetupAttachment(SkeletalMeshComponent);
	CableComponent->SetVisibility(false);
	CableComponent->SetComponentTickEnabled(false);
	CableComponent->SetRelativeTransform(TransformUtils::TransformLocation(0.0f, 130.0f, 1.0f));
	CableComponent->EndLocation = FVector::ZeroVector;
	
	MuzzleArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	MuzzleArrow->SetupAttachment(SkeletalMeshComponent);
	MuzzleArrow->SetRelativeLocation(FVector(0.0f,145.0f,0.0f));
	MuzzleArrow->SetRelativeRotation(FRotator(0,90.0f,90.0f));
	
	static ConstructorHelpers::FClassFinder<AElectricProjectile> ProjectileBPClass(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Spawnables/Projectiles/BP_AgencyElectricProjectile.BP_AgencyElectricProjectile_C'"));
	if (ProjectileBPClass.Succeeded())
	{
		ElectricProjectileClass = ProjectileBPClass.Class;
	}
	
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveAsset(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_DistanceDamageModifier.C_DistanceDamageModifier'"));
	if (CurveAsset.Succeeded())
	{
		DistanceDamageModifier = CurveAsset.Object;
	}
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetCanRegenerate(true);
	
	UpgradeComponent = CreateDefaultSubobject<UUpgradeComponent>(TEXT("UpgradeComponent"));

	MeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("MeleeComponent"));
	ParticleSpawnerComponent = CreateDefaultSubobject<UParticleSpawnerComponent>(TEXT("ParticleSpawnerComponent"));
	
	DataTableRowHandler.DataTable = AssetUtils::FindObject<UDataTable>(
		"/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_Umbrella.DT_LadyUmbrellaSystems_Umbrella'");

	if (IsValid(DataTableRowHandler.DataTable))
	{
		DataTableRowHandler.RowName = "BP_Umbrella";
	}
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactParticlePath(TEXT("/Game/Particles/CascadeParticleSystems/CPS_Explosion.CPS_Explosion"));
	if (ImpactParticlePath.Succeeded())
	{
		ImpactParticleSystem = ImpactParticlePath.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BulletImpactParticlePath(TEXT("/Game/Particles/CascadeParticleSystems/CPS_ShieldImpact2.CPS_ShieldImpact2"));
	if (BulletImpactParticlePath.Succeeded())
	{
		BulletImpactParticleSystem = BulletImpactParticlePath.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DestroyedParticlePath(TEXT("/Game/Particles/CascadeParticleSystems/CPS_ShieldBroken.CPS_ShieldBroken"));
	if (DestroyedParticlePath.Succeeded())
	{
		DestroyedParticleSystem = DestroyedParticlePath.Object;
	}

	OpenUmbrellaShieldTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("OpenUmbrellaShieldTimelineComponent"));

	// To be moved to a grapple component
	GrappleStartTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("GrappleStartTimeline"));

	UmbrellaSmokeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("UmbrellaSmokeNiagaraComponent"));
	UmbrellaSmokeNiagaraComponent->SetAutoActivate(false);
	UmbrellaSmokeNiagaraComponent->SetWorldLocation(MuzzleArrow->GetComponentLocation());

	ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>( UGameplayStatics::GetGameInstance(GetWorld()));
	if ( !IsValid(GameInstance))
	{
		return;
	}

	SetAmmoCurrentMax(GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMaxAmmo);
	SetAmmoCurrent(GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaCurrentMagAmmo);
	SetAmmoReserve(GameInstance->GetSaveGameFile()->GetPlayerData().UmbrellaInventoryAmmo);
}

void AUmbrella::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (IsValid(PlayerCharacter))
	{
		CameraComponent = PlayerCharacter->FindComponentByClass<UCameraComponent>();
	}
	
	ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bOpen = false;
	bOnTarget = false;

	if (IsValid(HealthComponent))
	{
		HealthComponent->OnHealthAmountChanged.AddDynamic(this, &AUmbrella::UmbrellaHealthChangedUpdate);
	}

	// To be moved to a grapple component
	if (IsValid(GrappleStartTimeline))
	{
		GrappleStartTimeline->SetPlayRate(UMBRELLA_OPENING_BASE_PLAY_RATE);
		
		FOnTimelineFloat GrappleStartProgressUpdate;
		GrappleStartProgressUpdate.BindUFunction(this, FName("GrappleStartTimelineProgress"));

		FOnTimelineEvent SwingFinishedEvent;
		SwingFinishedEvent.BindUFunction(this, FName("GrappleStartTimelineFinish"));
		
		if (IsValid(GrappleStartCurve))
		{
			GrappleStartTimeline->AddInterpFloat(GrappleStartCurve, GrappleStartProgressUpdate);
			GrappleStartTimeline->SetTimelineFinishedFunc(SwingFinishedEvent);
		}
	}

	if (IsValid(OpenUmbrellaShieldTimelineComponent) && IsValid(UmbrellaOpeningCurve))
	{
		OpenUmbrellaShieldTimelineComponent->SetPlayRate(1.0f);
		
		FOnTimelineFloat OpenShieldProgressUpdate;
		OpenShieldProgressUpdate.BindUFunction(this, FName("SetPercentageOpenedShield"));
		OpenUmbrellaShieldTimelineComponent->AddInterpFloat(UmbrellaOpeningCurve, OpenShieldProgressUpdate);
	}

	// We create a Dynamic Material Instance of our Material.
	if (IsValid(SkeletalMeshComponent) && IsValid(SkeletalMeshComponent->GetMaterial(UMBRELLA_INSIDE_MATERIAL_INDEX)))
	{
		DynamicInstanceInsideUmbrellaMaterial = UMaterialInstanceDynamic::Create(SkeletalMeshComponent->GetMaterial(UMBRELLA_INSIDE_MATERIAL_INDEX), this);
		if (IsValid(DynamicInstanceInsideUmbrellaMaterial))
		{
			SkeletalMeshComponent->SetMaterial(UMBRELLA_INSIDE_MATERIAL_INDEX, DynamicInstanceInsideUmbrellaMaterial);
			DynamicInstanceInsideUmbrellaMaterial->SetScalarParameterValue(UMBRELLA_HEALTH_PARAMETER_NAME, 1 - GetHealth() / GetMaxHealth());
		}
	}
	
	ResetGrapple();
	RecalculateBulletSpreadPositions();

	LoadUmbrellaFromDataTable();

	if (IsValid(UmbrellaSmokeNiagaraComponent))
	{
		UmbrellaSmokeNiagaraComponent->AttachToComponent(MuzzleArrow, FAttachmentTransformRules::KeepRelativeTransform);
	}

	OnUmbrellaUpgradeDelegate.BindUObject(this, &AUmbrella::ApplyUpgrade);
	
	UAnimInstance* PlayerAnimInstance = nullptr;
	if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetMesh()))
	{
		PlayerAnimInstance = FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance();
	}
	
	if (IsValid(PlayerAnimInstance) && !PlayerAnimInstance->OnPlayMontageNotifyBegin.IsAlreadyBound(this, &AUmbrella::OnMontageNotifyBegin))
	{
		PlayerAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AUmbrella::OnMontageNotifyBegin);
	}
}

void AUmbrella::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsAiming() && !bOpen)
	{
		TArray<FHitResult> HitsBulletRange = PerformSphereTrace(WidgetTraceRadius, BulletRange,EGadgetType::None);
		bOnTarget = IsOnTargetHit(HitsBulletRange,EGadgetType::None);
		bOnTargetGadget = false;
		if (!bOnTarget)
		{
			if (GadgetTypeStructMap[EGadgetType::Grapple] && GadgetType == EGadgetType::Grapple && bCanShootGrapple)
			{
				TArray<FHitResult> HitsHookRange = PerformSphereTrace(HookSphereRadius, HookRange,EGadgetType::Grapple);
				bOnTargetGadget = IsOnTargetHit(HitsHookRange,EGadgetType::Grapple);
			}
			if (GadgetTypeStructMap[EGadgetType::Electric] && GadgetType == EGadgetType::Electric && bCanShootElectric)
			{
				TArray<FHitResult> HitsDardRange = PerformSphereTrace(WidgetTraceRadius, ElectricDartAimRange,EGadgetType::Electric);
				bOnTargetGadget = IsOnTargetHit(HitsDardRange,EGadgetType::Electric);
			}
		}
	}
	if (bIsShooting && CableComponent->IsVisible())
	{
		if (GrappleStartTimeline->IsPlaying())
		{
			GrappleStartTimeline->Stop();
		}
		if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GrappleHitActor))
		{
			Enemy->EndGrapplingPosition();
		}
		
		ResetGrapple();
	}
	
	(void) OnUmbrellaHealthChanged.ExecuteIfBound(HealthComponent->GetHealth(), HealthComponent->GetMaxHealth());
	OnUmbrellaStateChanged.Broadcast(IsAiming(), IsOpen(), IsOnTarget(),IsOnTargetGrapple());
	(void) OnGadgetSwap.ExecuteIfBound(GadgetType);
	(void) OnGrappleCooldown.ExecuteIfBound(bCanShootGrapple, GrappleCooldownTimerHandle, GrappleCooldownDuration);
	(void) OnElectricDartCooldown.ExecuteIfBound(bCanShootElectric, ElectricCooldownTimerHandle, ElectricCooldownDuration);
	(void) OnPelletHit.ExecuteIfBound(bHasHitGenericCharacter, EnemiesHitByPellets);
	OnShootingCadence.Broadcast(ShootingTimer);
}

void AUmbrella::RecalculateBulletSpreadPositions()
{
	SpawnPattern.Empty();
	
	for (int32 i = 0; i < MaxNrBulletPerShot; ++i)
	{
		SpawnPattern.Add(FRotator(0, i * (360 / MaxNrBulletPerShot), 0).Vector() * BulletPatternSize);
	}
}

void AUmbrella::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName.IsEqual("AddOneShell"))
	{
		FGlobalPointers::PlayerCharacter->SetIsReloading(true);
		(void) OnCurrentBulletsChanged.ExecuteIfBound(FGlobalPointers::PlayerCharacter->IsReloading(), GetAmmoCurrent(), GetAmmoCurrentMax() , GetAmmoReserve());
		(void) OnTotalBulletsChanged.ExecuteIfBound(GetAmmoReserve());
	}

	if (NotifyName.IsEqual("CanShoot"))
	{
		ResetShooting();
		OnCanShoot.Broadcast(bIsShooting);
		
		if (AmmoCurrent <= 0)
		{
			StartReloading();
		}
	}
}

void AUmbrella::OnReloadMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && (Montage == StartReloadingMontage || Montage == ReloadMontage))
	{
		FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AUmbrella::OnReloadMontageCompleted);

		if (bInterrupted)
		{
			GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
			FGlobalPointers::PlayerCharacter->SetIsReloading(false);
			(void) OnCurrentBulletsChanged.ExecuteIfBound(FGlobalPointers::PlayerCharacter->IsReloading(), GetAmmoCurrent(), GetAmmoCurrentMax() , GetAmmoReserve());
		}
	}
}

void AUmbrella::StartPump()
{
	if (bIsPumping)
	{
		return;
	}
	
	if (FGlobalPointers::PlayerCharacter->GetPlayerGeneralState() != EPlayerState::InTraversalNoControls &&
		FGlobalPointers::PlayerCharacter->PlayMontage(ReloadPumpMontage, ReloadPumpAnimPlayRate) &&
		IsValid(FGlobalPointers::PlayerCharacter->GetMesh()) &&
		IsValid(FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()))
	{
		bIsPumping = true;

		if (!FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.IsAlreadyBound(this, &AUmbrella::OnReloadPumpMontageEnd))
			FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AUmbrella::OnReloadPumpMontageEnd);
	}
	else
	{
		ResetShooting();
		OnCanShoot.Broadcast(bIsShooting);
		if (AmmoCurrent <= 0)
		{
			StartReloading();
		}
	}
}

bool AUmbrella::ShootingUmbrellaGrapple()
{
	if (!bCanShootGrapple)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grapple still in cooldown"));
		return false;
	}
	
	const UWorld* World = GetWorld();
    if (!IsValid(World))
    {
    	return false;
    }
	
	bCanShootGrapple = false;
	
	World->GetTimerManager().SetTimer(GrappleCooldownTimerHandle, this, &AUmbrella::ResetGrappleCooldown, GrappleCooldownDuration, false);
	
	const APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(World, 0));

	if (!IsValid(CameraManager))
	{
		return false;
	}
	
	const FVector  CameraLocation   = CameraManager->GetCameraLocation();
	const FRotator CameraRotation   = CameraManager->GetCameraRotation();
	const FVector CameraFrontVector = CameraRotation.Vector();

	const FVector StartPoint = CameraLocation + CameraFrontVector * 100;
	const FVector Endpoint   = CameraLocation + CameraFrontVector * HookRange;
	
	// if (bDrawDebugBulletDispersion)
	// {
	// 	DrawDebugLine(World, CameraLocation, Endpoint, FColor::Green, false, 1, 0, 1);
	// }
	
	FHitResult GrappleHit(ForceInit);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor( this );

	// This has to be changed to a reference to the player directly instead, may be inefficient.
	QueryParams.AddIgnoredActor(FGlobalPointers::PlayerCharacter);
	
	if (IsValid(ParticleSpawnerComponent))
	{
		ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::HookShoot,MuzzleArrow->GetComponentLocation(), MuzzleArrow->GetComponentRotation());
	}
	
	if (IsValid(FmodGadgetsComponent))
	{
		if (!FmodGadgetsComponent->SetCurrentEvent("GADGET_HOOK_SHOOT"))
		{
			return false;
		}
	}

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(HookSphereRadius);

	bool bDidHookHit = World->SweepSingleByChannel(
		GrappleHit,
		StartPoint,
		Endpoint,
		FQuat::Identity,
		ECC_Camera,
		Sphere,
		QueryParams
	);

	if (bDrawDebugBulletDispersion)
	{
		const FVector DebugPos = (bDidHookHit ? GrappleHit.ImpactPoint : Endpoint);
		DrawDebugSphere(World, DebugPos, HookSphereRadius, 12, FColor::Green, false, 10.0f, 0, 5.0f);
	}

	//bool bDidHookHit = World->LineTraceSingleByChannel(GrappleHit, StartPoint, Endpoint, ECC_Camera, QueryParams); 
	FmodGadgetsComponent->SetParameter("DID_HOOK_HIT", !bDidHookHit);

	if (!bDidHookHit)
	{
		FmodGadgetsComponent->PlayEvent();
		return false;
	}

	AActor* HitActor = GrappleHit.GetActor();
	
	bDidHookHit = HitActor->Implements<UHookInterface>();

	FmodGadgetsComponent->SetParameter("DID_HOOK_HIT", !bDidHookHit);
	FmodGadgetsComponent->PlayEvent();
	
	if (!IsValid(HitActor) || !HitActor->Implements<UHookInterface>())
	{
		return false;
	}
	
	if (IsValid(FmodHookImpactComponent) && IsValid(HitActor))
    {
    	FmodHookImpactComponent->GetFModAudioComponent()->AttachToComponent(HitActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    	FmodHookImpactComponent->PlayEvent();
    }
    	
	GrappleHitActor = HitActor;
	bIsGrapplingCharacter = GrappleHitActor->IsA(AGenericCharacter::StaticClass());
	GrappleEndPoint = GrappleHit.ImpactPoint;
	GrappleHitComponent = GrappleHit.GetComponent();
	CableComponent->SetVisibility(true);
	CableComponent->SetComponentTickEnabled(true);
	// move cable until contact point

	TObjectPtr<AEnemyCharacter> Enemy = Cast<AEnemyCharacter>(HitActor);
	
	if (IsValid(Enemy) && !Enemy->HasBeenHookedBefore())
	{
		Enemy->SetHasBeenHookedBefore(true);
		HookEnemy();
	}
	
	if (IsValid(GrappleStartTimeline))
	{
		GrappleStartTimeline->PlayFromStart();
		return true;
	}
	
	return true;
}

TArray<FHitResult> AUmbrella::PerformSphereTrace(float TraceRadius, float TraceLength,EGadgetType GadgetTypeEnum)
{
	TArray<FHitResult> HitResults;

	if (TraceRadius == 0 || TraceLength == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("TraceRadius or TraceLength Missing! in Umbrella.cpp // PerformSphereTrace"));
		return HitResults;
	}

	if (!IsValid(PlayerCharacter))
	{
		return HitResults;
	}
	
	APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	if (!IsValid(PlayerController))
	{
		return HitResults;
	}
	
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceStart = CameraLocation;
	const FVector TraceEnd = TraceStart + (CameraRotation.Vector() * TraceLength);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(PlayerCharacter);

	const EDrawDebugTrace::Type DebugDrawType = EDrawDebugTrace::None;

	if (GadgetTypeEnum != EGadgetType::None) //electric dard and grapple
	{
		FHitResult HitResult;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel3));
		UKismetSystemLibrary::SphereTraceSingle(
this,
				TraceStart,
				TraceEnd,
				TraceRadius,
				UEngineTypes::ConvertToTraceType(ECC_Camera),
				false,
				IgnoredActors,
				DebugDrawType,
				HitResult,
				true
			);
		HitResults.Add(HitResult);
		return HitResults;
	}
	//shotgun umbrella
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		TraceStart,
		TraceEnd,
		TraceRadius,
		ObjectTypes,
		false,
		IgnoredActors,
		DebugDrawType,
		HitResults,
		true
	);

	return HitResults;
}

void AUmbrella::UmbrellaHealthChangedUpdate(int32 NewHealth, bool bLostHealth)
{
	const float HealthPercentageLeft = GetHealth() / GetMaxHealth();

	// Updating the Umbrella Health in the Inside Material
	if (IsValid(DynamicInstanceInsideUmbrellaMaterial) && IsValid(UmbrellaShieldHealthVisualsCurve))
	{
		const float EasedHealthPercentageLeft = UmbrellaShieldHealthVisualsCurve->GetFloatValue(HealthPercentageLeft);
		DynamicInstanceInsideUmbrellaMaterial->SetScalarParameterValue(UMBRELLA_HEALTH_PARAMETER_NAME, 1 - EasedHealthPercentageLeft);
	}

	// If we just increased our Health over the Threshold where the player can open it again.
	if (bCantOpenBrokenUmbrellaDueToLowHealth && HealthPercentageLeft > MinHealthPercentageToAllowShieldOpen)
	{
		bCantOpenBrokenUmbrellaDueToLowHealth = false;
		NotifyUmbrellaIsNoLongerBroken();
	}

	// Closing the umbrella if our health is over.
	if (GetHealth() <= 0)
	{
		OnBrokenUmbrella.Broadcast();
		
		bCantOpenBrokenUmbrellaDueToLowHealth = true;
		
		// Destroyed FX
		if (DestroyedParticleSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyedParticleSystem, MuzzleArrow->GetComponentLocation() + MuzzleArrow->GetForwardVector() * DestroyedParticlesOffset)->SetWorldScale3D(FVector(1.0f));
		}

		if (IsValid(PlayerCharacter))
		{
			PlayerCharacter->TryToCloseShield();
		}

		if (!IsValid(FmodUmbrellaShieldHealthComponent))
		{
			FLogger::ErrorLog("AUmbrella::UmbrellaHealthChangedUpdate FmodUmbrellaShieldHealthComponent->GetFmodEvent Failed");
			return;
		}
		if (!FmodUmbrellaShieldHealthComponent->SetCurrentEvent(UmbrellaBrokenFmodEventName))
		{
			FLogger::ErrorLog("AUmbrella::UmbrellaHealthChangedUpdate FmodUmbrellaShieldHealthComponent->SetCurrentEvent Failed");
			return;
		}
		FmodUmbrellaShieldHealthComponent->PlayEvent();
	}
}

void AUmbrella::ResetHasHitGenericCharacter()
{
	bHasHitGenericCharacter = false;
	for (AGenericCharacter* Enemy : EnemiesHitByPellets)
	{
		Enemy->GetHitPositions().Empty();
	}
	EnemiesHitByPellets.Empty();
}

void AUmbrella::SetPercentageOpenedShield(const float Value)
{
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}
	
	const float MorphValue4 = FMath::Clamp((Value - MorphTarget3To4TransitionValue) / (1.0f - MorphTarget3To4TransitionValue),0, 1);
	const float MorphValue3 = FMath::Clamp(FMath::Clamp((Value - MorphTarget2To3TransitionValue) / (MorphTarget3To4TransitionValue - MorphTarget2To3TransitionValue),0, 1) - MorphValue4, 0, 1);
	const float MorphValue2 = FMath::Clamp(FMath::Clamp((Value - MorphTarget1To2TransitionValue) / (MorphTarget2To3TransitionValue - MorphTarget1To2TransitionValue),0, 1) - MorphValue3 - MorphValue4, 0, 1);
	const float MorphValue1 = FMath::Clamp(FMath::Clamp(Value / MorphTarget1To2TransitionValue,0, 1) - MorphValue3 - MorphValue4 - MorphValue2, 0, 1);
	
	SkeletalMeshComponent->SetMorphTarget(UMBRELLA_OPENING_MORPH_TARGET_4, MorphValue4);
	SkeletalMeshComponent->SetMorphTarget(UMBRELLA_OPENING_MORPH_TARGET_3, MorphValue3);
	SkeletalMeshComponent->SetMorphTarget(UMBRELLA_OPENING_MORPH_TARGET_2, MorphValue2);
	SkeletalMeshComponent->SetMorphTarget(UMBRELLA_OPENING_MORPH_TARGET_1, MorphValue1);
}

void AUmbrella::UpdateVisualAmmoOnLoad() const
{
	FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
	FGlobalPointers::MainController->TogglePlayerHUD();
	(void) OnUmbrellaAmmoCurrentMax.ExecuteIfBound(AmmoCurrentMax);
	(void) OnUmbrellaLoadAmmo.ExecuteIfBound(AmmoCurrent, AmmoCurrentMax, AmmoReserve);
	(void) OnTotalBulletsChanged.ExecuteIfBound(GetAmmoReserve());
	FGlobalPointers::MainController->UnTogglePlayerHUD();
}

void AUmbrella::UpdateVisualGadgetsOnLoad() const
{
	if (IsValid(FGlobalPointers::MainController))
	{
		FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
		FGlobalPointers::MainController->TogglePlayerHUD();
		for (const TPair<EGadgetType, bool>& Pair : GadgetTypeStructMap)
		{
			OnUnlockingGadget.ExecuteIfBound(Pair.Key, *GadgetTypeStructMap.Find(Pair.Key));
		}
		FGlobalPointers::MainController->UnTogglePlayerHUD();
	}
}

void AUmbrella::PlayFallingShellSound() const
{
	FmodFallingShellsEvent->PlayEvent();
}

void AUmbrella::PlayOpenCloseShield(const uint8 Value) const
{
	FmodOpenCloseUmbrella->StopEvent();
	FmodOpenCloseUmbrella->SetParameter(UmbrellaOpenFmodParameterName, Value);
	FmodOpenCloseUmbrella->PlayEvent();

	if (Value)
	{
		FGlobalPointers::PlayerCharacter->PlayMontage(CloseUmbrellaMontage, OpenCloseUmbrellaPlayRate);
	}
	else
	{
		FGlobalPointers::PlayerCharacter->PlayMontage(OpenUmbrellaMontage, OpenCloseUmbrellaPlayRate);
	}
}

void AUmbrella::ApplyUpgrade(const EUpgradeType Type)
{
	float NextUpgrade = 0.0f;
	const FUpgrade* Upgrade = UpgradeComponent->Get(Type);
	
	TArray<FUpgradeDatatableRow*> UpgradeDatatable;
	FDatatableAssets::UpgradesDatatable->GetAllRows(FString(), UpgradeDatatable);
	
	for (FUpgradeDatatableRow* Row : UpgradeDatatable)
	{
		if (Row->UpgradeType == Type)
		{
			NextUpgrade = GetUpgradeMultiplier(Upgrade, Row);
			break;
		}
	}
	
	switch (Type)
	{
	case EUpgradeType::AmmoCapacity :
		AmmoCurrent    = NextUpgrade;
		AmmoCurrentMax = NextUpgrade;
		AmmoReserve    = AmmoReserveMax;
		OnMultiCurrentBulletsChanged.Broadcast();
		UpdateVisualAmmoOnLoad();
		break;
	case EUpgradeType::Handling :
		FGlobalPointers::PlayerCharacter->ChangeHandling(NextUpgrade);
		break;
	case EUpgradeType::FireRate :
		ReloadAnimPlayRate     = (1.0f / NextUpgrade);
		ReloadPumpAnimPlayRate = (1.0f / NextUpgrade);
		break;
	case EUpgradeType::ReloadSpeed :
		ReloadTime = NextUpgrade;
		break;
	// case EUpgradeType::Bounciness :
	// 	Bounciness = NextUpgrade;
	// 	break;
	case EUpgradeType::Endurance :
		HealthComponent->SetMaxHealth(NextUpgrade);
		break;
	// case EUpgradeType::ShieldMotion :
	// 	FGlobalPointers::PlayerCharacter->ChangeShieldMotion(NextUpgrade);
	// 	break;
	case EUpgradeType::ShieldRegen :
		HealthComponent->SetRegenerationSpeed(NextUpgrade);
		break;
	default:
		break;
	}
}

float AUmbrella::GetUpgradeMultiplier(const FUpgrade* Upgrade, const FUpgradeDatatableRow* Row) const
{
	if (Upgrade->GetLevel() <= 1) return Row->First;
	if (Upgrade->GetLevel() == 2) return Row->Second;
	if (Upgrade->GetLevel() == 3) return Row->Third;
	if (Upgrade->GetLevel() == 4) return Row->Fourth;

	return Row->Fifth;
}

void AUmbrella::OnReloadPumpMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsValid(Montage) && Montage == ReloadPumpMontage && IsValid(FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()))
	{
		bIsPumping = false;
		FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &AUmbrella::OnReloadPumpMontageEnd);
		if (bInterrupted && bIsShooting && !GetWorldTimerManager().IsTimerActive(ShootingTimer))
		{
			ResetShooting();
			OnCanShoot.Broadcast(bIsShooting);
		
			if (AmmoCurrent <= 0)
			{
				StartReloading();
			}
		}
	}
}

void AUmbrella::NotifyUmbrellaIsNoLongerBroken() const
{
	// Do things here.
	if (!FmodUmbrellaShieldHealthComponent->SetCurrentEvent(UmbrellaRegeneratedFmodEventName))
	{
		FLogger::ErrorLog("AUmbrella::NotifyUmbrellaIsNoLongerBroken FmodUmbrellaShieldHealthComponent->SetCurrentEvent Failed");
	}
	FmodUmbrellaShieldHealthComponent->PlayEvent();
}

bool AUmbrella::TryToOpenShield()
{
	if (!IsValid(HealthComponent) || bCantOpenBrokenUmbrellaDueToLowHealth || IsOpen())
	{
		if (FmodUmbrellaShieldHealthComponent->SetCurrentEvent("UMBRELLA_BROKEN"))
		{
			FmodUmbrellaShieldHealthComponent->PlayEvent();
		}
		return false;
	}

	if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::SHIELD);
	}

	bOpen = true;

	if (IsValid(FmodShieldOpenedComponent))
	{
		FmodShieldOpenedComponent->SetParameter("bIsOpenUmbrella", bOpen);
		FmodShieldOpenedComponent->PlayEvent();
	}
	
	// Triggering the opening umbrella animation.
	if (IsValid(OpenUmbrellaShieldTimelineComponent))
	{
		OpenUmbrellaShieldTimelineComponent->SetPlayRate(UMBRELLA_OPENING_BASE_PLAY_RATE);
		OpenUmbrellaShieldTimelineComponent->Play();
	}

	// Setting the umbrella's shield collision.
	if (IsValid(ProtectionCollisionComponent))
	{
		ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ProtectionCollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
		// ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		// ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		// ProtectionCollisionComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	}

	// Calling our delegate.
	OnIsCoveredByUmbrellaChanged.Broadcast(bOpen);
	
	PlayOpenCloseShield(UmbrellaOpenFmodParameterValueOpen);
	return true;
}
 
bool AUmbrella::TryToCloseShield()
{
	if (!IsOpen())
	{
		return false;
	}
	
	if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
	{
		FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->SetCameraState(ECameraStateKey::AIM);
	}
	
	bOpen = false;

	if (IsValid(FmodShieldOpenedComponent))
	{
		FmodShieldOpenedComponent->SetParameter("bIsOpenUmbrella", bOpen);
		FmodShieldOpenedComponent->PlayEvent();
	}
	
	// Triggering the closing umbrella animation.
	if (IsValid(OpenUmbrellaShieldTimelineComponent))
	{
		OpenUmbrellaShieldTimelineComponent->SetPlayRate(UMBRELLA_OPENING_BASE_PLAY_RATE * ClosingUmbrellaAnimationPlayRateMultiplier);
		OpenUmbrellaShieldTimelineComponent->Reverse();
	}

	// Disabling the umbrella's shield collision.
	if (IsValid(ProtectionCollisionComponent))
	{
		ProtectionCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Calling our delegate.
	OnIsCoveredByUmbrellaChanged.Broadcast(bOpen);
	PlayOpenCloseShield(UmbrellaOpenFmodParameterValueClosed);
	return true;
}

bool AUmbrella::TryToShoot()
{
	UWorld* World = GetWorld();
	// Nothing happens if we were already shooting or the umbrella is Open.
	if (bIsShooting || IsOpen() || !IsValid(CameraComponent) || !IsValid(World) || bIsPumping)
	{
		return false;
	}

	// If we try to shoot but have no bullets, then we try to Reload.
	if (AmmoCurrent <= 0)
	{
		StartReloading();
		return false;
	}

	// Blocking our ability to shoot for the cadence duration.
	bIsShooting = true;
	if (IsValid(ParticleSpawnerComponent))
	{
		ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::BulletCasing,SkeletalMeshComponent->GetSocketLocation("CasingEjectSocket"), FRotator::ZeroRotator);
	}

	OnCanShoot.Broadcast(bIsShooting);
	
	// Setting the timer to Reset our bIsShooting variable to false.
	GetWorldTimerManager().SetTimer(
		ShootingTimer,
		this,
		&AUmbrella::StartPump,
		ShootingCadenceTime,
		false,
		ShootingCadenceTime
	);

	// Playing the shoot sound
	FmodShootingComponent->PlayEvent();
	World->GetTimerManager().SetTimer(ShellFallingTimerHandle, this, &ThisClass::PlayFallingShellSound, 0.7f, false);
	
	const FVector StartCamera = CameraComponent->GetComponentLocation();
	const FVector StartMuzzle = MuzzleArrow->GetComponentLocation();

	// Playing Muzzleflash
	if (IsValid(ParticleSpawnerComponent))
	{
		ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::MuzzleflashUmbrella,StartMuzzle, this->GetActorRotation() + MuzzleFlashRotationOffset);
	}
	
	FCollisionQueryParams ShootingCollisionQueryParams;
	ShootingCollisionQueryParams.AddIgnoredActor(this);
	if (IsValid(PlayerCharacter))
	{
		ShootingCollisionQueryParams.AddIgnoredActor(PlayerCharacter);
	}

	// Reducing our ammo count.
	if (!bInfiniteAmmo)
	{
		AmmoCurrent--;

		if (IsValid(FGlobalPointers::GameInstance))
			FGlobalPointers::GameInstance->AddBulletShotInLevel();

		(void) OnCurrentBulletsChanged.ExecuteIfBound(FGlobalPointers::PlayerCharacter->IsReloading(), GetAmmoCurrent(), GetAmmoCurrentMax() , GetAmmoReserve());
	}
	
	FColor DebugColor = FColor(150, 150, 150);
	
	for (int32 i = 0; i < MaxNrBulletPerShot; ++i)
	{
		// Random spread
		float AngleX = FMath::FRandRange(-SpreadAngleX, SpreadAngleX);
		float AngleZ = FMath::FRandRange(-SpreadAngleZ, SpreadAngleZ);

		FRotator SpreadRot = CameraComponent->GetComponentRotation();
		SpreadRot.Yaw	+= AngleX;
		SpreadRot.Pitch += AngleZ;

		//FVector Start = CameraComponent->GetComponentLocation();
		FVector Start	= StartCamera;
		FVector End		= Start + SpreadRot.Vector() * BulletRange;

		FHitResult Hit;
		const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, ShootingCollisionQueryParams);
				
		if (bHit && Hit.bBlockingHit)
		{
			AActor* HitActor = Hit.GetActor();
			if (!IsValid(HitActor)) continue;

			if (bDrawDebugBulletDispersion)
			{
				DrawDebugLine(World, StartMuzzle, Hit.Location, DebugColor, false, 0.5f, 0, 1.0f);
			}

			// Impact FX
			if (IsValid(ParticleSpawnerComponent))
			{
				ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::NonLethalAmmoHit, Hit.Location, FRotator::ZeroRotator);
			} // TODO: Maybe change non lethal ammo hit to some smoke or whatever @PAlexUtad
			
			if(!HitActor->Implements<UHealthInterface>() && !HitActor->Implements<UHitReactiveInterface>())
			{
				continue;
			}

			if (AGenericCharacter* Enemy = Cast<AGenericCharacter>(HitActor))
			{
				SetHasHitGenericCharacter(true);
				Enemy->GetHitPositions().Add(Hit.Location);
				EnemiesHitByPellets.Add(Enemy);
				if (Enemy->GetHealthComponent()->IsDead())
				{
					if (IsValid(ParticleSpawnerComponent))
					{
						ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::LethalAmmoHit, Hit.Location, Hit.GetActor()->GetActorRotation());
					}
				}
			}
			
			// Health logic
			if (IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitActor))
			{
				float Distance = FVector::Dist(Hit.Location, GetActorLocation());
				float DistanceMod = DistanceDamageModifier ? DistanceDamageModifier->GetFloatValue(Distance / BulletRange) : 1.0f;

				// Cylinder boundaries
				FVector CylinderStart = CameraComponent->GetComponentLocation();
				FVector CylinderEnd = CylinderStart + CameraComponent->GetForwardVector() * BulletRange;

				bool bInsideCylinder = IsPointInCylinder(CylinderStart, CylinderEnd, PelletCylinderRadius, Hit.Location);
				float Damage = BulletDamage * DistanceMod * (bInsideCylinder ? PelletDamageMultiplier : 1.0f);
				
				if (bDrawDebugBulletDispersion)
				{
					if (bInsideCylinder)
					{
						UE_LOG(LogTemp, Warning, TEXT("Pellet %d hit inside cylinder: Damage = %.2f (Multiplied)"), i, Damage);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Pellet %d hit outside cylinder: Damage = %.2f"), i, Damage);
					}
				}
				HealthInterface->SubtractHealth(Damage, EWeaponType::Umbrella);
			}
			
			if (IHitReactiveInterface* HitReactive = Cast<IHitReactiveInterface>(HitActor))
			{
				HitReactive->Reaction(HitSource::PLAYER_B);
			}
			
		}
		else if (bDrawDebugBulletDispersion)
		{
			DrawDebugLine(World, StartMuzzle, End, DebugColor, false, 0.5f, 0, 1.0f);
		}
	}
	
	// Spawning Smoke VFX
	if (IsValid(UmbrellaSmokeNiagaraComponent))
	{
		UmbrellaSmokeNiagaraComponent->ActivateSystem();
	}
	
	if (bHasHitGenericCharacter)
	{
		FTimerHandle HitMarkersTimer;
		GetWorldTimerManager().SetTimer(
			HitMarkersTimer,
			this,
			&AUmbrella::ResetHasHitGenericCharacter,
			ShootingCadenceTime,
			false,
			ShootingCadenceTime
		);
	}

	// draw the debug cylinder for visualization (it's a capsule idk how to draw an actual cylinder) :( 
	if (bDrawDebugBulletDispersion)
	{
		FVector CapsuleStart = CameraComponent->GetComponentLocation();
		FVector CapsuleEnd = CapsuleStart + CameraComponent->GetForwardVector() * BulletRange;
		FVector CapsuleCenter = (CapsuleStart + CapsuleEnd) * 0.5f;
		float HalfHeight = BulletRange * 0.5f;

		FVector CapsuleAxis = (CapsuleEnd - CapsuleStart).GetSafeNormal();
		FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(CapsuleAxis).ToQuat();
/*
		DrawDebugCapsule(
			World,
			CapsuleCenter,
			HalfHeight,
			PelletCylinderRadius,
			CapsuleRotation,
			FColor::Blue,
			false,
			2.0f,
			0,
			1.0f
			);
*/
	}

	
	return true; // We return true since we did shoot bullets. 
}

bool AUmbrella::IsPointInCylinder(const FVector& Start, const FVector& End, float Radius, const FVector& Point)
{
	const FVector Axis = End - Start;
	const FVector StartToPoint = Point - Start;

	const float AxisDot = FVector::DotProduct(StartToPoint, Axis.GetSafeNormal());
	if (AxisDot < 0 || AxisDot > Axis.Size())
	{
		return false;
	}
	
	const FVector ClosestPoint = Start + Axis.GetSafeNormal() * AxisDot;
	const float DistanceToAxis = FVector::Dist(ClosestPoint, Point);

	return DistanceToAxis <= Radius;
}

bool AUmbrella::GadgetShoot()
{
	// This searches in the map of gadgets if the gadget trying to shoot is unlocked or not
	if (!*GadgetTypeStructMap.Find(GadgetType))
	{
		return false;
	}
	
	switch (GadgetType)
	{
		case EGadgetType::Grapple:
			return ShootingUmbrellaGrapple();
		case EGadgetType::Electric:
			return OnElectricShoot();
		default:
			return false;
	}
}

bool AUmbrella::OnElectricShoot()
{
	if (!bCanShootElectric)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dart still in cooldown"));
		return false;
	}
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}
	bCanShootElectric = false;

	World->GetTimerManager().SetTimer(ElectricCooldownTimerHandle, this, &AUmbrella::ResetElectricCooldown, ElectricCooldownDuration, false);
	
	const FVector StartCamera = CameraComponent->GetComponentLocation();
	const FVector EndCamera = StartCamera + CameraComponent->GetForwardVector() * ElectricDartAimRange;

	// Need this so the projectile doesn't collide with the shield (Surely there is a more elegant solution)
	const FVector SpawnLocation = MuzzleArrow->GetComponentLocation() + MuzzleArrow->GetForwardVector() * SpawnForwardMargin;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(PlayerCharacter);

	FHitResult Hit;
	bool bHit = World->LineTraceSingleByChannel(Hit, StartCamera, EndCamera, ECC_Pawn, QueryParams);

	const FVector ProjectileDirection = bHit ? (Hit.Location - SpawnLocation).GetSafeNormal() : (EndCamera - SpawnLocation).GetSafeNormal();

	// Making the Player the dart's instigator.
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	if (IsValid(PlayerCharacter))
	{
		SpawnParameters.Owner		= PlayerCharacter;
		SpawnParameters.Instigator	= PlayerCharacter;
	}

	TObjectPtr<AElectricProjectile> Projectile =
		World->SpawnActor<AElectricProjectile>(
			ElectricProjectileClass,
			SpawnLocation,
			ProjectileDirection.Rotation(),
			SpawnParameters
		);

	if (!Projectile->OnShockUniqueEnemy.IsBound())
		Projectile->OnShockUniqueEnemy.BindUObject(this, &AUmbrella::ShockEnemy);

	if (FmodGadgetsComponent->SetCurrentEvent("GADGET_ELECTRIC_SHOOT"))
	{
		FmodGadgetsComponent->PlayEvent();
	}
	
	return true;
}

void AUmbrella::HookEnemy()
{
	if (!GetWorldTimerManager().IsTimerActive(HookAchievementTimer))
	{
		HookAchievementCount = 0;
		
		GetWorldTimerManager().SetTimer(
			HookAchievementTimer,
			this,
			&AUmbrella::HookAchievementCheck,
			HOOK_ACHIEVEMENT_TIMER
		);
	}

	HookAchievementCount++;
	Steam::UnlockAchievement(ESteamAchievement::ZUPA_DI_PESCE);
}

void AUmbrella::ShockEnemy()
{
	if (!GetWorldTimerManager().IsTimerActive(ShockAchievementTimer))
	{
		ShockAchievementCount = 0;
		
		GetWorldTimerManager().SetTimer(
			ShockAchievementTimer,
			this,
			&AUmbrella::ShockAchievementCheck,
			SHOCK_ACHIEVEMENT_TIMER
		);
	}

	ShockAchievementCount++;
	Steam::UnlockAchievement(ESteamAchievement::MAN_IN_BLUE);
}

void AUmbrella::HookAchievementCheck() const
{
	if (HookAchievementCount >= HOOK_ACHIEVEMENT_COUNT)
		Steam::UnlockAchievement(ESteamAchievement::FISHING_ROUTINE);
}

void AUmbrella::ShockAchievementCheck() const
{
	if (ShockAchievementCount >= SHOCK_ACHIEVEMENT_COUNT)
		Steam::UnlockAchievement(ESteamAchievement::ZIP_ZAP_ZUP);
}

void AUmbrella::CycleBulletType()
{	
	if (GadgetTypeStructMap[EGadgetType::Grapple] && GadgetTypeStructMap[EGadgetType::Electric])
	{
		GadgetType = GadgetType == EGadgetType::Grapple ? EGadgetType::Electric : EGadgetType::Grapple;
		const FString GadgetName = static_cast<bool>(GadgetType) ?  GadgetElectricName : GadgetHookName;

		if (IsValid(FGlobalPointers::MainController))
		{
			if (IsValid(FGlobalPointers::MainController->GetMainHUD()))
			{
				FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
				if (!FGlobalPointers::MainController->GetMainHUD()->GetPlayerHUDState())
				{
					FGlobalPointers::MainController->TogglePlayerHUD();
				}
				if (!IsAiming())
				{
					FGlobalPointers::MainController->DeactivatePlayerHUDOnTimer();
				}
			}
		}
		(void)FmodGadgetsComponent->SetCurrentEvent(GadgetName);
		FmodGadgetsComponent->PlayEvent();
	
		SpawnGadgetCycleParticle();
	}
}

void AUmbrella::SpawnGadgetCycleParticle() const
{
	if (!IsValid(ParticleSpawnerComponent))
	{
		return;
	}
	
	const FVector ParticleLocation = SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle");
	switch (GadgetType)
	{
	case EGadgetType::Grapple:
		ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::HookEquip, ParticleLocation, FRotator::ZeroRotator);
		break;
	case EGadgetType::Electric:
		ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::DartEquip, ParticleLocation, FRotator::ZeroRotator); // TODO: Take umbrella rotation into account for cylindrical particles @PALEXUTAD
		break;
	default: break;
	}
}

void AUmbrella::SetBulletType(EGadgetType TargetGadget)
{
	GadgetType = TargetGadget;
}

bool AUmbrella::IsOnTargetHit(const TArray<FHitResult>& HitResults,EGadgetType IsHookGadget)
{
	if (HitResults.IsEmpty())
	{
		return false;
	}
	
	const AActor* FirstHitResult = HitResults[0].GetActor();
	for (const FHitResult& Hit : HitResults)
	{
		const AActor* HitActor = Hit.GetActor(); 
		if (!IsValid(HitActor))
		{
			continue;
		}

		bool IsGenericActor = false;
		const AGenericCharacter* Character = Cast<AGenericCharacter>(HitActor);
		if (IsValid(Character))
		{
			IsGenericActor = !Character->IsDead();
		}
		
		bool ImplementsHookInterface = HitActor->IsA(AHookImpactPoint::StaticClass()) || HitActor->ActorHasTag("AgencyShield");
		bool ImplementsElectricInterface = HitActor->IsA(AElectricPuzzle::StaticClass());
		bool ActorCanBeHittedByShoot = IsGenericActor || (HitActor->IsA(AReactiveElement::StaticClass()) && !HitActor->IsA(AElectricElement::StaticClass()));

		if (HitActor == FirstHitResult)
		{
			if (IsHookGadget == EGadgetType::Grapple && (ImplementsHookInterface || IsGenericActor))
			{
				return true;
			}
			if (IsHookGadget == EGadgetType::Electric && (ImplementsElectricInterface || IsGenericActor))
			{
				return true;
			}
		}

		if (IsHookGadget == EGadgetType::None  && ActorCanBeHittedByShoot)
		{
			return true;
		}
	}

	return false;
}

void AUmbrella::UnlockGadgets(const EGadgetType GadgetToUnlock)
{
	if (GadgetTypeStructMap.Contains(GadgetToUnlock))
	{
		if (IsValid(FGlobalPointers::MainController))
		{
			FGlobalPointers::MainController->ActivateResetHidePlayerHUDTimer();
			FGlobalPointers::MainController->TogglePlayerHUD();
			*GadgetTypeStructMap.Find(GadgetToUnlock) = true;
			OnUnlockingGadget.ExecuteIfBound(GadgetToUnlock, *GadgetTypeStructMap.Find(GadgetToUnlock));
			FGlobalPointers::MainController->UnTogglePlayerHUD();
		}
	}
}

void AUmbrella::SetAmmoCurrent(const int32 Amount)
{
	AmmoCurrent = Amount;

	if (IsValid(FGlobalPointers::PlayerCharacter)) // Cinematics can crash Umbrella if played from Editor as FGlobalPointers::PlayerCharacter == nullptr.
	{
		(void) OnCurrentBulletsChanged.ExecuteIfBound(FGlobalPointers::PlayerCharacter->IsReloading(), GetAmmoCurrent(), GetAmmoCurrentMax() , GetAmmoReserve());
	}
}

void AUmbrella::SetAmmoCurrentMax(const int32 Amount)
{
	AmmoCurrentMax = Amount;
	(void) OnUmbrellaAmmoCurrentMax.ExecuteIfBound(AmmoCurrentMax);
}

void AUmbrella::SetAmmoReserve(const int32 Amount)
{
	AmmoReserve = Amount;
	(void) OnTotalBulletsChanged.ExecuteIfBound(GetAmmoReserve());
	OnMultiCurrentBulletsChanged.Broadcast();
}

void AUmbrella::AutoReload()
{
	if (AmmoCurrent < AmmoCurrentMax && AmmoReserve > 0)
	{
		const int32 Needed = AmmoCurrentMax - AmmoCurrent;
		if (AmmoReserve >= Needed)
		{
			AmmoCurrent = AmmoCurrentMax;
			AmmoReserve -= Needed;
		}
		else
		{
			AmmoCurrent += AmmoReserve;
			AmmoReserve = 0;
		}
	}
}

void AUmbrella::InterruptReload()
{
	if (!FGlobalPointers::PlayerCharacter->IsReloading())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	FGlobalPointers::PlayerCharacter->SetIsReloading(false);
}

//To be moved to grapple component
void AUmbrella::GrappleStartTimelineProgress(float Value)
{
	if (IsValid(GrappleHitActor))
	{
		if (bIsGrapplingCharacter)
		{
			GrappleEndPoint = GrappleHitActor->GetActorLocation();
		}
		const FVector StartPoint = SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle");
		const FVector NewPoint = FMath::Lerp(StartPoint,GrappleEndPoint , Value);
		
		SetCableEndLocation(NewPoint);
		
		if (NewPoint.Equals(GrappleEndPoint) && IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
		{
			FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::GADGET_RECOIL, GrappleEndPoint);
		}
	}
}

void AUmbrella::GrappleStartTimelineFinish()
{
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	if (bIsGrapplingCharacter && IsValid(ParticleSpawnerComponent))
	{
		ParticleSpawnerComponent->SpawnParticleByName(EParticleEffectType::HookHit,GrappleEndPoint, GrappleHitActor->GetActorRotation());
	}
	
	PlayerCharacter->PlayGrappleAnim();

	FVector SocketLocation = GetActorLocation();
	if (IsValid(SkeletalMeshComponent))
	{
		SocketLocation = SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle"); 
	}

	if (!GrappleHitActor->Implements<UHookInterface>())
	{
		return;
	}
	
	IHookInterface* HookInterface = Cast<IHookInterface>(GrappleHitActor);
	if (HookInterface)
	{
		switch ( HookInterface->GetGrabType() )
		{
		case IHookInterface::PULL:
			HookInterface->ContactWithCable(SocketLocation);
			break;
		case IHookInterface::PUSH:
			if (PlayerCharacter)
			{ 
				PlayerCharacter->MovePlayerViaGrapple(GrappleEndPoint);	
			}
			break;
		case IHookInterface::PULLWITHCOMPONENT:
			HookInterface->ContactWithCable(SocketLocation, GrappleHitComponent);
			break;
		default:
			break;
		}
	}
	if (IsValid(this))
	{
		ResetGrapple();
	}
}

void AUmbrella::LoadUmbrellaFromDataTable()
{
	const FUmbrellaDataDriven* UmbrellaData = FDataTableHelpers::ValidateTableAndGetData<FUmbrellaDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	// UmbrellaData not UObject
	if (!UmbrellaData)
	{
		FLogger::ErrorLog("Umbrella Data not loaded from Table [DataTableRowHandler.DataTable == nullptr]");
		return;
	}

	ShootingCadenceTime = UmbrellaData->FireRate;
	ReloadTime			= UmbrellaData->ReloadingTime;
	//AmmoCurrentMax	= UmbrellaData->MagazineCapacity;
	//AmmoCurrent		= AmmoCurrentMax;
	//AmmoReserve  		= UmbrellaData->StoredAmmoCapacity;
	//AmmoReserveMax	= AmmoReserve;
	BulletRange			= UmbrellaData->ShotgunMaxRange;
	//UmbrellaData->DamagePerProjectileMin;
	BulletDamage		= UmbrellaData->DamagePerProjectileMax;
	MaxNrBulletPerShot	= UmbrellaData->ProjectileCount;
	//UmbrellaData->DPS;
	//UmbrellaData->MaxShotDMG;
	PelletDamageMultiplier	 = UmbrellaData->DMGMultiplierCylinderRadius;
	HookRange				 = UmbrellaData->HookMaxRange;
	ElectricCooldownDuration = UmbrellaData->DartCooldown;
	
	HealthComponent->SetMaxHealth(UmbrellaData->ShieldEndurance);
	HealthComponent->SetHealth(UmbrellaData->ShieldEndurance);
	HealthComponent->SetRegenerationSpeed(UmbrellaData->ShieldRegenerationSpeed);
	HealthComponent->SetRegenerationCooldown(UmbrellaData->ShieldTimeToRegenerate);
	//UmbrellaData->StunTimePostShieldBrake;
	MeleeComponent->SetDamage(UmbrellaData->MeleeDMG);
	MeleeComponent->SetTraceRadius(UmbrellaData->MeleeMaxRange);
	//UmbrellaData->MeleeAttackCadence;
	//UmbrellaData->MeleeAttackCooldown;
	MeleeComponent->SetDamage(UmbrellaData->MeleeDMG);
	
}

void AUmbrella::ResetGrapple()
{
	bIsGrapplingCharacter = false;
	if (IsValid(CableComponent))
	{
		CableComponent->SetVisibility(false);
		CableComponent->SetComponentTickEnabled(false);
		if (IsValid(SkeletalMeshComponent))
		{
			SetCableEndLocation(SkeletalMeshComponent->GetSocketLocation("WeaponMuzzle"));
		}
	}
}

void AUmbrella::SetCableEndLocation(const FVector& EndPoint) const
{
	CableComponent->EndLocation = UKismetMathLibrary::InverseTransformLocation(this->GetActorTransform(), EndPoint);
}

void AUmbrella::StartReloading()
{
	const bool bAtMaxBullets = GetAmmoCurrent() == GetAmmoCurrentMax();
	const bool bHasBulletsToReload = GetAmmoReserve() <= 0;
	UAnimInstance* PlayerAnimInstance = FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance();
	
	if (!GetWorldTimerManager().IsTimerActive(ReloadTimerHandle)
		&& !bIsShooting && !bAtMaxBullets && !bHasBulletsToReload && FGlobalPointers::PlayerCharacter->GetPlayerGeneralState() != EPlayerState::InTraversalNoControls)
	{
		(void) OnUmbrellaReloadStateChanged.ExecuteIfBound(EUmbrellaLocation::Hand, true, false);
		FGlobalPointers::PlayerCharacter->PlayMontage(StartReloadingMontage, StartReloadingAnimPlayRate);
		if (IsValid(FGlobalPointers::PlayerCharacter->GetMesh()->GetAnimInstance()))
		{
			PlayerAnimInstance->OnMontageEnded.AddDynamic(this, &AUmbrella::OnReloadMontageCompleted);
		}

		GetWorldTimerManager().SetTimer(
			ReloadTimerHandle,
			this,
			&AUmbrella::ReloadOneShell,
			FMath::Clamp(ReloadTime, MinReloadTime, MaxReloadTime),
			true,
			StartReloadingMontage->GetPlayLength()
		);		
	}
}

void AUmbrella::ReloadOneShell()
{
	const bool bAtMaxBullets = GetAmmoCurrent() == GetAmmoCurrentMax();
	const bool bHasBulletsToReload = GetAmmoReserve() <= 0;
	
	if (bIsShooting || bAtMaxBullets || bHasBulletsToReload || FGlobalPointers::PlayerCharacter->GetPlayerGeneralState() == EPlayerState::InTraversalNoControls)
	{
		InterruptReload();
		(void) OnCurrentBulletsChanged.ExecuteIfBound(FGlobalPointers::PlayerCharacter->IsReloading(), GetAmmoCurrent(), GetAmmoCurrentMax() , GetAmmoReserve());
		StartPump();
		if (!IsAiming())
		{
			(void) OnUmbrellaReloadStateChanged.ExecuteIfBound(EUmbrellaLocation::Back, false, true);
		}
	}
	else if (FGlobalPointers::PlayerCharacter->IsReloading())
	{
		SetAmmoCurrent(GetAmmoCurrent() + 1);
		SetAmmoReserve(GetAmmoReserve() - 1);
		
		FGlobalPointers::PlayerCharacter->PlayMontage(ReloadMontage, ReloadAnimPlayRate);
		
		if (!FGlobalPointers::PlayerCharacter->IsPlayerInCombat())
		{
			return;
		}
		
		FGlobalPointers::PlayerCharacter->PlayVoiceLine(EVoiceLineState::RELOADING);
	}
}

void AUmbrella::SubtractHealth(const float HealthToSubtract, EWeaponType DamageType)
{
	if (!IsValid(HealthComponent))
	{
		return;
	}
	HealthComponent->SubtractHealth(HealthToSubtract);

	if (EWeaponType::Pistol == DamageType || EWeaponType::Rifle == DamageType)
	{
		FModImpactBulletsComponent->PlayEvent();
	}
}

void AUmbrella::SetHealth(const float NewHealth)
{
	HealthComponent->SetHealth(NewHealth);
}

const float AUmbrella::GetHealth() const
{
	return HealthComponent->GetHealth();
}

float AUmbrella::GetMaxHealth() const
{
	return HealthComponent->GetMaxHealth();
}