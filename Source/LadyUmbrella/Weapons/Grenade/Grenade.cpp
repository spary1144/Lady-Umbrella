// 
// Grenade.cpp
// 
// Implementation of the Grenade class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Grenade.h"

#include "FMODAudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h" 
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Characters/Enemies/Mafia/MafiaCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/CameraComponent/CustomCameraSystemComponent.h"
#include "LadyUmbrella/Components/CameraComponent/CameraComponentTypes/ShakeSource.h"
#include "LadyUmbrella/Components/CoverComponent/CharacterCoverMovementComponent.h"
#include "LadyUmbrella/Environment/CoverSystem/CoverParent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/Util/Steam/Steam.h"
#include "LadyUmbrella/Util/Steam/SteamAchievement.h"

#define GRENADE_STATIC_MESH_ASSET TEXT("/Script/Engine.StaticMesh'/Game/Blueprints/LevelElements/PlaceholderMeshes/Primitives/SM_Sphere.SM_Sphere'")
#define GRENADE_MATERIAL_ASSET TEXT("/Script/Engine.MaterialInstance'/Game/Materials/Instancias/Opaque/MI_PlaceholderWhite.MI_PlaceholderWhite'")
#define GRENADE_EXPLOSION_ASSET TEXT("/Script/Niagara.NiagaraSystem'/Game/Particles/NiagaraSystems/NS_GrenadeExplosion.NS_GrenadeExplosion'")
#define GRENADE_EXPLOSION_SOUND TEXT("/Script/Engine.SoundWave'/Game/Sounds/SoundWaves/Explosions/SW_Explode02.SW_Explode02'")

FOnGrenadeBounced AGrenade::OnGrenadeBounced;

AGrenade::AGrenade()
{
	SphereComponent  = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Block);

	FModAudioComponentBounce = CreateDefaultSubobject<UFModComponentInterface>(TEXT("FModAudioComponent"));

	UFMODEvent* BouncingEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Mafia/GrenadeBounce.GrenadeBounce'");
	FmodUtils::TryAddEvent(BouncingEvent, FModAudioComponentBounce);
	
	FModAudioComponentExplode = CreateDefaultSubobject<UFModComponentInterface>("Fmod Explosion Component Interface");
	
	UFMODEvent* ExplosionEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Mafia/Grenade.Grenade'");
	FmodUtils::TryAddEvent(ExplosionEvent, FModAudioComponentExplode);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
	StaticMeshComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(GRENADE_STATIC_MESH_ASSET).Object);
	StaticMeshComponent->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterialInstance>(GRENADE_MATERIAL_ASSET).Object);
	StaticMeshComponent->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.25f;
	
	NS_Explosion		= LoadObject<UNiagaraSystem>(nullptr, GRENADE_EXPLOSION_ASSET, nullptr, LOAD_None, nullptr);
	ExplosionRange		= 500.0f;
	MaxDamage           = 100.0f;
	MinDamage           = 0.0f;
	GrenadeRestitution	= 0.75f;
	GrenadeTimeToExplode = 3.f;
	DamagedActors       = TArray<AActor*>();
	bExplodeOnContact   = false;
	
	const ConstructorHelpers::FObjectFinder<UCurveFloat> ExplosionDamageFalloff(TEXT("/Script/Engine.CurveFloat'/Game/DataStructures/Curves/Combat/C_GrenadeDamageFalloff.C_GrenadeDamageFalloff'"));
	if (ExplosionDamageFalloff.Succeeded())
	{
		DamageFalloff = ExplosionDamageFalloff.Object;
	}
	
	DataTableRowHandler.DataTable = AssetUtils::FindObject<UDataTable>("/Script/Engine.DataTable'/Game/DataStructures/DataTables/DataDrivenTables/DT_LadyUmbrellaSystems_GrenadeData.DT_LadyUmbrellaSystems_GrenadeData'");
	if (IsValid(DataTableRowHandler.DataTable))
	{
		DataTableRowHandler.RowName = "BP_Grenade";
		LoadGrenadeFromDataTable();
	}
}

void
AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	ExplosionAudioComponent->Stop();
	SetLifeSpan(5);

	SphereComponent->OnComponentHit.AddDynamic(this, &AGrenade::OnHit);
	LoadGrenadeFromDataTable();
}

void
AGrenade::Tick(float DeltaSeconds)
{
	(void) OnGrenadeThrown.ExecuteIfBound(GetActorLocation());
}

void
AGrenade::Throw(const FVector& Impulse)
{
	ProjectileMovementComponent->Velocity = Impulse;
	
	GetWorldTimerManager().SetTimer(
		UnusedHandle,
		this,
		 &AGrenade::Explode, 
		GrenadeTimeToExplode,
		true,
		GrenadeTimeToExplode
	);

	if (const AMainController* PCMainController = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PCMainController->HandleToggleGrenadeIndicator(this);
	}
}

void
AGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor->IsA(AUmbrella::StaticClass()) && FGlobalPointers::Umbrella->IsOpen())
	{
		OnGrenadeBounced.Broadcast();
		ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity * -FGlobalPointers::Umbrella->GetBounciness();

		FModAudioComponentBounce->PlayEvent();
		
		bExplodeOnContact = true;
		bHasBeenRebounded = true;
		
		if (IsValid(FGlobalPointers::PlayerCharacter) && IsValid(FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()))
		{
			FGlobalPointers::PlayerCharacter->GetCameraSystemComponent()->MakeCameraShake(EShakeSource::IMPACT, GetActorLocation(), 0, 200);
		}
	}
	else if (OtherActor->IsA(AEnemyCharacter::StaticClass()))
	{
		GetProjectileMovementComponent()->StopMovementImmediately();

		if (bExplodeOnContact)
		{
			Explode();
		}
	}
	else
	{
		Cast<AGrenade>(HitComponent->GetOwner())->GetProjectileMovementComponent()->Velocity *= GrenadeRestitution;
	}
}

void
AGrenade::Explode()
{
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	
	World->GetTimerManager().ClearTimer(UnusedHandle);
	
	if (NS_Explosion)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NS_Explosion, GetActorLocation());
	}

	if (IsValid(FModAudioComponentExplode))
	{
		FModAudioComponentExplode->PlayEvent();
	}
	
	OnGrenadeExplodedEvent();
	
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;
	ActorsToIgnore.Add(this);
	
	UKismetSystemLibrary::SphereTraceMulti
	(
		World,
		GetActorLocation(),
		GetActorLocation(),
		ExplosionRange,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResults,
		true,
		FLinearColor::White,
		FLinearColor::White,
		2.f
	);
	
	for (FHitResult Result : HitResults)
	{
		AActor* HitActor = Result.GetActor();
		
		if (!IsValid(HitActor))
			continue;
		
		if (ShouldDamageActor(HitActor))
		{
			IHealthInterface* HealthInterface = Cast<IHealthInterface>(HitActor);
			
			if (HealthInterface)
			{
				if (ShouldDamageGod(HitActor))
				{
					HealthInterface->SubtractHealth(GetGrenadeDamage(HitActor), EWeaponType::Grenade);
					if (HitActor->IsA(APlayerCharacter::StaticClass()))
					{
						OnPlayerDamaged();
					}
					else if (HitActor->IsA(AEnemyCharacter::StaticClass()) && bHasBeenRebounded)
					{
						FGlobalPointers::GameInstance->AddGrenadeRebounds();

						if (FGlobalPointers::GameInstance->GetGrenadeRebounds() >= GRENADE_REBOUND_COUNT)
							Steam::UnlockAchievement(ESteamAchievement::RESEND);
					}
				}
			}
		}

		DamagedActors.Add(HitActor);
	}
	
	if (const TObjectPtr<AMainController> PCMainController = FGlobalPointers::MainController)
	{
		PCMainController->HandleUnToggleGrenadeIndicator();
	}
	
	FTimerHandle TimerHandle;

	World->GetTimerManager().SetTimer(TimerHandle, this, &AGrenade::DestroyGrenade, 8.f, false);
	//Destroy();
	SetActorHiddenInGame(true);
}

void AGrenade::DestroyGrenade()
{
	Destroy();
}

float AGrenade::GetGrenadeDamage(const AActor* HitActor) const
{
	const float DamageCurveValue = DamageFalloff->GetFloatValue(GetDistanceTo(HitActor) / ExplosionRange);
	const float ClampedValue = FMath::Clamp(DamageCurveValue * MaxDamage, MinDamage, MaxDamage);

	if (HitActor->IsA(AEnemyCharacter::StaticClass()))
	{
		return ClampedValue * 1000.0f;
	}
	
	return ClampedValue;
}

bool
AGrenade::ShouldDamageActor(const AActor* HitActor) const
{
	if (DamagedActors.Contains(HitActor))
	{
		return false;
	}
	
	if (HitActor->IsA(APlayerCharacter::StaticClass()))
	{
		return ShouldDamagePlayer(Cast<APlayerCharacter>(HitActor));
	} 
	
	if (HitActor->IsA(AGenericCharacter::StaticClass()))
	{
		return !Cast<AGenericCharacter>(HitActor)->GetCoverMovementComponent()->IsMovementControlledByCover();
	}
	
	return true;
}

bool AGrenade::ShouldDamagePlayer(const APlayerCharacter* PlayerCharacter) const
{
	FHitResult Obstacle;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(
		Obstacle,
		GetActorLocation(),
		PlayerCharacter->GetActorLocation(),
		ECC_GameTraceChannel1,
		CollisionQueryParams
	);
	
	const AActor* ObstacleActor = Obstacle.GetActor();

	if (IsValid(ObstacleActor))
	{
		if (PlayerCharacter->GetCoverMovementComponent()->IsMovementControlledByCover())
		{
			return !ObstacleActor->IsA(ACoverParent::StaticClass());
		}
		
		if (ObstacleActor->IsA(AUmbrella::StaticClass()))
		{
			return !Cast<AUmbrella>(ObstacleActor)->IsOpen();
		}
	}

	return true;
}

bool AGrenade::ShouldDamageGod(const AActor* HitActor)
{
	if (!HitActor->IsA(APlayerCharacter::StaticClass()))
	{
		return true;
	}
	
	return  IsValid(Cast<APlayerCharacter>(HitActor)) && !Cast<APlayerCharacter>(HitActor)->IsGodmode();	
}

void AGrenade::LoadGrenadeFromDataTable()
{
	const FGrenadeDataDriven* Data = FDataTableHelpers::ValidateTableAndGetData<FGrenadeDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	if (!Data)
	{
		FLogger::ErrorLog("Grenade Data not loaded from Table [DataTableRowHandler.DataTable == nullptr]");
		return;
	}
	
	MaxDamage		= Data->MaxDmg;
	MinDamage		= Data->MinDmg;
	ExplosionRange  = Data->SphereDmgRadius;
}

