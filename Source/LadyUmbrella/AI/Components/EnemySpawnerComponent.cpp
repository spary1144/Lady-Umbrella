// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnerComponent.h"


#include "Engine/TriggerVolume.h"
#if WITH_EDITOR
#include "Builders/EditorBrushBuilder.h"
#endif
#include "LadyUmbrella/AI/Managers/AISpawnPoint.h"
#include "LadyUmbrella/Characters/Enemies/EnemyCharacter.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "LadyUmbrella/AI/IAConstants.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/AI/Triggers/TriggerArenaDoor.h"
#include "LadyUmbrella/AI/Triggers/TriggerArenaWindow.h"

// Sets default values for this component's properties
UEnemySpawnerComponent::UEnemySpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	CurrentSpawnPointIndex = 0;
	CurrentEventEnterArena = 0;
	SpawnIndexInGroup = 0;
	RemainingEnterArenaEvent = 0;
	TotalEnterArenaEvent = 0;
	NumReinforcementsSpawned = 0;
	
	TimeBetweenSpawnsOnActivate = 1.f;
	TimeBetweenSpawnsOnEvent = 0.3f;
	TimeBetweenSpawnsOnReinforcement = 0.3f;
	SpawnZOffset = 50.f;

	CurrentWave = 0;
	TotalWaves = 0;

	bLastEnemyRushPlayer = false;
	EnemiesFirstCombatIndexCounter = 0;
	NumReinforcementsSpawned = 0;
}

void UEnemySpawnerComponent::SpawnEnemies()
{
	//get spawns associated with current wave
	TArray<AAISpawnPoint*>* CurrentWaveSpawns = SpawnPointsOnWaves.Find(CurrentWave);
	
	//checks validations
	if (SpawnPointsOnWaves.IsEmpty() || CurrentWaveSpawns == nullptr)
	{
		return;
	}
	//if (SpawnPointsOnActivate.Num() != 0)
	//{
		//Sort by spawn order
		SortSpawns(*CurrentWaveSpawns);
		//Timer Delegate
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UEnemySpawnerComponent::SpawnHandlerOnActive);
	
		UWorld* World = GetWorld();
		if (!IsValid(World))
		{
			return;
		}
		World->GetTimerManager().SetTimer(TimerHandle_SpawnDelay, Delegate, TimeBetweenSpawnsOnActivate, true, 0);
	//}
}

void UEnemySpawnerComponent::SetReferences(APlayerCharacter* NewPlayer,AArenaManager* NewArena)
{
	PlayerCharacter = NewPlayer;
	ArenaManager = NewArena;
}

void UEnemySpawnerComponent::AddSpawnPoint(AAISpawnPoint* NewSpawnPoint)
{
	if (!IsValid(NewSpawnPoint))
	{
		return;
	}

	//check if this spawn has an enter arena event through Door
	if (NewSpawnPoint->bEnterArenaThroughDoor && IsValid(NewSpawnPoint->DoorReference) && IsValid(ArenaManager))
	{
		NewSpawnPoint->DoorReference->SetOwnerArena(ArenaManager);
	}
	//check if this spawn has an enter arena event through Window
	if (NewSpawnPoint->bEnterArenaThroughWindow && IsValid(NewSpawnPoint->WindowReference) && IsValid(ArenaManager))
	{
		NewSpawnPoint->WindowReference->SetOwnerArena(ArenaManager);
	}
	
	//check if is reinforcement
	if (NewSpawnPoint->bIsReinforcement)
	{
		SpawnPointsReinforcements.Add(NewSpawnPoint);
		return;
	}
	//check if is part of an event
	if (NewSpawnPoint->EnterArenaGroup != -1)
	{
		// assign group
		if (!SpawnPointsOnEnterEvent.Contains(NewSpawnPoint->EnterArenaGroup))
		{
			TotalEnterArenaEvent++;
			RemainingEnterArenaEvent++;
		}
		TArray<AAISpawnPoint*>& GroupArray = SpawnPointsOnEnterEvent.FindOrAdd(NewSpawnPoint->EnterArenaGroup);
		GroupArray.Add(NewSpawnPoint);
		return;
	}

	//Enemy is spawned when arena is activated
	for (int32 Wave : NewSpawnPoint->WavesOnBeActivated)
	{
		if (!SpawnPointsOnWaves.Find(Wave))
		{
			TotalWaves++;
			SpawnPointsOnWaves.Add(Wave);
		}
		SpawnPointsOnWaves[Wave].Add(NewSpawnPoint);
	}
}

void UEnemySpawnerComponent::SortSpawns(TArray<AAISpawnPoint*>& SpawnGroup) const
{
	if (bSpawnFollowsAnOrder)
	{
		SpawnGroup.Sort([](const AAISpawnPoint& A, const AAISpawnPoint& B)
		{
			return A.SpawnOrder < B.SpawnOrder;
		});
	}
}

bool UEnemySpawnerComponent::CheckImLastEnemy() const
{
	if (!IsValid(ArenaManager))
	{
		return false;
	}
	
	bool bArenaEventsEnd = EnterArenaEventsFinished();
	int32 NumEnemiesCurrentWave = ArenaManager->GetNumEnemiesInCurrentWave();
	bool bLastWave = IsLastWave();
	bool bLastReinforcement = NumReinforcementsSpawned == SpawnPointsReinforcements.Num() - 1;
	bool bAllWavesFinished = bLastWave && NumEnemiesCurrentWave == 0;
	
	AAISpawnPoint* SpawnPoint = SpawnPointsOnWaves[CurrentWave][CurrentSpawnPointIndex];
	bool bEnterThroughWindow = IsValid(SpawnPoint) && SpawnPoint->bEnterArenaThroughWindow;


	//enemies in windows dont rush if there are the last one
	bool bLastEnemyInWave = bArenaEventsEnd && bLastWave && NumEnemiesCurrentWave == 1 && !bEnterThroughWindow;
	
	bool bLastEnemyInReinforcement = bArenaEventsEnd && bAllWavesFinished && bLastReinforcement && !bEnterThroughWindow;
	
	return ArenaManager->ReinforcementsActivated() ? bLastEnemyInReinforcement : bLastEnemyInWave;
}

void UEnemySpawnerComponent::SpawnEnterArenaGroup(uint8 NumGroup)
{
	if (NumGroup != CurrentEventEnterArena)	return;

	RemainingEnterArenaEvent--;
	
	if (SpawnPointsOnEnterEvent.Find(CurrentEventEnterArena))
	{
		TArray<AAISpawnPoint*>& SpawnGroup = SpawnPointsOnEnterEvent[CurrentEventEnterArena];
		//Sort by spawn order
		SortSpawns(SpawnGroup);
		if (!IsValid(GetWorld())) return;
		
		
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelayEvent, [this, SpawnGroup]()
		{
			// Verify if current event group already exist
			if (!SpawnPointsOnEnterEvent.Contains(CurrentEventEnterArena))
			{
				//go next
				++CurrentEventEnterArena;
				SpawnIndexInGroup = 0;

				//no events available
				if (!SpawnPointsOnEnterEvent.Contains(CurrentEventEnterArena))
				{
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelayEvent);
					return;
				}
			}

			// Verify if there are spawns to be triggered
			if (SpawnIndexInGroup < SpawnGroup.Num())
			{
				AAISpawnPoint* SpawnPoint = SpawnGroup[SpawnIndexInGroup];
				
				if (SpawnPoint && SpawnPoint->bIsActive)
				{
					SpawnEnemy(*SpawnPoint);
				}
				++SpawnIndexInGroup;
			}
			else
			{
				//go next
				++CurrentEventEnterArena;
				SpawnIndexInGroup = 0;
				//clear timer
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelayEvent);
			}
		}, TimeBetweenSpawnsOnEvent, true,0);
	}
	else
	{
		//FLogger::DebugLog("No se ha encontrado el grupo asociado al evento");
	}
}

void UEnemySpawnerComponent::SetCurrentEventEnterArena(int32 NewEventEnterArena)
{
	CurrentEventEnterArena = NewEventEnterArena;
	RemainingEnterArenaEvent = TotalEnterArenaEvent;
}

void UEnemySpawnerComponent::SpawnReinforcements()
{
	SortSpawns(SpawnPointsReinforcements);
	
	int NumEnemiesToBeSpawned = SpawnPointsReinforcements.Num();
	NumReinforcementsSpawned = 0;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelayReinforcements, [this,NumEnemiesToBeSpawned]() mutable
	{
		
		if (NumReinforcementsSpawned < NumEnemiesToBeSpawned)
		{
			SpawnEnemy(*SpawnPointsReinforcements[NumReinforcementsSpawned]);
			NumReinforcementsSpawned++;
		}
		else
		{
			ArenaManager->SetReinforcementsCalled();
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelayReinforcements);
		}
	},TimeBetweenSpawnsOnReinforcement,true);
}

void UEnemySpawnerComponent::TryToInterruptReinforcementsSpawn()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(TimerHandle_SpawnDelayReinforcements))
	{
		TimerManager.ClearTimer(TimerHandle_SpawnDelayReinforcements);
	}

	if (TimerManager.IsTimerActive(TimerHandle_SpawnDelayEvent))
	{
		TimerManager.ClearTimer(TimerHandle_SpawnDelayEvent);
	}

	if (TimerManager.IsTimerActive(TimerHandle_SpawnDelay))
	{
		TimerManager.ClearTimer(TimerHandle_SpawnDelay);
	}
}

int32 UEnemySpawnerComponent::GetEnemiesInCurrentWave() const
{
	if (!SpawnPointsOnWaves.Find(CurrentWave))
	{
		return 0;
	}
	return SpawnPointsOnWaves[CurrentWave].Num();
}

bool UEnemySpawnerComponent::CurrentWaveIsFinished(int32 RemainingEnemies)
{
	if (RemainingEnemies == 0 && !IsLastWave())
	{
		CurrentWave++;
		SpawnEnemies(); //spawn new wave , timer?
		return true;
	}
	return false;
}

#if UE_EDITOR
void UEnemySpawnerComponent::SpawnTriggerVolume()
{
	FActorSpawnParameters Params;
	Params.Name = "TriggerVolume";
	Params.Owner = GetOwner();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TriggerVolume = GetWorld()->SpawnActor<ATriggerVolume>(ATriggerVolume::StaticClass(),GetOwner()->GetActorLocation(),FRotator::ZeroRotator,Params);
	if (IsValid(TriggerVolume))
	{
		TriggerVolume->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
		
		UBrushBuilder* NewBoxBuilder = NewObject<UBrushBuilder>(TriggerVolume);
		TriggerVolume->BrushBuilder = NewBoxBuilder;
		NewBoxBuilder->Build(GetWorld(), TriggerVolume);
		
		
		// if (!ExistingBuilder || !ExistingBuilder->IsA<FMultiBoxBuilder>())
		// {
		// 	UBoxBuilder* NewBoxBuilder = NewObject<UBoxBuilder>(TriggerVolume);
		// 	TriggerVolume->BrushBuilder = NewBoxBuilder;
		// }
		//
		// ABrush* MyBrush = TriggerVolume; // Tu TriggerVolume o volumen generado
		// FVector BoxExtent(100.f, 100.f, 100.f);
		// SetBrushToBox(MyBrush, BoxExtent);
	}
}
#endif

// Called when the game starts
void UEnemySpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemySpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemySpawnerComponent::SpawnHandlerOnActive()
{
	if (const AAISpawnPoint* CurrentSpawnPoint = SpawnPointsOnWaves[CurrentWave][CurrentSpawnPointIndex])
	{
		switch (CurrentSpawnPoint->Target)
		{
		case ETarget::Enemy:
			{
				SpawnEnemy(*CurrentSpawnPoint); //funcion spawn enemie generica
			}
			break;
		case ETarget::Interactable:
			{	//pasar a funcion generica spawn interactable
				SpawnInteractable(*CurrentSpawnPoint);
			}
			break;
		default:
			break;
		}
	}
	
	if (CurrentSpawnPointIndex == SpawnPointsOnWaves[CurrentWave].Num() - 1/*SpawnPointsOnActivate.Num() - 1*/)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnDelay);
		CurrentSpawnPointIndex = 0;
		return;
	}
	
	CurrentSpawnPointIndex++;
}

void UEnemySpawnerComponent::CheckIfThereIsSingleEnemyLeftAlive(AAIControllerGeneric* Controller) const
{
	if (bLastEnemyRushPlayer && CheckImLastEnemy())
	{
		Controller->SetRushPlayerBlackboard();
	}
}

void UEnemySpawnerComponent::SpawnEnemy(const AAISpawnPoint& SpawnPoint)
{
	//get transform
	FVector Location = SpawnPoint.GetStaticMesh()->GetComponentLocation();
	Location.Z += SpawnZOffset;
	FRotator Rotation = SpawnPoint.GetStaticMesh()->GetComponentRotation();
	//get Enemy Class
	UClass* EnemyClass = SpawnPoint.EnemyType;
	if (IsValid(EnemyClass))
	{
		//Spawn
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetWorld()->SpawnActor(
			EnemyClass,
			&Location,
			&Rotation
		));
		if (IsValid(Enemy))
		{
			//setup AIController
			Enemy->SpawnDefaultController();
			if (AAIControllerGeneric* Controller = Cast<AAIControllerGeneric>(Enemy->GetController()))
			{
				Controller->CleanBlackboard();
				if (SpawnPoint.EnterArenaGroup != IAConstants::INVALID_NUMBER_DF_ENEMIES || SpawnPoint.bIsReinforcement || CurrentWave != 0)
				{
					Controller->DetectPlayer(PlayerCharacter,false);
					Enemy->OnDetectPlayer();
				}

				CheckIfThereIsSingleEnemyLeftAlive(Controller);
				
				Controller->InitializeController(ArenaManager, SpawnPoint);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Controller is not of expected base type"));
			}
			//setup EnemyCharacter
			Enemy->SetSplinePath(SpawnPoint.Path);
			Enemy->SetWeaponBP(SpawnPoint.WeaponBP);
			Enemy->Initialize();
			//if enemies enter arena throught Door or Window set the key to unlock
			if (SpawnPoint.bEnterArenaThroughDoor && IsValid(SpawnPoint.DoorReference))
			{
				Enemy->SetUnlockID(SpawnPoint.DoorReference->GetUniqueID());
			}
			if (SpawnPoint.bEnterArenaThroughWindow && IsValid(SpawnPoint.WindowReference))
			{
				Enemy->SetUnlockID(SpawnPoint.WindowReference->GetUniqueID());
			}
			if (SpawnPoint.bIsFirstCombatAgent)
			{
				Enemy->TriggerVoiceLineFirstCombat(EnemiesFirstCombatIndexCounter);
				++EnemiesFirstCombatIndexCounter;
			}
			//save in pool
			ArenaManager->AddEnemyToPool(FEnemyPoolEntry(Enemy,SpawnPoint.EnterArenaGroup));
		}	
	}
}

void UEnemySpawnerComponent::SpawnInteractable(const AAISpawnPoint& SpawnPoint)
{
	FVector Location = SpawnPoint.GetStaticMesh()->GetComponentLocation();
	FRotator Rotation = SpawnPoint.GetStaticMesh()->GetComponentRotation();
	
	UClass* InteractableClass = SpawnPoint.ReactiveElementType;
	AReactiveElement* Interactable = Cast<AReactiveElement>(GetWorld()->SpawnActor(
		InteractableClass,
		&Location,
		&Rotation
		));
	InteractablesPool.Emplace(Interactable);
}