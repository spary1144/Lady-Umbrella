// 
// ArenaManager.h
// 
// Manager that handles enemy spawning, victory condition when everyone is dead or reset when the player dies.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/AI/Components/EnemySpawnerComponent.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "ArenaManager.generated.h"

// Forward declaration

class AAmmo;
class UEnemyCoordinationComponent;
class UEnemySpawnerComponent;
class AEnemyCharacter;
class AAISpawnPoint;
class ACoordinationManager;
class APlayerCharacter;
class UEnemyZonesComponent;
class ATriggerBoxArena;
class ATriggerVolume;
class AGenericCharacter;
class ULU_GameInstance;
class UFModComponentInterface;
class UVoiceLineManager;
enum class ECombatMusicType : uint8;

#pragma region structs
USTRUCT(BlueprintType)
struct FEnemyPoolEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	AEnemyCharacter* Enemy = nullptr;

	UPROPERTY(VisibleAnywhere)
	int Group = 0;
	
	bool operator==(const FEnemyPoolEntry & other) const
	{
		return (other.Enemy == Enemy);
	}
};

USTRUCT()
struct FQueueStruct {
	GENERATED_BODY()

public:
	//Enemies Ids in zone
	UPROPERTY(VisibleInstanceOnly)
	TArray<int32> EnemiesIDs;

	void Clear()
	{
		EnemiesIDs.Empty();
	}
};



#pragma endregion
//Delegates
DECLARE_MULTICAST_DELEGATE(FRegisterSpawnsDelegate);
DECLARE_MULTICAST_DELEGATE(FResetTriggersDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyNearbyDeathDelegate, const FVector&);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReinforcementsCalled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArenaStarted, bool, bIsInCombat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArenaEnded, bool, bIsInCombat);

UCLASS()
class LADYUMBRELLA_API AArenaManager : public AActor, public ISaveInterface
{
	GENERATED_BODY()
#pragma region configuration
	//int -> num zone
	//struct -> arrray de flee enemies ( ids) 
	UPROPERTY(VisibleInstanceOnly,Category="Configuration")
	TMap<int32,FQueueStruct> FleeQueue;

	UPROPERTY(VisibleInstanceOnly,Category="Configuration")
	TArray<int32> CloseCombatQueue;
	
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	TArray<FEnemyPoolEntry> EnemiesPool;
	
	int32 NumEnemiesInCurrentWave;
	int32 NumEnemiesAlive;
	
	//arena has spawned enemies or not
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bEnemiesSpawned;
	//combat started
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bCombatStarted;
	//arena is active or not
	UPROPERTY(EditAnywhere,Category="Configuration")
	bool bArenaActive;

	UPROPERTY(EditAnywhere, Category="Configuration")
	bool bReActivateArenaOnPlayerDeath;

	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bEnemiesRegistered;
	
	UPROPERTY(EditAnywhere,Category="Configuration")
	float HearRadius;

	//if is false, arena is manageed as Mafia Arena
	UPROPERTY(EditAnywhere,Category="Configuration")
	bool bIsAgencyArena;

	UPROPERTY(EditInstanceOnly,Category="Configuration")
	bool bDetectEnemiesAccordingToDistance;
	
	//condition to know is reinforcement is needed
	UPROPERTY(EditInstanceOnly,Category="Configuration")
	bool bReinforcementsActivated;
	
	UPROPERTY(EditInstanceOnly,Category="Configuration",meta=(EditCondition = "bReinforcementsActivated == true",EditConditionHides))
	int32 NumEnemiesAliveToCallReinforcements;
	
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	bool bReinforcementsHasBeenCalled;
	
	//Triggers
	UPROPERTY(EditDefaultsOnly,Category="Triggers")
	TSubclassOf<ATriggerBoxArena> TriggerClass;

	UPROPERTY(EditAnywhere, Category="Triggers")
	int32 NumTriggersToSpawn;

	UPROPERTY(VisibleAnywhere,Category="Triggers")
	TArray<ATriggerBoxArena*> AttachedTriggers;

	const int MinNumberOfAttachedTriggers = 2;

	//Spawn Points
	UPROPERTY(EditDefaultsOnly,Category="EnemySpawns")
	TSubclassOf<AAISpawnPoint> SpawnPointClass;

	UPROPERTY(EditAnywhere, Category="EnemySpawns")
	int32 NumEnemySpawnPoints;

	UPROPERTY(VisibleAnywhere,Category="EnemySpawns")
	TArray<AAISpawnPoint*> AttachedEnemySpawns;

	const int32 MinNumberOfAttachedEnemySpawns = 1;

#pragma endregion
#pragma region components
	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* Billboard;

	UPROPERTY()
	UStaticMesh* StaticMeshForSpawnPoint;

	//Zones Component - manage zones in covers and enemies
	UPROPERTY(EditInstanceOnly)
	UEnemyZonesComponent* EnemyZonesComponent;

	UPROPERTY(EditInstanceOnly)
	UEnemySpawnerComponent* EnemySpawnerComponent;

	UPROPERTY(EditInstanceOnly)
	UEnemyCoordinationComponent* EnemyCoordinationComponent;
	
	UPROPERTY()
	APlayerCharacter* PlayerCharacter; //player reference

	UPROPERTY()
	ULU_GameInstance* GameInstance; //player reference

	UPROPERTY()
	UVoiceLineManager* VoiceLineManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta =(AllowPrivateAccess = true))
	UFModComponentInterface* CombatMusicFmodComponent;
	
	 //StopCombatParameter = 1: Combat music stops
	const float StopCombatParameter = 1.f;

	const FName StopCombatParameterName = "InCombat";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta =(AllowPrivateAccess = true))
	bool bShouldPlayCombatMusic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta =(AllowPrivateAccess = true))
	ECombatMusicType CombatMusicType;
	
#pragma endregion

	//agency deads
	UPROPERTY(VisibleAnywhere)
	int32 AgencySpecialistsEnemies;
	
	bool bArenaCleared;

	const FString CauseToPauseLogic = "PlayerHasSkippedArena";

	TArray<AAmmo*> AmmoBoxesDroppedInArena;
public:	
	//delegate to notifi spawn points to be registered
	FRegisterSpawnsDelegate OnRegisterSpawnsDelegate;

	FResetTriggersDelegate OnResetTriggersDelegate;

	FNotifyNearbyDeathDelegate OnAllyDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnReinforcementsCalled OnReinforcementsCalled;

	static FOnArenaStarted OnArenaStarted;
	static FOnArenaEnded OnArenaEnded;
	
	void Activate();

	void StartCombat();

	void EnterToArena(const uint8 NumGroup) const;

	FORCEINLINE bool IsActive() const { return bEnemiesSpawned; }

	FORCEINLINE bool IsCombatStarted() const { return bCombatStarted; }

	FORCEINLINE void SetReinforcementsCalled() { bReinforcementsHasBeenCalled = true; }

	FORCEINLINE bool ReinforcementsActivated() const { return bReinforcementsActivated; }
	
	FORCEINLINE UEnemyZonesComponent* GetEnemyZonesComponent() const { return EnemyZonesComponent; }

	FORCEINLINE UEnemySpawnerComponent* GetEnemySpawnerComponent() const { return EnemySpawnerComponent; }

	FORCEINLINE UEnemyCoordinationComponent* GetEnemyCoordinationComponent() const { return EnemyCoordinationComponent; }

	void AddAmmoBoxToDropArray(AAmmo* NewAmmoBox) { AmmoBoxesDroppedInArena.Add(NewAmmoBox); }
	
	void UnlockBlockingElement();
#pragma region  ZonesComponentMethods
	void ClearZone(const int NumZone) const;
	void ReduceEnemyInZone(const int NumZone) const;
	void ReduceFreeCoverInZone(const int NumZone) const;
	void RemoveEnemyFromZone(AEnemyCharacter* Enemy) const;
	void AddCoverZone(const int NumZone) const;
	void AddEnemyZone(const int NumZone) const;
	bool IsZoneCreated(const int NumZone) const;
	int32 GetFreeCoversInZone(const int NumZone) const;
	int32 GetEnemiesInZone(const int32 NumZone) const;
#pragma endregion

	void AddEnemyToPool(FEnemyPoolEntry NewEnemy);

	void RegisterSpawn(AAISpawnPoint* SpawnToRegister) const;

	int32 GetHighestZone() const;

	FORCEINLINE bool GetIsAgencyArena() const {return bIsAgencyArena; };
	
	UFUNCTION()
	FORCEINLINE TArray<FEnemyPoolEntry>& GetEnemyPool() { return EnemiesPool;};
	
	TMap<int32,FQueueStruct>* GetFleeQueue();

	TArray<int32>* GetCloseCombatQueue();
	// Sets default values for this actor's properties
	AArenaManager();

	UFUNCTION()
	void OnPlayerRespawn();

	UFUNCTION()
	void OnPlayerSkipArena();
	
	//Manage agency specialist enemies ( shield and flanker)
	FORCEINLINE int32  GetSpecialistEnemies() const{  return AgencySpecialistsEnemies; }
	FORCEINLINE void  AddSpecialistEnemies() { AgencySpecialistsEnemies = AgencySpecialistsEnemies + 1; }
	FORCEINLINE void  RemoveSpecialistEnemies() { AgencySpecialistsEnemies = AgencySpecialistsEnemies - 1; }
	FORCEINLINE int32 GetNumEnemiesInCurrentWave() const { return NumEnemiesInCurrentWave; }
	FORCEINLINE bool IsArenaCleared() const { return bArenaCleared; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void LoadGame() override{};
	virtual void SaveGame() override {};
	virtual void SaveDataCheckPoint() override{}
	virtual void LoadDataCheckPoint() override;
	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Temp")
	void OnArenaFinished();
	
private:

	void RemoveAmmoBoxesDroppedByEnemies();
	
	bool ReinforcementsCanBeCalled();
	
	//death delegates related functions
	UFUNCTION()
	void OnEnemyDeath(AGenericCharacter* DeadCharacter);
	void ClearEnemiesPool();

	UFUNCTION()
	void OnPlayerDeath(AGenericCharacter* DeadCharacter);
	//shoot delegates
	UFUNCTION()
	void OnPlayerShoot();

	//UFUNCTION()

#if UE_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	void DeleteIncorrectAttachedActors();
	void CheckMinNumberofAttachedActors();
	void CreateNewTriggerBoxInstances();
	void RemoveExcessTriggerBoxes();
	void CreateNewEnemySpawnPointsInstances();
	void RemoveExcessEnemySpawnPoints();
public:
	void SpawnTriggerVolume();
#endif
};
