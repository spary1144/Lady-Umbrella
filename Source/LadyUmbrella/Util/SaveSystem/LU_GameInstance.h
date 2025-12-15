// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSavePuzzle.h"
#include "LU_SaveGame.h"
#include "Engine/GameInstance.h"
#include "LadyUmbrella/AI/Managers/ArenaManager.h"
#include "LadyUmbrella/UI/Basics/State/EPlatform.h"
#include "LadyUmbrella/UI/Credits/Credits.h"
#include "LadyUmbrella/Util/SaveSystem/FSaveStructPlayer.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "LU_GameInstance.generated.h"

class UCredits;
class ULoadingScreen;
class ISaveInterface;

class ACheckPointTrigger;
class AArenaManager;

enum class ELanguageCode : uint8;
enum class ESteamAchievement : uint8;
/**
 * ULU_GameInstance
 * Represents the game instance class that manages game-wide systems, states, and persistent data.
 * Typically used for orchestrating game-specific logic that persists beyond individual levels or sessions.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawned);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlatformUpdated, EPlatform)

UCLASS()
class LADYUMBRELLA_API ULU_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	ULU_GameInstance();

	virtual void Init() override;

	virtual void BeginDestroy() override;

	EPlatform GetPlatformName() const;
	
	void SetPlatformName(EPlatform NewPlatformName);

	int32 GetControlTypeCounter() const;
	
	void SetControlTypeCounter(const int NewControlTypeCounter);

	//Arenas Save
	void SaveLastArena(AArenaManager* LastArena) { ActivatedArenas.Push(LastArena); /*LastArenaManager = LastArena;*/ }
	void RemoveArena(AArenaManager* LastArena) { ActivatedArenas.Remove(LastArena); }
	FVector GetLastArenaPosition();
	AArenaManager* GetLastArena();

	//FORCEINLINE AArenaManager* GetLastArena() const { return LastArenaManager; }

	/**
	* Create Game Save Object
	* Creates a Save Game Object in case it doesn't exist
	*/
	UFUNCTION(BlueprintCallable)
	void CreateGameSaveObject();

	void SaveSublevelToGameSaveFile();
	
	/**
	* Save the game data
	* Save the Game Data into the Save Game Object
	*/
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	
	bool DestroySaveGameFile() const;

	[[nodiscard]] bool ReadSaveGameFile();

	/**
	* Load Game Data
	* Load the data saved from the Save Game Object into the game
	*/
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	UFUNCTION()
	void LoadAfterVerifyingSublevel();

	/**
	 * Get all data from the actors and set it into the GameInstance
	 */
	UFUNCTION(BlueprintCallable)
	void SaveDataToGameInstance();

	UFUNCTION(BlueprintCallable)
	void SaveCheckPoint();

	UFUNCTION(BlueprintCallable)
	void LoadCheckpoint();
	const FTransform& GetPlayerStartTransform() const;

	UFUNCTION()
	[[nodiscard]] bool UnlockLevelProgress(const FName& ChapterToUnlockProgress);

	[[nodiscard]] FORCEINLINE ULU_SaveGame* GetSaveGameFile() const { return SaveFile; }

	[[nodiscard]] bool IsSaveGameCreated() const { return IsValid(GetSaveGameFile()); }
	/**
	 * Setter should not be created as we manage its creation from GameInstance, but just in case
	 */
	FORCEINLINE void SetSaveFile(ULU_SaveGame* NewSaveFile) { SaveFile = NewSaveFile; }

	[[nodiscard]] FORCEINLINE FString GetCurrentSaveSlot() const { return CurrentSaveSlot; }
	
	FORCEINLINE void SetCurrentSaveSlot(const FString& NewCurrentSaveSlot) { CurrentSaveSlot = NewCurrentSaveSlot; }

	[[nodiscard]] FORCEINLINE FTransform GetLastTransformSaved() const { return LastTransformSaved; }
	
	FORCEINLINE void SetLastTransformSaved(const FTransform& NewLastTransformSaved) { LastTransformSaved = NewLastTransformSaved; }

	[[nodiscard]] FORCEINLINE FSaveStructPlayer& GetSaveStructPlayer() { return GI_SaveStructPlayer; }

	FORCEINLINE void SetSaveStructPlayer(const FSaveStructPlayer& NewSaveStructPlayer) { GI_SaveStructPlayer = NewSaveStructPlayer; }

	FORCEINLINE void SetLastCheckpointTrigger(ACheckPointTrigger* LastCheckpointHit) { LastCheckpointTrigger = LastCheckpointHit; }

	[[nodiscard]] FORCEINLINE ACheckPointTrigger* GetLastCheckpointTrigger() const { return LastCheckpointTrigger; }
	
	UFUNCTION()
	void OpenLevelFromLoadGame();

	UFUNCTION()
	void OpenLevelFromLoadGameDelegate(UWorld* World);

	UPROPERTY()
	FTimerHandle LevelReadyCheckHandle;

	UFUNCTION()
	void CheckLevelReady();
	
	bool DoesPersistentLevelContainSublevel(UWorld* World, const FString& SublevelName) const;

	UFUNCTION()
	void PostLoadingLevel(UWorld* World);

	static ISaveInterface* ValidateSaveInterface(AActor*& ActorToValidate);
		
	/**
	 * Array of pointers of all the Actors that can save data. Initialized in the constructor.
	 * Marked as deprecated. Will be deleted.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TArray<AActor*> ActorsWithSaveData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FSavePuzzle GI_SavePuzzle;

	UPROPERTY()
	FTimerHandle SublevelLoaderTimerHandle;

	virtual void Shutdown() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LoadingScreenClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> CreditsClass;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerRespawned OnPlayerRespawned;
	
	FOnPlatformUpdated OnPlatformUpdated;

	FORCEINLINE TObjectPtr<ULoadingScreen> GetLoadingScreenWidget() const { return LoadingScreenWidget; }
	FORCEINLINE TObjectPtr<UCredits> GetCreditsWidget() const { return CreditsWidget; }

	UFUNCTION(Blueprintable) void ShowCredits() const;

	/**
	 *  Localization functions
	 */
	FORCEINLINE ELanguageCode GetLanguageCode() const { return LanguageCode; }
	UFUNCTION(BlueprintCallable) FORCEINLINE void SetLanguageCode(const ELanguageCode Value) { LanguageCode = Value; }
	//
	
	void OpenLevelWithLoad(const FString& MapName);
	
	FORCEINLINE bool GetSaveGameFlag() const { return SaveGameFlag; }

	FORCEINLINE uint8 GetMeleeKills() const { return MeleeKills; }
	FORCEINLINE uint8 GetVaultStuns() const { return VaultStuns; }
	FORCEINLINE uint8 GetGrenadeRebounds() const { return GrenadeRebounds; }
	FORCEINLINE uint8 GetBulletsShotInLevel() const { return BulletsShotInLevel; }
	UFUNCTION(BlueprintCallable) FORCEINLINE uint8 GetDeathsInLevel() const { return DeathsInLevel; }

	FORCEINLINE void SetMeleeKills(const uint8 Value) { MeleeKills = Value; }
	FORCEINLINE void SetVaultStuns(const uint8 Value) { VaultStuns = Value; }
	FORCEINLINE void SetGrenadeRebounds(const uint8 Value) { GrenadeRebounds = Value; }
	FORCEINLINE void SetBulletsShotInLevel(const uint8 Value) { BulletsShotInLevel = Value; }
	FORCEINLINE void SetDeathsInLevel(const uint8 Value) { DeathsInLevel = Value; }

	void AddMeleeKill();
	void AddVaultStuns();
	void AddGrenadeRebounds();
	void AddBulletShotInLevel();
	void AddDeathsInLevel();

	UFUNCTION(BlueprintCallable)
	void UnlockAchievement(const ESteamAchievement Achievement);
	
private:
	
	bool SaveGameFlag;
		
	UPROPERTY()
	ULoadingScreen* LoadingScreenWidget;
	
	UPROPERTY()
	UCredits* CreditsWidget;

	UPROPERTY()
	FTransform LastTransformSaved;
	
	UPROPERTY(VisibleAnywhere)
	TArray<AArenaManager*> ActivatedArenas;
	/**
	 * SaveFile
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	ULU_SaveGame* SaveFile;

	UPROPERTY()
	ACheckPointTrigger* LastCheckpointTrigger;
	
	UPROPERTY()
	FString CurrentSaveSlot;

	EPlatform PlatformName;
	
	int32 ControlTypeCounter;

	/**
	 * Localization moved from the MainController to the Game Instance
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization", meta = (AllowPrivateAccess = "true"))
	ELanguageCode LanguageCode;

	/**
	 *	Struct that contains the Player Data Info in the Game Instance. There should only be one
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	FSaveStructPlayer GI_SaveStructPlayer;

	float LoadVerificationLoopTime;
	FTimerHandle LoadVerificationHandle;

	UPROPERTY()
	ULevelStreaming* SubLevel;

	uint8 MeleeKills;
	uint8 VaultStuns;
	uint8 GrenadeRebounds;
	uint8 BulletsShotInLevel;
	uint8 DeathsInLevel;
	
};

