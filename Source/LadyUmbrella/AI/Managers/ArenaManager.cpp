// 
// ArenaManager.cpp
// 
// Manager can be activated(Spawn enemies and start patroling), startcombat(enemies start searching covers and attacking)
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#include "ArenaManager.h"
#include "LadyUmbrella/AI/Triggers/TriggerBoxArena.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Map.h"
#include "LadyUmbrella/AI/Components/EnemyCoordinationComponent.h"
#include "LadyUmbrella/AI/Components/EnemySpawnerComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/AI/Controllers/AIControllerMafia.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/AI/Components/EnemyZonesComponent.h"
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineManager.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Environment/Items/AmmoBox.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/PickableObjects/Ammo.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/CombatMusicType.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "Misc/DataValidation.h"

FOnArenaStarted AArenaManager::OnArenaStarted;
FOnArenaEnded AArenaManager::OnArenaEnded;

// Sets default values
AArenaManager::AArenaManager()
{
	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureArena(TEXT("/Script/Engine.Texture2D'/Game/Textures/Sprites/Icons/ArenaManager/T_ArenaIcon.T_ArenaIcon'"));
	if (TextureArena.Object != nullptr)
	{
		Billboard->SetSprite(TextureArena.Object);
	}
	Billboard->SetupAttachment(RootComponent);

	EnemyZonesComponent		   = CreateDefaultSubobject<UEnemyZonesComponent>(TEXT("EnemyZonesComponent"));
	EnemySpawnerComponent	   = CreateDefaultSubobject<UEnemySpawnerComponent>(TEXT("EnemySpawnerComponent"));
	EnemyCoordinationComponent = CreateDefaultSubobject<UEnemyCoordinationComponent>(TEXT("EnemyCoordinationComponent"));  
	CombatMusicFmodComponent   = CreateDefaultSubobject<UFModComponentInterface>("Combat Fmod Component Interface");

	UFMODEvent* CombatEvent		  = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Music/DEFAULT_COMBAT_MUSIC.DEFAULT_COMBAT_MUSIC'");
	UFMODEvent* MafiaCombatEvent  = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Music/MAFIA_COMBAT_MUSIC.MAFIA_COMBAT_MUSIC'");
	UFMODEvent* AgencyCombatEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/Music/AGENCY_COMBAT_MUSIC.AGENCY_COMBAT_MUSIC'");
	
	FmodUtils::TryAddEvent(CombatEvent		, CombatMusicFmodComponent);
	FmodUtils::TryAddEvent(MafiaCombatEvent	, CombatMusicFmodComponent);
	FmodUtils::TryAddEvent(AgencyCombatEvent, CombatMusicFmodComponent);
	
#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ArrowStaticMeshRef (TEXT("/Game/Meshes/StaticMeshes/EditorOnly/ArenaManager/SM_SpawnPoint.SM_SpawnPoint"));
	if (ArrowStaticMeshRef.Object != nullptr)
	{
		StaticMeshForSpawnPoint = ArrowStaticMeshRef.Object;
	}
#endif

	//Initialize variables
	HearRadius = 3000.f;
	bEnemiesSpawned = false;
	bCombatStarted = false;
	bArenaActive = true;
	bEnemiesRegistered = false;
	NumTriggersToSpawn = 2;
	NumEnemySpawnPoints = 1;
	bIsAgencyArena = false;
	bArenaCleared = false;
	bReActivateArenaOnPlayerDeath = true;
	bDetectEnemiesAccordingToDistance = false;
}

void AArenaManager::Activate()
{
	if (bEnemiesSpawned || !bArenaActive || !IsValid(EnemySpawnerComponent) || bArenaCleared) return;

	bEnemiesSpawned = true;

	if (IsValid(GameInstance))
	{
		if (!GameInstance->OnPlayerRespawned.IsAlreadyBound(this,&AArenaManager::OnPlayerRespawn))
		{
			GameInstance->OnPlayerRespawned.AddDynamic(this, &AArenaManager::OnPlayerRespawn);
		}
		VoiceLineManager = GameInstance->GetSubsystem<UVoiceLineManager>();
		if (IsValid(VoiceLineManager))
		{
			VoiceLineManager->SetCombatState(true);
		}
		GameInstance->SaveLastArena(this);
	}

	EnemyCoordinationComponent->Initialize();
	
	//suscribe delegate dead and shoot
	if (!IsValid(PlayerCharacter))
	{
		return;
	}
	
	if (!PlayerCharacter->OnDeathChanged.IsAlreadyBound(this, &AArenaManager::OnPlayerDeath))
	{
		PlayerCharacter->OnDeathChanged.AddDynamic(this, &AArenaManager::OnPlayerDeath);
	}
	if (!PlayerCharacter->OnPlayerShot.IsBound())
	{
		PlayerCharacter->OnPlayerShot.BindUObject(this, &AArenaManager::OnPlayerShoot);
	}

	//spawns will be registered
	if (!bEnemiesRegistered)
	{
		OnRegisterSpawnsDelegate.Broadcast();
		bEnemiesRegistered = true;
	}

	NumEnemiesInCurrentWave = EnemySpawnerComponent->GetEnemiesInCurrentWave();
	
	EnemySpawnerComponent->SpawnEnemies();
}

void AArenaManager::StartCombat()
{
	if (bCombatStarted || !bArenaActive || !bEnemiesSpawned)
	{
		return;
	}
	
	if (bShouldPlayCombatMusic)
	{
		if (CombatMusicFmodComponent->SetCurrentEvent(ToString(CombatMusicType)))
		{
			CombatMusicFmodComponent->SetParameter(StopCombatParameterName, 0);
			CombatMusicFmodComponent->PlayEvent();
		}
		else
		{
			FLogger::ErrorLog("ArenaManager::StartCombat - Could not set current event.");
		}
	}

	bCombatStarted = true;
	
	EnemyCoordinationComponent->StartCombat();
	OnArenaStarted.Broadcast(true);
	
	//enemies begin detect player
	for (FEnemyPoolEntry EnemyEntry : EnemiesPool)
	{
		if (AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(EnemyEntry.Enemy->GetController()))
		{
			Controller->DetectPlayer(PlayerCharacter,bDetectEnemiesAccordingToDistance);
			EnemyEntry.Enemy->OnDetectPlayer();
		}
	}
}

void AArenaManager::EnterToArena(const uint8 NumGroup) const
{
	if (!bArenaActive || !IsValid(EnemySpawnerComponent))
	{
		return;
	}
	
	EnemySpawnerComponent->SpawnEnterArenaGroup(NumGroup);
}

// Called when the game starts or when spawned
void AArenaManager::BeginPlay()
{
	Super::BeginPlay();

	if (!bArenaActive) return; //check if arena is active

	CombatMusicFmodComponent->InitializeMap();
	
	//get player reference
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->OnPlayerShot.BindUObject(this, &AArenaManager::OnPlayerShoot);
	}
	
	EnemySpawnerComponent->SetReferences(PlayerCharacter,this);
	//EnemySpawnerComponent->SaveSpawnPoints();
	
	//save initial zones Pool
	EnemyZonesComponent->Initialize();

	GameInstance = Cast<ULU_GameInstance>(GetGameInstance());
}
#if WITH_EDITOR
void AArenaManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	CheckMinNumberofAttachedActors();

	DeleteIncorrectAttachedActors();

	// If we need to create new Trigger Boxes to reach our number.
	if (AttachedTriggers.Num() < NumTriggersToSpawn)
	{
		CreateNewTriggerBoxInstances();
	}
	// If we need to delete Trigger Boxes because we have too many.
	else if (AttachedTriggers.Num() > NumTriggersToSpawn)
	{
		RemoveExcessTriggerBoxes();
	}
	
	// If we need to create new Spawn to reach our number.
	if (AttachedEnemySpawns.Num() < NumEnemySpawnPoints)
	{
		CreateNewEnemySpawnPointsInstances();
	}
	// If we need to delete Spawn Boxes because we have too many.
	else if (AttachedEnemySpawns.Num() > NumEnemySpawnPoints)
	{
		RemoveExcessEnemySpawnPoints();
	}
}
#endif
void AArenaManager::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed - Arena Manager %s"), *GetName());	
	Super::Destroyed();

#if UE_EDITOR // This code is only meant to execute in the editor, cause the Arena Manager should not be destroyed otherwise.
	
	// Deleting all Triggers attached to this Manager.
	for (int i = AttachedTriggers.Num() - 1; i >= 0; --i)
	{
		if (IsValid(AttachedTriggers[i]))
		{
			AttachedTriggers[i]->Destroy();
		}
		
		AttachedTriggers.RemoveAt(i);
	}

	// Deleting all EnemySpawnPoints attached to this Manager.
	for (int i = AttachedEnemySpawns.Num() - 1; i >= 0; --i)
	{
		if (IsValid(AttachedEnemySpawns[i]))
		{
			AttachedEnemySpawns[i]->Destroy();
		}
		
		AttachedEnemySpawns.RemoveAt(i);
	}

#endif
}

void AArenaManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CombatMusicFmodComponent->StopEvent();
	
}

void AArenaManager::LoadDataCheckPoint()
{
	if (!bEnemiesSpawned)
	{
		return;
	}
	
	OnPlayerRespawn();

	bEnemiesSpawned = false;
	bCombatStarted = false;
}

void AArenaManager::UnlockBlockingElement()
{
	//unlock blocking element
	CombatMusicFmodComponent->SetParameter(StopCombatParameterName, StopCombatParameter);
	
	FGlobalPointers::GameInstance->GetSubsystem<UVoiceLineManager>()->ResetManager();
	
	//FLogger::DebugLog("Arena limpia");
	if (IsValid(GameInstance))
	{
		GameInstance->OnPlayerRespawned.RemoveAll(this);
	}

	if (IsValid(VoiceLineManager))
	{
		VoiceLineManager->SetCombatState(false);
	}
}

void AArenaManager::ClearZone(const int32 NumZone) const
{
	EnemyZonesComponent->ClearEnemiesInZone(NumZone);
	EnemyZonesComponent->ClearFreeCoversInZone(NumZone);
}

void AArenaManager::ReduceEnemyInZone(const int32 NumZone) const
{
	EnemyZonesComponent->RemoveEnemyFromZone(NumZone);
}

void AArenaManager::ReduceFreeCoverInZone(const int32 NumZone) const
{
	EnemyZonesComponent->RemoveFreeCoverFromZone(NumZone);
}

void AArenaManager::AddCoverZone(const int32 NumZone) const
{
	EnemyZonesComponent->AddFreeCoverToZone(NumZone);
}
void AArenaManager::AddEnemyZone(const int32 NumZone) const
{
	EnemyZonesComponent->AddEnemyToZone(NumZone);
}

bool AArenaManager::IsZoneCreated(const int32 NumZone) const
{
	return EnemyZonesComponent->IsZoneCreated(NumZone);
}

int32 AArenaManager::GetFreeCoversInZone(const int32 NumZone) const
{
	return EnemyZonesComponent->GetFreeCoversInZone(NumZone);
}

int32 AArenaManager::GetEnemiesInZone(const int32 NumZone) const
{
	return EnemyZonesComponent->GetEnemiesInZone(NumZone);
}

void AArenaManager::AddEnemyToPool(FEnemyPoolEntry NewEnemy)
{
	NewEnemy.Enemy->OnDeathChanged.AddDynamic(this, &AArenaManager::OnEnemyDeath);
	OnAllyDeathDelegate.AddUObject(NewEnemy.Enemy, &AEnemyCharacter::OnAllyDeath);
	
	EnemiesPool.Emplace(NewEnemy);
	++NumEnemiesAlive;
	EnemyCoordinationComponent->AddEnemy(NewEnemy);
}

void AArenaManager::RemoveEnemyFromZone(AEnemyCharacter* Enemy) const
{
	if (IsValid(Enemy) && IsValid(Enemy->GetController()))
	{
		if (Enemy->GetController()->IsA(AAIControllerMafia::StaticClass()))
		{
			//Remove from zone pool
			const AAIControllerMafia* Controller = Cast<AAIControllerMafia>(Enemy->GetController());
			ReduceEnemyInZone(Controller->GetZone());
		}	
	}
}

void AArenaManager::RegisterSpawn(AAISpawnPoint* SpawnToRegister) const
{
	EnemySpawnerComponent->AddSpawnPoint(SpawnToRegister);
}

int AArenaManager::GetHighestZone() const
{
	return EnemyZonesComponent->GetHiguestZone();
}

TMap<int,FQueueStruct>* AArenaManager::GetFleeQueue()
{
	return &FleeQueue;
}

TArray<int32>* AArenaManager::GetCloseCombatQueue()
{
	return &CloseCombatQueue;
}

///////////////////////
//Dead Events

bool AArenaManager::ReinforcementsCanBeCalled()
{
	bool bArenaEventsEnd = EnemySpawnerComponent->GetRemainingEnterArenaEvents() == 0;
	bool bLastWave = EnemySpawnerComponent->IsLastWave();
	bool bRemainingEnemiesToCall = NumEnemiesInCurrentWave <= NumEnemiesAliveToCallReinforcements;

	return bReinforcementsActivated && !bReinforcementsHasBeenCalled && bRemainingEnemiesToCall && bArenaEventsEnd && bLastWave;
}

void AArenaManager::OnEnemyDeath(AGenericCharacter* DeadCharacter)
{
	FGlobalPointers::PlayerCharacter->PlayVoiceLine(EVoiceLineState::DEFEAT_ENEMY,true);
	
	//If Player is dead before enemy dies then skip that logic
	UHealthComponent* HealthComp = PlayerCharacter->GetHealthComponent();

	if (!IsValid(PlayerCharacter) || !IsValid(HealthComp) || HealthComp->IsDead())
	{
		return;
	}
	
	if (!IsValid(EnemyCoordinationComponent) || !EnemyCoordinationComponent->IsInitialized())
	{
		return;
	}

	//remove enemy
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(DeadCharacter);
	if (IsValid(Enemy))
	{
		Enemy->StopVoiceLineActive();
		//clean delegate
		FGlobalPointers::GameInstance->GetSubsystem<UVoiceLineManager>()->ClearSpecificEnemyVoiceline(Enemy->GetUniqueID());

		OnAllyDeathDelegate.RemoveAll(Enemy);

		//notify allies the position of this enemy
		OnAllyDeathDelegate.Broadcast(Enemy->GetActorLocation());
		
		//reduce 1 enemy in zone
		RemoveEnemyFromZone(Enemy);
		//remove enemy in coordination
		EnemyCoordinationComponent->RemoveEnemy(Enemy);
		
		//set enemy to dead in pool
		// for (FEnemyPoolEntry& EnemyEntry : EnemiesPool)
		// {
		// 	if (IsValid(EnemyEntry.Enemy) && EnemyEntry.Enemy == Enemy)
		// 	{
		// 		EnemyEntry.IsDead = true;
		// 		break;
		// 	}
		// }
		--NumEnemiesAlive;
	}

	//Reduce enemy in current wave
	if (NumEnemiesInCurrentWave > 0)
	{
		NumEnemiesInCurrentWave--;
	}
	
	//Try Call reinforcements
	if (ReinforcementsCanBeCalled())
	{
		OnReinforcementsCalled.Broadcast();
		bReinforcementsHasBeenCalled = true;
		EnemySpawnerComponent->SpawnReinforcements();
		return;
	}

	//Check if current wave is finished and 
	if (EnemySpawnerComponent->CurrentWaveIsFinished(NumEnemiesInCurrentWave))
	{   // new wave
		NumEnemiesInCurrentWave = EnemySpawnerComponent->GetEnemiesInCurrentWave();
		return;
	}

	//all waves finished
	if (EnemySpawnerComponent->GetRemainingEnterArenaEvents() == 0 && NumEnemiesAlive == 0)
	{
		FGlobalPointers::PlayerCharacter->PlayVoiceLine(EVoiceLineState::DEFEAT_ENEMY,true,false);
		
		EnemyCoordinationComponent->StopAndCleanup();
		//unlock blocking element
		UnlockBlockingElement();
		bArenaCleared = true;
		
		//unbind delegates
		if (IsValid(PlayerCharacter))
		{
			PlayerCharacter->OnDeathChanged.RemoveDynamic(this, &AArenaManager::OnEnemyDeath);
			PlayerCharacter->OnPlayerShot.Unbind();
		}
	
		bEnemiesSpawned = false;
		bCombatStarted = false;
		bReinforcementsHasBeenCalled = false;
		NumEnemiesAlive = 0;

		OnArenaFinished();
		OnArenaEnded.Broadcast(false);
	}

	//If single wave combat check when remains one enemy , it rush player
	if (EnemySpawnerComponent->IsLastEnemyRushPlayerActive() && EnemySpawnerComponent->IsSingleWave() && NumEnemiesInCurrentWave == 1)
	{
		AEnemyCharacter* LastEnemy = EnemiesPool.Last().Enemy;
		if (!IsValid(LastEnemy))
		{
			return;
		}

		AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(LastEnemy->GetController());
		if (!IsValid(Controller))
		{
			return;
		}
		
		Controller->SetRushPlayerBlackboard();
	}
	
}


void AArenaManager::ClearEnemiesPool()
{
	for (FEnemyPoolEntry EnemyEntry : EnemiesPool)
	{
		if (!IsValid(EnemyEntry.Enemy))
		{
			continue;
		}
		EnemyEntry.Enemy->DestroyEquippedWeapon();
		//EnemyEntry.Enemy->StopVoiceLineActive();
		EnemyEntry.Enemy->Destroy();
	}
	
	EnemiesPool.Empty();
}

void AArenaManager::OnPlayerDeath(AGenericCharacter* DeadCharacter)
{
	if (!IsValid(EnemyCoordinationComponent))
	{
		return;
	}

	VoiceLineManager->ResetManager();

	if (EnemyCoordinationComponent->IsInitialized())
	{
		//Stop Coordination Manager
		EnemyCoordinationComponent->StopAndCleanup();
		EnemyCoordinationComponent->TryReduceTokens();
	}
	// Interrupt Enemy Spawn
	EnemySpawnerComponent->TryToInterruptReinforcementsSpawn();
	
	// Reset Events
	EnemySpawnerComponent->SetCurrentEventEnterArena(0);
	// Reset Waves
	EnemySpawnerComponent->ResetWaves();
	NumEnemiesInCurrentWave = 0;
	
	//restore initial zones Pool
	EnemyZonesComponent->Reset();
	//restore flee queue
	for (TPair<int,FQueueStruct>& Elem : FleeQueue)
	{
		Elem.Value.Clear();
	}
	
	//Reset triggers conditions
	bEnemiesSpawned = false;
	bCombatStarted = false;
	bReinforcementsHasBeenCalled = false;
	

	if (IsValid(PlayerCharacter))
	{
		PlayerCharacter->OnDeathChanged.RemoveDynamic(this, &AArenaManager::OnPlayerDeath);
		PlayerCharacter->OnPlayerShot.Unbind();
	}

	if (bShouldPlayCombatMusic && IsValid(CombatMusicFmodComponent))
	{
		CombatMusicFmodComponent->SetParameter(StopCombatParameterName, 1);
	}
	OnArenaEnded.Broadcast(false);
	NumEnemiesAlive = 0;
}

void AArenaManager::OnPlayerShoot()
{
	//distance arena instance to player
	float DistanceArenaPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).Size();
	if (DistanceArenaPlayer < HearRadius)
	{
		StartCombat();
	}
}

void AArenaManager::RemoveAmmoBoxesDroppedByEnemies()
{
	for (AAmmo* Ammo : AmmoBoxesDroppedInArena)
	{
		if (!IsValid(Ammo))
		{
			continue;	
		}
		
		Ammo->Destroy();
	}
}

void AArenaManager::OnPlayerRespawn()
{
	//Clear Enemies Pool
	ClearEnemiesPool();

	//Reset Trigger Doors and Windows
	if (OnResetTriggersDelegate.IsBound())
	{
		OnResetTriggersDelegate.Broadcast();
	}
	
	//ReActivate Arena
	if (!bArenaCleared && bReActivateArenaOnPlayerDeath)
	{
		Activate();
	}

	//delete all ammo boxes dropped by enemies
	RemoveAmmoBoxesDroppedByEnemies();
	
	AmmoBoxesDroppedInArena.Empty();
}

void AArenaManager::OnPlayerSkipArena()
{
	if (bArenaCleared || EnemiesPool.IsEmpty())
	{
		return;
	}
	
	ClearEnemiesPool();
	RemoveAmmoBoxesDroppedByEnemies();
	EnemiesPool.Empty();
	OnArenaEnded.Broadcast(false);
}

//Editor functions
#pragma region Editor:functions

#if UE_EDITOR
void AArenaManager::DeleteIncorrectAttachedActors()
{
	// Delete (from our array and the world) those AttachedTriggers that are no longer correctly setup.
	for (int i = AttachedTriggers.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(AttachedTriggers[i]))
		{
			AttachedTriggers.RemoveAt(i);
			continue;
		}
		
		if (!AttachedTriggers[i]->IsAttachedTo(this))
		{
			AttachedTriggers[i]->Destroy();
			AttachedTriggers.RemoveAt(i);
		}
	}

	// Delete (from our array and the world) those AttachedEnemySpawns that are no longer correctly setup.
	for (int i = AttachedEnemySpawns.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(AttachedEnemySpawns[i]))
		{
			AttachedEnemySpawns.RemoveAt(i);
			continue;
		}
		
		if (!AttachedEnemySpawns[i]->IsAttachedTo(this))
		{
			AttachedEnemySpawns[i]->Destroy();
			AttachedEnemySpawns.RemoveAt(i);
		}
	}
}

void AArenaManager::CheckMinNumberofAttachedActors()
{
	// Forcing a minimum positive number of TriggersToSpawn
	if (NumTriggersToSpawn < MinNumberOfAttachedTriggers)
	{
		NumTriggersToSpawn = MinNumberOfAttachedTriggers;
	}
	
	// Forcing a minimum positive number of EnemySpawns
	if (NumEnemySpawnPoints < MinNumberOfAttachedEnemySpawns)
	{
		NumEnemySpawnPoints = MinNumberOfAttachedEnemySpawns;
	}
}

void AArenaManager::CreateNewTriggerBoxInstances()
{
	for (int i = AttachedTriggers.Num(); i < NumTriggersToSpawn; i++)
	{
		FVector Offset(0, 200 * i, 0);
		FVector Location = GetActorLocation() + Offset;
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ATriggerBoxArena* Trigger = GetWorld()->SpawnActor<ATriggerBoxArena>(TriggerClass, Location, FRotator::ZeroRotator, Params);
		if (IsValid(Trigger))
		{
			Trigger->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			Trigger->SetArenaReference(this);
			AttachedTriggers.Add(Trigger);

			if (i == 0)
			{
				Trigger->SetEFunc(EPurpose::ActivateArena);
			}
			if (i == 1)
			{
				Trigger->SetEFunc(EPurpose::StartCombatArena);
			}
		}
	}
}

void AArenaManager::RemoveExcessTriggerBoxes()
{
	for (int i = AttachedTriggers.Num() - 1; i >= NumTriggersToSpawn; i--)
	{
		if (IsValid(AttachedTriggers[i]))
		{
			AttachedTriggers[i]->Destroy();
		}
		AttachedTriggers.RemoveAt(i);
	}
}

void AArenaManager::CreateNewEnemySpawnPointsInstances()
{
	for (int32 i = AttachedEnemySpawns.Num(); i < NumEnemySpawnPoints; i++)
	{
		FVector Offset(200, 200 * i, 0);
		FVector Location = GetActorLocation() + Offset;
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AAISpawnPoint* NewSpawnPoint = GetWorld()->SpawnActor<AAISpawnPoint>(SpawnPointClass, Location, FRotator::ZeroRotator, Params);
		if (IsValid(NewSpawnPoint))
		{
			NewSpawnPoint->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			NewSpawnPoint->SetArenaReference(this);
			AttachedEnemySpawns.Add(NewSpawnPoint);
		}
	}
}

void AArenaManager::RemoveExcessEnemySpawnPoints()
{
	for (int i = AttachedEnemySpawns.Num() - 1; i >= NumEnemySpawnPoints; i--)
	{
		if (IsValid(AttachedEnemySpawns[i]))
		{
			AttachedEnemySpawns[i]->Destroy();
		}
		AttachedEnemySpawns.RemoveAt(i);
	}
}

void AArenaManager::SpawnTriggerVolume()
{
	EnemySpawnerComponent->SpawnTriggerVolume();
}
#endif
#pragma endregion  Editor:functions