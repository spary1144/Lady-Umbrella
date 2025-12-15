// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FSaveStructPlayer.h"
#include "FChaptersProgress.h"
#include "LadyUmbrella/Components/UpgradeComponent/Upgrade.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeType.h"
#include "LadyUmbrella/UI/Basics/State/FInputConfig.h"
#include "LadyUmbrella/Weapons/Umbrella/GadgetType.h"
#include "PickableSaveStruct/FPickableSaveData.h"
#include "LU_SaveGame.generated.h"

enum class ESecurityState : uint8;
/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ULU_SaveGame : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, SaveGame)
	FSaveStructPlayer PlayerData;
	
	/**
	 * ~TODO: This has to be changed, having the checkpoint as a String is dangerous because
	 * same names can be used for different actors in different names~
	 * Latest: This is no longer used. Do not remove for future impl.
	*/
	
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString,bool> CheckPointsUnlocked;

	/**
	* ChaptersProgress: Contains the Chapters and the progress of them in game
	* @details FChaptersProgress: Is a struct that contains the state of the Chapter,
	* divided in bIsCompleted and bIsLastLevel
	* @details bIsCompleted: Marks a Chapter as completed
	* @details bIsLastLevel: Marks the Chapter as last reached for Loading Game purposes
	* @details FString has to be the names defined in
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, meta = (AllowPrivateAccess = true))
	TMap<FName, FChaptersProgress> ChaptersProgress;

	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<EGadgetType, bool> GadgetsUnlocked;

	UPROPERTY(SaveGame)
	TMap<int32, FPickableSaveData> GenericPickableProgress;

	UPROPERTY(SaveGame)
	TMap<EUpgradeType, FUpgrade> Upgrades;

	UPROPERTY(SaveGame)
	TMap<FName, FSectionConfig> OptionPopulationSections;
	
	UPROPERTY(SaveGame)
	FString LastSublevelName;

	UPROPERTY(SaveGame)
	ESecurityState SecurityState;

	// This is for the music being played in the first level
	UPROPERTY(Blueprintreadwrite, EditAnywhere, meta=(AllowPrivateAccess = "true"))
	bool bIsIntroMusicBeenPlayed;

	UPROPERTY(SaveGame)
	uint8 TotalPiecesPickedUp;
	
public:

	ULU_SaveGame();

	void InitializeDefaults();

	void SetLastSublevelName(const FString& NewLastSublevelName) { LastSublevelName = NewLastSublevelName; }

	UFUNCTION(BlueprintCallable) FORCEINLINE void SetIsIntroMusicBeenPlayed(const bool B) { bIsIntroMusicBeenPlayed = B; }
	
	FORCEINLINE void SetOptionPopulationSections(const TMap<FName, FSectionConfig>& NewOptionPopulationSections) { OptionPopulationSections = NewOptionPopulationSections; }

	FORCEINLINE const TMap<FName, FSectionConfig>& GetOptionPopulationSections() { return OptionPopulationSections; }
	
	[[nodiscard]] FORCEINLINE const FString& GetLastSublevelName() const { return LastSublevelName; }

	FORCEINLINE const TMap<FString,bool>& GetCheckPointsUnlocked() const { return CheckPointsUnlocked; }
	
	FORCEINLINE const TMap<FName, FChaptersProgress>& GetChaptersProgress() const { return ChaptersProgress; }
	
	FORCEINLINE TMap<FName, FChaptersProgress>& GetChaptersProgress() { return ChaptersProgress; }
	
	FORCEINLINE const TMap<EGadgetType, bool>& GetGadgetsUnlocked() const { return GadgetsUnlocked; }
	
	FORCEINLINE FSaveStructPlayer& GetPlayerData() { return PlayerData; } // :D javi - art was here
	
	FORCEINLINE const FSaveStructPlayer& GetPlayerData() const { return PlayerData; }

	FORCEINLINE void SetPlayerData(const FSaveStructPlayer& NewPlayerData) { PlayerData = NewPlayerData; }
	
	FORCEINLINE void SetCheckPointsUnlocked(const TMap<FString,bool>& NewCheckPointsUnlocked) { CheckPointsUnlocked = NewCheckPointsUnlocked; }
	
	FORCEINLINE void SetChaptersProgress(const TMap<FName, FChaptersProgress>& NewChaptersProgress) { ChaptersProgress = NewChaptersProgress; }
	
	FORCEINLINE void SetGadgetsUnlocked(const TMap<EGadgetType, bool>& NewGadgetsUnlocked) { GadgetsUnlocked = NewGadgetsUnlocked; }

	FORCEINLINE TMap<int32, FPickableSaveData>& GetGenericPickableProgress() { return GenericPickableProgress; }
	
	FORCEINLINE const TMap<int32, FPickableSaveData>& GetGenericPickableProgress() const { return GenericPickableProgress; }
	
	FORCEINLINE void SetGenericPickableProgress(const TMap<int32, FPickableSaveData>& NewGenericPickableProgress) { GenericPickableProgress = NewGenericPickableProgress; }

	FORCEINLINE void SetSecurityState(const ESecurityState NewSecurityState) { SecurityState = NewSecurityState; }

	FORCEINLINE ESecurityState GetSecurityState() const { return SecurityState; }

	FORCEINLINE void SetUpgradesMap(const TMap<EUpgradeType, FUpgrade>& NewUpgrades) { Upgrades = NewUpgrades; }

	FORCEINLINE const TMap<EUpgradeType, FUpgrade>& GetUpgradesMap() const { return Upgrades; }
	
	FORCEINLINE TMap<EUpgradeType, FUpgrade>& GetUpgradesMap() { return Upgrades; }

	FORCEINLINE uint8 GetTotalPiecesPickedUp() const { return TotalPiecesPickedUp; }

	FORCEINLINE void SetTotalPiecesPickedUp(const uint8 Value) { TotalPiecesPickedUp = Value; }
	
};