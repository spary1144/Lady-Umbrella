// 
// VoiceLineManager.cpp
// 
// Implementation of the 'VoiceLineManager' class.  
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "VoiceLineManager.h"

#include "VoiceLineActor.h"
#include "VoiceLineComponent.h"
#include "VoiceLineDTO.h"
#include "VoiceLineState.h"
#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Util/GlobalUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"

class UVoiceLineComponent;
// +-------------------------------------------------+
// |                                                 |
// |                 LOCAL VARIABLES                 |
// |                                                 |
// +-------------------------------------------------+
//static UVoiceLineManager* VoicelineInstance;

// +-------------------------------------------------+
// |                                                 |
// |                    FUNCTIONS                    |
// |                                                 |
// +-------------------------------------------------+
// UVoiceLineManager::UVoiceLineManager()
// {
// 	
// }

void UVoiceLineManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bPlayingVoiceline = false;
	bIsCombatActivated = false;
	VoiceLineCooldownSequence = 5.0f;
	VoiceLineCooldownInGame = 15.f;
	
	CinematicQueues = TMap<EVoiceLineState, TArray<TArray<EVoiceLineActor>>>();

	CinematicQueues.Emplace(EVoiceLineState::CINEMATIC_CHAPTER1, {
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER }
	});

	CinematicQueues.Emplace(EVoiceLineState::CINEMATIC_CHAPTER2, {
		{ EVoiceLineActor::PLAYER, EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER }
	});

	CinematicQueues.Emplace(EVoiceLineState::CINEMATIC_CHAPTER3, {
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER, EVoiceLineActor::MARCO, EVoiceLineActor::PLAYER, EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER, EVoiceLineActor::AGENCY, EVoiceLineActor::PLAYER, EVoiceLineActor::AGENCY, EVoiceLineActor::PLAYER }
	});

	CinematicQueues.Emplace(EVoiceLineState::CINEMATIC_CHAPTER4, {
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER },
		{ EVoiceLineActor::PLAYER }
	});

	CinematicQueues.Emplace(EVoiceLineState::CINEMATIC_CHAPTER5, {
		{ EVoiceLineActor::MARCO, EVoiceLineActor::PLAYER, EVoiceLineActor::MARCO, EVoiceLineActor::PLAYER, EVoiceLineActor::MARCO },
		{ EVoiceLineActor::MAFIA, EVoiceLineActor::PLAYER, EVoiceLineActor::MARCO, EVoiceLineActor::PLAYER, EVoiceLineActor::MARCO, EVoiceLineActor::PLAYER }
	});
}

bool UVoiceLineManager::PlayVoiceLineAndResetTimer(const FVoiceLineDTO& Request)
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || FGlobalPointers::PlayerCharacter->IsDead() || FGlobalPointers::PlayerCharacter->IsPendingKillPending())
	{
		return false;
	}
	
	if (!Request.RequestingActor.IsValid() || Request.RequestingActor->IsDead() || Request.RequestingActor->IsPendingKillPending())
	{
		return false;
	}

	UVoiceLineComponent* Comp = Request.RequestingActor->FindComponentByClass<UVoiceLineComponent>();
	if (!IsValid(Comp))
	{
		return false;
	}

	bPlayingVoiceline = true;
	
	Request.IndexState == MAX_uint8 ? Comp->PlayRandomVoiceLine(Request) : Comp->PlaySpecificVoiceLine(Request);
	// start cooldown
	StartInGameVoiceLineCooldown();

	return true;
}

void UVoiceLineManager::ProcessVoiceLine(const FVoiceLineDTO& Request)
{
	if (!PlayVoiceLineAndResetTimer(Request))
	{
		return;
	}

	if (Request.IndexState != MAX_uint8) //if is specific voice line dont reset nothing
	{
		return;
	}
	
	// update history
	LastProcessedActors.Add(Request.Actor);
	if (LastProcessedActors.Num() > 2)
	{
		LastProcessedActors.RemoveAt(0);
	}
	
	//bool IsPrioritary = Request.PriorityAboveAll;
	int32 ProcessedVoiceLineID = Request.RequestingActor->GetUniqueID();
	VoiceLinesQueue.Remove(ProcessedVoiceLineID);
	
	// remove not prioritary request
	TArray<int32> RemovedVoiceLines;
	for (auto& VoiceLine : VoiceLinesQueue)
	{
		if (VoiceLine.Value.PriorityAboveAll)
		{
			continue;
		}
		RemovedVoiceLines.Add(VoiceLine.Key);
	}

	for (auto& VoiceLineKey : RemovedVoiceLines)
	{
		VoiceLinesQueue.Remove(VoiceLineKey);
		VoiceLineTimeStamps.Remove(VoiceLineKey);
	}
}

bool UVoiceLineManager::IsSequenceCooldownFinished()
{
	if (!IsValid(GetWorld()))
	{
		FLogger::ErrorLog("Invalid GameWorld pointer in VoiceLineManager.TriggerVoiceLine()");
		return false;
	}

	if (!bPlayingVoiceline)
	{
		bPlayingVoiceline = true;
		
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimerHandle,
			[this]() { this->ClearVoiceLineBuffer(); },
			VoiceLineCooldownSequence,
			false
		);

		return true;
	}
	return false;
}

void UVoiceLineManager::StartInGameVoiceLineCooldown()
{
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&UVoiceLineManager::ClearVoiceLineBuffer,
		VoiceLineCooldownInGame,
		false
	);
}

bool UVoiceLineManager::LastTwoWere(EVoiceLineActor Actor) const
{
	if (LastProcessedActors.Num() < MIN_NUMBER_OF_ENEMIES_QUEUED)
	{
		return false;
	}

	int32 Count = LastProcessedActors.Num();
	return (LastProcessedActors[Count - 1] == Actor &&
			LastProcessedActors[Count - 2] == Actor);
}

void UVoiceLineManager::ProcessVoiceLinesQueue()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || FGlobalPointers::PlayerCharacter->IsDead() || FGlobalPointers::PlayerCharacter->IsPendingKillPending())
	{
		return;
	}
	
	if (bPlayingVoiceline || VoiceLinesQueue.IsEmpty())
	{
		return;
	}
	
	TArray<FVoiceLineDTO> Enemies;
	bool bPlayerInQueue = false;
	FVoiceLineDTO PlayerDTO;

	//clean expired request
	TArray<int32> EpiredKeys;
	const double CurrentTime = GetWorld()->GetTimeSeconds();
	for (auto It = VoiceLinesQueue.CreateIterator(); It; ++It)
	{
		if (!It.Value().RequestingActor.IsValid() || It.Value().RequestingActor->IsDead() || It.Value().RequestingActor->IsPendingKillPending())
		{
			EpiredKeys.Add(It.Key());
			continue;
		}
		
		EVoiceLineActor Actor = It.Value().Actor;
		bool bIsPlayer = Actor == EVoiceLineActor::PLAYER;
		double MaxLifeTime = bIsPlayer ? MAX_TIME_IN_QUEUE_PLAYER : MAX_TIME_IN_QUEUE_ENEMY;
		
		const double* TimeStamp = VoiceLineTimeStamps.Find(It.Key());
		if (!TimeStamp || (CurrentTime - *TimeStamp > MaxLifeTime))
		{
			EpiredKeys.Add(It.Key());
			continue;
		}
		
		if (bIsPlayer)
		{
			PlayerDTO = It.Value();
			bPlayerInQueue = true;
		}
		else
		{
			Enemies.Add(It.Value());
		}
	}

	for (int32 Key : EpiredKeys)
	{
		VoiceLinesQueue.Remove(Key);
		VoiceLineTimeStamps.Remove(Key);
	}

	if (VoiceLinesQueue.IsEmpty())
	{
		return; // all expired
	}

	//only one queued actor
	if (VoiceLinesQueue.Num() == 1)
	{
		auto const& Pair = *VoiceLinesQueue.CreateIterator();
		ProcessVoiceLine(Pair.Value);
		return;
	}

	//at least two queued actors
	if (bPlayerInQueue && LastTwoWere(EVoiceLineActor::ENEMY))
	{
		ProcessVoiceLine(PlayerDTO);
		return;
	}

	FVoiceLineDTO* BestPrioritary = nullptr;
	double BestTimestamp = TNumericLimits<double>::Max();

	for (auto& Pair : VoiceLinesQueue)
	{
		const FVoiceLineDTO& DTO = Pair.Value;

		if (DTO.PriorityAboveAll)
		{
			if (const double* TimeStamp = VoiceLineTimeStamps.Find(Pair.Key))
			{
				if (*TimeStamp < BestTimestamp) // más antigua → menos tiempo en cola
				{
					BestTimestamp = *TimeStamp;
					BestPrioritary = &Pair.Value;
				}
			}
		}
	}

	if (BestPrioritary != nullptr)
	{
		ProcessVoiceLine(*BestPrioritary);
		return;
	}
	
	//choose random index between enemies actors
	const int32 Index = FMath::RandRange(0, Enemies.Num() - 1);
	if (Index < 0 || Index > Enemies.Num() - 1)
	{
		return;
	}
	ProcessVoiceLine(Enemies[Index]);
}


void UVoiceLineManager::TryEnqueueRequest(const FVoiceLineDTO& NewRequest)
{
	if (!IsValid(FGlobalPointers::PlayerCharacter) || FGlobalPointers::PlayerCharacter->IsDead() || FGlobalPointers::PlayerCharacter->IsPendingKillPending())
	{
		return;
	}
	
	if (!NewRequest.RequestingActor.IsValid() || NewRequest.RequestingActor->IsDead() || NewRequest.RequestingActor->IsPendingKillPending())
	{
		return;
	}
	const TWeakObjectPtr<AGenericCharacter>& NewRequestActor = NewRequest.RequestingActor;
	if (!NewRequestActor.IsValid() || NewRequestActor.Get()->IsDead())
	{
		return;
	}
	//Combat is not active, only player can request
	if (!bIsCombatActivated && !bPlayingVoiceline) 
	{
		PlayVoiceLineAndResetTimer(NewRequest);
		return;
	}
	
	//Combat is active, enqueue to manage
	const uint32 NewRequestActorID = NewRequest.RequestingActor->GetUniqueID();

	FVoiceLineDTO* ExistingVoiceLine = VoiceLinesQueue.Find(NewRequestActorID);
	if (ExistingVoiceLine == nullptr)
	{
		VoiceLinesQueue.Add(NewRequestActorID,NewRequest);
		VoiceLineTimeStamps.Add(NewRequestActorID, GetWorld()->GetTimeSeconds());
	}
	else
	{
		bool SameStates = ExistingVoiceLine->State == NewRequest.State;
		//update time if already exist
		double* time = VoiceLineTimeStamps.Find(NewRequestActorID);
		*time = GetWorld()->GetTimeSeconds();
		
		if (NewRequest.Actor == EVoiceLineActor::PLAYER && !SameStates)
		{
			ExistingVoiceLine->State = NewRequest.State;
			ExistingVoiceLine->PriorityAboveAll = NewRequest.PriorityAboveAll;
			
			FString State = UEnum::GetValueAsString(ExistingVoiceLine->State);
			FString Actor = UEnum::GetValueAsString(ExistingVoiceLine->Actor);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Actor: %s : %s"), *Actor, *State));
			
			return;
		}
		
		//Enemigos
		if (!SameStates && (!ExistingVoiceLine->PriorityAboveAll || NewRequest.PriorityAboveAll))
		{
			//update DTO
			ExistingVoiceLine->State = NewRequest.State;
			ExistingVoiceLine->PriorityAboveAll = NewRequest.PriorityAboveAll;

			FString State = UEnum::GetValueAsString(ExistingVoiceLine->State);
			FString Actor = UEnum::GetValueAsString(ExistingVoiceLine->Actor);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Actor: %s : %s"), *Actor, *State));
			
			return;
		}
	}
	
	if (!bPlayingVoiceline)
	{
		ProcessVoiceLinesQueue();
	}
}

void UVoiceLineManager::ClearVoiceLineBuffer()
{
	bPlayingVoiceline = false;
	ProcessVoiceLinesQueue();
}

void UVoiceLineManager::ResetManager()
{
	VoiceLinesQueue.Empty();
	VoiceLineTimeStamps.Empty();
	LastProcessedActors.Empty();
	bPlayingVoiceline = false;
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	World->GetTimerManager().ClearTimer(CooldownTimerHandle);
}
