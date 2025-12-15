// 
// VoiceLineManager.h
// 
// Singleton class that controls the different voice line components to avoid voice line overlapping.  
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VoiceLineDTO.h"
#include "VoiceLineManager.generated.h"


enum class EVoiceLineActor : uint8;
enum class EVoiceLineState : uint8;

UCLASS()
class LADYUMBRELLA_API UVoiceLineManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	const float MAX_TIME_IN_QUEUE_PLAYER = 1.f;
	const float MAX_TIME_IN_QUEUE_ENEMY = 3.f;
	const float MIN_NUMBER_OF_ENEMIES_QUEUED = 2.f;
	
	bool  bPlayingVoiceline;
	bool  bIsCombatActivated;
	
	float VoiceLineCooldownSequence;
	float VoiceLineCooldownInGame;
	
	FTimerHandle CooldownTimerHandle;
	
	//Priority queue
	TMap<uint32, FVoiceLineDTO> VoiceLinesQueue;
	TMap<uint32, double> VoiceLineTimeStamps;
	TArray<EVoiceLineActor> LastProcessedActors;
	
	TMap<EVoiceLineState, TArray<TArray<EVoiceLineActor>>> CinematicQueues;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |            CONSTRUCTORS & DESTRUCTORS           |
	// |                                                 |
	// +-------------------------------------------------+
	//UVoiceLineManager();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	bool PlayVoiceLineAndResetTimer(const FVoiceLineDTO& Request);
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	void ProcessVoiceLine(const FVoiceLineDTO& Request);
public:
	
	bool IsSequenceCooldownFinished();

	bool LastTwoWere(EVoiceLineActor Actor) const;
	void ProcessVoiceLinesQueue();
	
	void TryEnqueueRequest(const FVoiceLineDTO& NewRequest);

	void StartInGameVoiceLineCooldown();
	
	void ClearVoiceLineBuffer(void);
	//TODO: move to cpp later
	void ClearSpecificEnemyVoiceline(const int32 Id)
	{
		if (VoiceLinesQueue.Find(Id))
		{
			VoiceLinesQueue.Remove(Id);
			VoiceLineTimeStamps.Remove(Id);
		}
	}

	void ResetManager();// { VoiceLinesQueue.Empty(); VoiceLineTimeStamps.Empty(); LastProcessedActors.Empty();}
	FORCEINLINE void SetCombatState(bool active) {	bIsCombatActivated = active; }
	FORCEINLINE bool IsCombatState() const{	return bIsCombatActivated; }
	FORCEINLINE float GetVoiceLineCooldown() const { return VoiceLineCooldownSequence; }
	FORCEINLINE bool IsPlayingVoiceLine() const { return bPlayingVoiceline; }
	
	FORCEINLINE TMap<EVoiceLineState, TArray<TArray<EVoiceLineActor>>> GetCinematicQueues() const { return CinematicQueues; }
};

