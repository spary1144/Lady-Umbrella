// Fill out your copyright notice in the Description page of Project Settings.


#include "AgencyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Components/StatusEffectComponent/StatusEffectComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FEnemyDataDriven.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"

FOnElectricBulletTokenRequest AAgencyCharacter::OnElectricBulletTokenRequest;
FOnFlankTokenRequest AAgencyCharacter::OnFlankTokenRequest;

AAgencyCharacter::AAgencyCharacter()
{
	SetWeaponType(EWeaponType::Pistol);
	MinNumRafagues = 3;
	MaxNumRafagues = 5;
	SetRandomNumRafagues();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // Hook
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block); // for the melee
	SpeedOnMoveBetweenCovers = 450.f;
	SpeedOnCloseCombat = 200.f;
	SpeedOnMoveAroundCover = 350.f;
	bHasElectricBullet = false;
	DataTableRowHandler.RowName = "BP_RegularAgencyCharacter";

	// Add Voice Lines
	// TODO: Move ALL asset loading to the ShaderMap.
	static UFMODEvent* Throwing = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_THROWING.AGENCY_THROWING'");
	FmodUtils::TryAddEvent(Throwing, GetFmodComponentVoiceLine());

	static UFMODEvent* ShootingFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_SHOOTING.AGENCY_SHOOTING'");
	FmodUtils::TryAddEvent(ShootingFmodEvent, GetFmodComponentVoiceLine());

	static UFMODEvent* ShieldDepleatedFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_SHIELD_DEPLETED.AGENCY_SHIELD_DEPLETED'");
	FmodUtils::TryAddEvent(ShieldDepleatedFmodEvent, GetFmodComponentVoiceLine());
	
	static UFMODEvent* ReloadingFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_RELOADING.AGENCY_RELOADING'");
	FmodUtils::TryAddEvent(ReloadingFmodEvent, GetFmodComponentVoiceLine());
	
	static UFMODEvent* BouncingFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_BOUNCING.AGENCY_BOUNCING'");
	FmodUtils::TryAddEvent(BouncingFmodEvent, GetFmodComponentVoiceLine());

	static UFMODEvent* CinematicFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_CINEMATIC_CHAPTER_3.AGENCY_CINEMATIC_CHAPTER_3'");
	FmodUtils::TryAddEvent(CinematicFmodEvent, GetFmodComponentVoiceLine());

	static UFMODEvent* DistractingFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_DISTRACTING.AGENCY_DISTRACTING'");
	FmodUtils::TryAddEvent(DistractingFmodEvent, GetFmodComponentVoiceLine());

	static UFMODEvent* EnemyInCoverFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_ENEMY_IN_COVER.AGENCY_ENEMY_IN_COVER'");
	FmodUtils::TryAddEvent(EnemyInCoverFmodEvent, GetFmodComponentVoiceLine());

	static UFMODEvent* FlankingFmodEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Agency/AGENCY_FLANKING.AGENCY_FLANKING'");
	FmodUtils::TryAddEvent(FlankingFmodEvent, GetFmodComponentVoiceLine());
	
}

void AAgencyCharacter::SetRandomNumRafagues()
{
	NumRafaguesRemaining = FMath::RandRange(MinNumRafagues,MaxNumRafagues);
}

void AAgencyCharacter::PlayFlankVoiceLine()
{
	UVoiceLineComponent* VoiceLineComp = GetVoiceLineComponent();
	
	if (!IsValid(VoiceLineComp))
	{
		return;
	}
	
	VoiceLineComp->QueueVoiceLine(
		FVoiceLineDTO{
			EVoiceLineState::FLANKING,EVoiceLineActor::AGENCY,
			this,true
		}
	);
}

void AAgencyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetFmodComponentVoiceLine()->InitializeMap();
	GetFmodComponentVoiceLine()->SetVoiceLineIdentifier(FMOD_EVENT_IDENTIFIER);
	
	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::AgencyVoiceLines);
	GetVoiceLineComponent()->SetOwnerName(OWNER_NAME);
	
	LoadEnemyFromDataTable();
}

void AAgencyCharacter::LoadEnemyFromDataTable()
{
	const FEnemyDataDriven* Data = FDataTableHelpers::ValidateTableAndGetData<FEnemyDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	if (!Data)
	{
		FLogger::ErrorLog("AgencyCharacter Data not loaded from Table [DataTableRowHandler.DataTable == nullptr]");
		return;
	}

	MinMissChance = Data->MinMissingChance;
	MaxMissChance = Data->MaxMissingChance;
	MinShootRange = Data->MinShootDistance;
	MaxShootRange = Data->MaxShootDistance;
	
	SpeedOnFindPlayerCovered = Data->OnFindPlayerCoveredSpeed;
	SpeedOnCloseCombat = Data->CloseCombatSpeed;
	SpeedOnMoveAroundCover = Data->MoveAroundCoverSpeed;
	SpeedOnMoveBetweenCovers = Data->MoveBetweenCoverSpeed;
	
	if (IsValid(GetHealthComponent()))
	{
		GetHealthComponent()->SetMaxHealth(Data->MaxHealth);
		GetHealthComponent()->SetHealth(Data->MaxHealth);
	}
	if (IsValid(GetCharacterMovement()))
	{
		GetCharacterMovement()->MaxWalkSpeed = Data->MaxSpeed;
	}
}

void AAgencyCharacter::SetDead()
{
	GetWorld()->GetTimerManager().ClearTimer(ActivatingElectricDardDelegate);
	Super::SetDead();
}

void AAgencyCharacter::OnStatusEffectChangeListener(const EStatusEffect StatusEffect, const bool bStunned)
{
	Super::OnStatusEffectChangeListener(StatusEffect, bStunned);
	if (GetStatusEffectComponent()->IsStunned())
	{
		AbortElectricShoot();
	}
}

void AAgencyCharacter::FireElectricProjectile()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || !IsValid(GetEquippedWeapon()))
	{
		return;
	}

	if (IsEnemyInFrontOf() || IsPlayerTurnHisBack() || IsReloading())
	{
		return;
	}

	GetEquippedWeapon()->SpawnElectricCharge();
	FTimerDelegate Delegate;
	Delegate.BindUObject(this,&AAgencyCharacter::SpawnElectricProjectile);
	GetWorld()->GetTimerManager().SetTimer(ActivatingElectricDardDelegate,Delegate, GetEquippedWeapon()->GetElectricBulletChargeTime(), false);
}

void AAgencyCharacter::SpawnElectricProjectile()
{
	if (GetStatusEffectComponent()->IsElectrified() || !IsValid(FGlobalPointers::PlayerCharacter))
	{
		return;
	}

	const FVector PlayerLocation = FGlobalPointers::PlayerCharacter->GetActorLocation() + PLAYER_CHEST_OFFSET_FROM_ORIGIN;
	if (!IsValid(EquippedWeapon))
	{
		return;
	}
	
	if (IsEnemyInFrontOf() || IsPlayerTurnHisBack() || IsReloading())
	{
		OnElectricBulletTokenRequest.Broadcast(false, this);
		AbortElectricShoot();
	}
	
	EquippedWeapon->SpawnElectricProjectile(PlayerLocation, this);
	ResetElectricShootLogic();
}

void AAgencyCharacter::ResetElectricShootLogic() const
{
	EquippedWeapon->StopElectricCharge();
	EquippedWeapon->SetElectricBulletShooted(true);
	EquippedWeapon->SetElectricBulletActivated(false);
	EquippedWeapon->SetWeaponBulletType(EBulletType::Normal);
}

void AAgencyCharacter::AbortElectricShoot()
{
	ResetElectricShootLogic();
	GetWorld()->GetTimerManager().ClearTimer(ActivatingElectricDardDelegate);
}

bool AAgencyCharacter::IsEnemyInFrontOf()
{
	FHitResult HitResult;

	const FVector& TraceStart = EquippedWeapon->GetMuzzleSocketLocation();
	const FVector Direction = (FGlobalPointers::PlayerCharacter->GetActorLocation() - TraceStart).GetSafeNormal();
	const FVector TraceEnd = TraceStart + Direction * CHECK_ENEMY_IN_FRON_TRACE_DISTANCE;
	const float TraceRadius = SPHERE_TRACE_RADIUS;

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(FGlobalPointers::PlayerCharacter);
	
	EDrawDebugTrace::Type DebugDrawType = EDrawDebugTrace::None;

	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_Camera),
		false,
		IgnoredActors,
		DebugDrawType,
		HitResult,
		true // ignore self
	);

	if (bHit && HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) && HitActor->IsA(AEnemyCharacter::StaticClass()))
		{
			AbortElectricShoot(); // abort if ally is in front of enemy
			return true;
		}
	}
	return false;
}

bool AAgencyCharacter::IsPlayerTurnHisBack() const
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return false;
	}

	UCustomCameraSystemComponent* CameraSystem = FGlobalPointers::PlayerCharacter->GetCameraSystemComponent();//->->GetForwardVector();
	if (!IsValid(CameraSystem))
	{
		return false;
	}
	TObjectPtr<UCineCameraComponent> CineCamera = CameraSystem->GetFollowCamera();
	if (!IsValid(CineCamera))
	{
		return false;
	}
	
	FVector CameraPlayerFordward = CineCamera->GetForwardVector();
	
	float DotProductPlayerEnemy = FVector::DotProduct(GetActorForwardVector(), CameraPlayerFordward);
	if (DotProductPlayerEnemy >= MIN_DOT_PRODUCT_TO_CONSIDER_OUT_OF_CAMERA)
	{
		return true;
	}
	
	return false;
}
