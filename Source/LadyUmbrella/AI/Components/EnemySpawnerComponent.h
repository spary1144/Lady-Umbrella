// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LadyUmbrella/AI/Controllers/AIControllerGeneric.h"
#include "EnemySpawnerComponent.generated.h"

//forward declarations
class APlayerCharacter;
class AArenaManager;
class ACoordinationManager;
class AAISpawnPoint;
class ATriggerVolume;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UEnemySpawnerComponent : public UActorComponent
{
	GENERATED_BODY()
	
	//Spawn points references
	// UPROPERTY(EditInstanceOnly,Category="Configuration")
	// TArray<AAISpawnPoint*> SpawnPointsOnActivate;

	TMap<int32, TArray<AAISpawnPoint*>> SpawnPointsOnWaves;

	UPROPERTY(EditInstanceOnly,Category="Configuration")
	TArray<AAISpawnPoint*> SpawnPointsReinforcements;
	
	//interactable pool
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	TArray<AActor*> InteractablesPool;
	
	//spawns trigger in Events
	TMap<int32, TArray<AAISpawnPoint*>> SpawnPointsOnEnterEvent;

	UPROPERTY(EditAnywhere,Category="Configuration")
	TSubclassOf<AAISpawnPoint> SpawnPointClass;

	UPROPERTY(EditAnywhere,Category="Configuration")
	bool bSpawnFollowsAnOrder = false;

	UPROPERTY(EditInstanceOnly, Category = "Configuration")
	ATriggerVolume* TriggerVolume;
	
	//Counters
	int CurrentSpawnPointIndex; //index of SpawnPointsOnActivate
	int CurrentEventEnterArena; //key of SpawnPointsOnEnterEvent
	int32 SpawnIndexInGroup; //index of array inside SpawnPointsOnEnterEvent map
	int8_t RemainingEnterArenaEvent;
	int8_t TotalEnterArenaEvent;

	int32 NumReinforcementsSpawned;
	uint8_t EnemiesFirstCombatIndexCounter;

	//Waves Control
	UPROPERTY(VisibleInstanceOnly, Category = "Data")
	int32 TotalWaves;

	UPROPERTY(VisibleInstanceOnly, Category = "Data")
	int32 CurrentWave;
	
	//Timers
	UPROPERTY()
	FTimerHandle TimerHandle_SpawnDelay; //Spawn delay
	UPROPERTY()
	FTimerHandle TimerHandle_SpawnDelayEvent; //Spawn delay

	UPROPERTY()
	FTimerHandle TimerHandle_SpawnDelayReinforcements; 

	UPROPERTY(EditAnywhere,Category="Configuration")
	float TimeBetweenSpawnsOnActivate;

	UPROPERTY(EditAnywhere,Category="Configuration")
	float TimeBetweenSpawnsOnEvent;

	UPROPERTY(EditAnywhere,Category="Configuration")
	float TimeBetweenSpawnsOnReinforcement;

	UPROPERTY(EditAnywhere,Category="Configuration",meta=(AllowPrivateAccess = true))
	float SpawnZOffset;
	//References
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	AArenaManager* ArenaManager;

	UPROPERTY(EditAnywhere,Category="Configuration",meta=(AllowPrivateAccess = true))
	bool bLastEnemyRushPlayer;
	
public:	
	// Sets default values for this component's properties
	UEnemySpawnerComponent();

	void SpawnEnemies();

	void SetReferences(APlayerCharacter* NewPlayer, AArenaManager* NewArena);
	
	void AddSpawnPoint(AAISpawnPoint* NewSpawnPoint);
	
	void SpawnEnterArenaGroup(uint8 NumGroup);

	void SetCurrentEventEnterArena(int32 NewEventEnterArena);

	void SpawnReinforcements();

	void TryToInterruptReinforcementsSpawn();

	FORCEINLINE int GetRemainingEnterArenaEvents() const { return RemainingEnterArenaEvent;}
	FORCEINLINE bool EnterArenaEventsFinished() const { return RemainingEnterArenaEvent == 0; }

	FORCEINLINE ATriggerVolume* GetTriggerVolume() const { return TriggerVolume; }
	
	FORCEINLINE bool AllWavesHaveBeenCleared() const { return CurrentWave >= TotalWaves; }

	FORCEINLINE bool CurrentWaveIsFinished() const { return SpawnPointsOnWaves[CurrentWave].IsEmpty(); }

	int32 GetEnemiesInCurrentWave() const;// { return SpawnPointsOnWaves[CurrentWave].Num(); }

	bool CurrentWaveIsFinished(int32 RemainingEnemies);

	FORCEINLINE bool IsLastWave() const { return CurrentWave == TotalWaves - 1; }

	FORCEINLINE void ResetWaves() { CurrentWave = 0; }

	FORCEINLINE bool IsSingleWave() const { return TotalWaves == 1; }

	FORCEINLINE bool IsLastEnemyRushPlayerActive() const { return bLastEnemyRushPlayer; }
#if UE_EDITOR
	void SpawnTriggerVolume();
#endif
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckIfThereIsSingleEnemyLeftAlive(AAIControllerGeneric* Controller) const;
	
private:
	UFUNCTION()
	void SpawnHandlerOnActive();

	void SpawnEnemy(const AAISpawnPoint& SpawnPoint);

	void SpawnInteractable(const AAISpawnPoint& SpawnPoint);

	void SortSpawns(TArray<AAISpawnPoint*>& SpawnGroup) const;

	bool CheckImLastEnemy() const;
};



