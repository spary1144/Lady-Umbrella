// 
// MafiaCharacter.cpp
// 
// Implementation of the Mafia character class
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MafiaCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineActor.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineDTO.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineState.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FEnemyDataDriven.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Weapons/Grenade/Grenade.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"

#define ANIM_CLASS_ASSET TEXT("/Script/Engine.AnimBlueprint'/Game/Animations/AnimationBlueprints/Enemies/ABP_Mafia.ABP_Mafia_C'")
#define SKELETAL_MESH_ASSET TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_BasicMesh.SK_BasicMesh'") 

FOnGrenadeTokenReleased AMafiaCharacter::OnGrenadeTokenReleased;

void AMafiaCharacter::ResetCollision()
{
	if (IsValid(Grenade))
	{
		Grenade->SetActorEnableCollision(true);
		GetWorld()->GetTimerManager().ClearTimer(ResetCollisionTimerHandler);
	}
}

void AMafiaCharacter::LoadEnemyFromDataTable()
{
	const FEnemyDataDriven* Data = FDataTableHelpers::ValidateTableAndGetData<FEnemyDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	if (!Data)
	{
		FLogger::ErrorLog("MafiaCharacter Data not loaded from Table [DataTableRowHandler.DataTable == nullptr]");
		return;
	}

	MinMissChance = Data->MinMissingChance;
	MaxMissChance = Data->MaxMissingChance;
	MinShootRange = Data->MinShootDistance;
	MaxShootRange = Data->MaxShootDistance;
	
	SpeedOnFleeing = Data->FleeSpeed;
	SpeedOnCloseCombat = Data->CloseCombatSpeed;
	SpeedOnPatroling = Data->PatrolSpeed;
	SpeedOnEnterArena = Data->EnterArenaSpeed;
	SpeedOnMoveToCover = Data->MoveToCoverSpeed;

	if (IsValid(GetHealthComponent()))
	{
		GetHealthComponent()->SetMaxHealth(Data->MaxHealth);
		GetHealthComponent()->SetHealth(Data->MaxHealth);
	}
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = Data->MaxSpeed;
	}
}

void AMafiaCharacter::SetDead()
{
	Super::SetDead();

	if (IsValid(DeathSystem))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		DeathSystem,
		this->GetActorLocation(),
		FRotator::ZeroRotator,
		FVector::OneVector
		);
	}

	
}

AMafiaCharacter::AMafiaCharacter()
	: bHasGrenadeToken(false)
	, MinThrowGrenadeDistance(1000)
	, MaxThrowGrenadeDistance(5000)
	, GrenadeEndLocationZOffset(-70)
	, GrenadeEndLocationForwardOffset(200)
	, GrenadeEndLocationLateralOffset(200)
	, GrenadeMinLateralOffset(100)
	, GrenadeArc(0.75)
{
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -80.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SKELETAL_MESH_ASSET).Object);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	
	TSoftObjectPtr<UAnimBlueprintGeneratedClass> AnimBlueprint = TSoftObjectPtr<UAnimBlueprintGeneratedClass>(
		FSoftObjectPath(TEXT("/Game/StarterContent/Characters/Mannequins/Animations/ABP_Manny.ABP_Manny_C"))
	);

	if (AnimBlueprint.IsValid()) 
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint.Get());
	} 
	else 
	{
		AnimBlueprint.LoadSynchronous();

		if (AnimBlueprint.IsValid()) 
		{
			GetMesh()->SetAnimInstanceClass(AnimBlueprint.Get());
		}
	}
	
	SetWeaponType(EWeaponType::Rifle);
	SpeedOnPatroling	= 300.f;
	SpeedOnFleeing		= 450.f;
	SpeedOnCloseCombat	= 200.f;
	SpeedOnEnterArena	= 400.f;
	SpeedOnMoveToCover	= 200.f;
	DataTableRowHandler.RowName = "BP_MafiaCharacter";

	// Add Voice Lines
	// TODO: Move ALL asset loading to the ShaderMap.


	UFMODEvent* Throwing = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_THROWING.MAFIA_THROWING'");
	FmodUtils::TryAddEvent(Throwing, GetFmodComponentVoiceLine());

	UFMODEvent* Shooting = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_SHOOTING.MAFIA_SHOOTING'");
	FmodUtils::TryAddEvent(Shooting, GetFmodComponentVoiceLine());

	UFMODEvent* ShieldDepleated = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_SHIELD_DEPLETED.MAFIA_SHIELD_DEPLETED'");
	FmodUtils::TryAddEvent(ShieldDepleated, GetFmodComponentVoiceLine());
	
	UFMODEvent* Reloading = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_RELOADING.MAFIA_RELOADING'");
	FmodUtils::TryAddEvent(Reloading, GetFmodComponentVoiceLine());
	
	UFMODEvent* NonCombat = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_NON_COMBAT.MAFIA_NON_COMBAT'");
	FmodUtils::TryAddEvent(NonCombat, GetFmodComponentVoiceLine());

	UFMODEvent* EnemyInCover = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_ENEMY_IN_COVER.MAFIA_ENEMY_IN_COVER'");
	FmodUtils::TryAddEvent(EnemyInCover, GetFmodComponentVoiceLine());

	UFMODEvent* Cinematic = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_Cinematic__Chapter_5_.MAFIA_Cinematic__Chapter_5_'");
	FmodUtils::TryAddEvent(Cinematic, GetFmodComponentVoiceLine());

	UFMODEvent* Bouncing = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyVoices/Mafia/MAFIA_BOUNCING.MAFIA_BOUNCING'");
	FmodUtils::TryAddEvent(Bouncing, GetFmodComponentVoiceLine());

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DeathSystemFinder(TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_HatMafia.NS_HatMafia'"));
	if (DeathSystemFinder.Succeeded())
	{
		DeathSystem = DeathSystemFinder.Object;
	}
}

void AMafiaCharacter::PostInitProperties()
{
	Super::PostInitProperties();
	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::MafiaVoiceLines); //MafiaVoiceLines
	GetVoiceLineComponent()->SetOwnerName(OWNER_NAME); //Mafia
}

void AMafiaCharacter::OnDetectPlayer()
{
	Super::OnDetectPlayer();

	if (!IsValid(EquippedWeapon))
	{
		return;
	}
	
	EquippedWeapon->SetActorRelativeLocation(FVector(-2.413895,0.000001,-6.834901));
	EquippedWeapon->SetActorRelativeRotation(FRotator(0.000000,0.000000,0.000000));
}

void AMafiaCharacter::OnThrowGrenadeEnd(UAnimMontage* Montage, bool bInterrupted)
{
	SetHasGrenadeToken(false);
	OnGrenadeTokenReleased.Broadcast();
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(Montage) && Montage == ThrowGrenadeMontage)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &AMafiaCharacter::OnThrowGrenadeEnd);
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &AMafiaCharacter::OnMontageNotifyBeginMafia);
	}
}

void
AMafiaCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	LoadEnemyFromDataTable();

	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->SetActorRelativeLocation(FVector(15.13,7.95,0.53));
		EquippedWeapon->SetActorRelativeRotation(FRotator(8.06,24.25,-15.3));
	}

	GetFmodComponentVoiceLine()->InitializeMap();
	
	GetFmodComponentVoiceLine()->SetVoiceLineIdentifier(FMOD_EVENT_IDENTIFIER); //MAFIA_
	
	AGrenade::OnGrenadeBounced.AddUObject(this,&AMafiaCharacter::OnGrenadeBouncedByPlayer);
}

void
AMafiaCharacter::ThrowGrenade()
{
	UWorld* World = GetWorld();
	
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(
		GetTransform().GetLocation(),
		UGameplayStatics::GetPlayerPawn(World, 0)->GetTransform().GetLocation()
	));
	
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(ThrowGrenadeMontage))
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation("GrenadeSocket");
		FTransform GrenadeSpawnTransform;
		GrenadeSpawnTransform.SetLocation(SpawnLocation);
		GrenadeSpawnTransform.SetRotation(FQuat(FRotator::ZeroRotator));
		GrenadeSpawnTransform.SetScale3D(FVector(1, 1, 1));

		if (GrenadeBP != nullptr)
		{
			Grenade = World->SpawnActor<AGrenade>(GrenadeBP, GrenadeSpawnTransform);
			if (IsValid(Grenade))
			{
				Grenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "GrenadeSocket");
			}
		}
		
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AMafiaCharacter::OnMontageNotifyBeginMafia);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMafiaCharacter::OnThrowGrenadeEnd);
		AnimInstance->Montage_Play(ThrowGrenadeMontage, 1);

		//Voice Line Throwing
		if (IsValid(GetVoiceLineComponent()))
		{
			GetVoiceLineComponent()->QueueVoiceLine(
				FVoiceLineDTO
				{
					EVoiceLineState::THROWING,EVoiceLineActor::MAFIA, this,true
				}
			);	//THROWING
		}
	}
}

void AMafiaCharacter::OnMontageNotifyBeginMafia(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{	
	if (NotifyName.IsEqual("ThrowGrenade"))
	{

		UWorld* World = GetWorld();
		
		OnGrenadeThrowEvent();
		Grenade->Destroy();

		FVector GrenadeVelocity;
		
		const APlayerCharacter* Player = FGlobalPointers::PlayerCharacter;

		GrenadeEndPosition = Player->GetActorLocation();
		GrenadeEndPosition.Z += GrenadeEndLocationZOffset;
	
		GrenadeEndPosition += (GetActorLocation() - UGameplayStatics::GetPlayerPawn(World, 0)->GetActorLocation()).GetSafeNormal() * GrenadeEndLocationForwardOffset;

		if (Player->GetCoverMovementComponent()->IsInTallCover())
		{
			GrenadeEndLocationLateralOffset = FMath::RandRange(GrenadeMinLateralOffset, GrenadeEndLocationLateralOffset);

			if (FMath::RandBool())
			{
				GrenadeEndLocationLateralOffset *= -1;
			}
		}
		else
		{
			GrenadeEndLocationLateralOffset = 1.0f;
		}

		GrenadeEndPosition += (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal().Cross(GetActorUpVector()) * GrenadeEndLocationLateralOffset;
	
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(
			World,
			GrenadeVelocity,
			Grenade->GetActorLocation(),
			GrenadeEndPosition,
			0,
			GrenadeArc
		);

		FTransform GrenadeSpawnTransform;
		GrenadeSpawnTransform.SetLocation(Grenade->GetActorLocation());
		GrenadeSpawnTransform.SetRotation(FQuat(FRotator::ZeroRotator));
		GrenadeSpawnTransform.SetScale3D(FVector(1, 1, 1));

		if (GrenadeBP != nullptr)
		{
			Grenade = World->SpawnActor<AGrenade>(GrenadeBP, GrenadeSpawnTransform);
			Grenade->SetActorEnableCollision(false);
			Grenade->Throw(GrenadeVelocity);
			World->GetTimerManager().SetTimer(ResetCollisionTimerHandler, this, &AMafiaCharacter::ResetCollision, .05f, false);
		}
	}
}


void AMafiaCharacter::OnGrenadeBouncedByPlayer()
{
	if (!IsValid(GetVoiceLineComponent()))
	{
		return;
	}
	GetVoiceLineComponent()->QueueVoiceLine(
		FVoiceLineDTO
		{
			EVoiceLineState::BOUNCING, EVoiceLineActor::ENEMY,  this,true
		}
	);
}
