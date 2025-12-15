// 
// VoiceLineComponent.cpp
// 
// Implementation of the 'VoiceLineComponent' class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "VoiceLineComponent.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"
#include "LadyUmbrella/UI/HUD/MainHUD.h"
#include "LadyUmbrella/Util/Localization/LanguageCode.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "VoiceLineDatatableRow.h"
#include "VoiceLineManager.h"
#include "VoiceLineDTO.h"
#include "VoiceLineInGameDTO.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Util/GlobalUtil.h"

// +-------------------------------------------------+
// |                                                 |
// |                    FUNCTIONS                    |
// |                                                 |
// +-------------------------------------------------+

UVoiceLineComponent::UVoiceLineComponent()
{
	OwnerName      = FString("");
	StateIndices   = TMap<EVoiceLineState, FVector2D>();
	
	StateIndices.Emplace(EVoiceLineState::OTHER, FVector2d(0.0f));
	
	PrimaryComponentTick.bCanEverTick = false;

}

void UVoiceLineComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!IsValid(GameInstance))
	{
		return;
	}
	
	VoiceLineManager = GameInstance->GetSubsystem<UVoiceLineManager>();
}

void UVoiceLineComponent::InitializeVoiceLine(const TObjectPtr<UDataTable>& Value)
{
	if (!IsValid(Value))
	{
		FLogger::ErrorLog("Could not find dataTable in VoiceLineComponent.InitializeVoiceLines()");
		return;
	}

	TArray<FVoiceLineDatatableRow*> Rows;
	Value->GetAllRows<FVoiceLineDatatableRow>(VOICELINE_CONTEXT, Rows);

	VoiceLineTableRows.Empty();
	for (const FVoiceLineDatatableRow* Row : Rows)
	{
		VoiceLineTableRows.Add(*Row);
	}

	SortVoiceLineRows();
	FindVoiceLineIndices();
}

void UVoiceLineComponent::SpeakSequentialVoiceLine(const EVoiceLineState State)
{
	if (!CheckVoiceLineHasBeenFound(State))
	{
		return;
	}

	bool bShouldMovetoNext = true;
	uint8* LastUsedState   = &LastUsedStateIndex[State];
	
	if (*LastUsedState == VOICELINE_STATE_INVALID_INDEX)
	{
		bShouldMovetoNext = false;
		*LastUsedState    = StateIndices[State].X;
	}
	
	const uint8 VoiceLineCount = (StateIndices[State].Y - StateIndices[State].X) + 1;

	if (bShouldMovetoNext && (VoiceLineCount > 1))
	{
		++LastUsedStateIndex[State];

		if (*LastUsedState > StateIndices[State].Y)
		{
			*LastUsedState = StateIndices[State].X;
		}
	}
	
	uint8 Index = *LastUsedState - StateIndices[State].X;
	float Duration = VoiceLineTableRows[*LastUsedState].Duration;
	DisplaySubtitle(VoiceLineTableRows[*LastUsedState],Duration);
	OnVoiceLinePlayedDelegate.Broadcast(ToString(State), Index);	
}

void UVoiceLineComponent::PlayRandomVoiceLine(const FVoiceLineDTO& VoiceLineDTO)
{
	const uint8 RandomIndex = FilterByRepetition(VoiceLineDTO.State);
	if (RandomIndex == -1)
	{
		return;
	}
	DisplaySubtitle(VoiceLineTableRows[RandomIndex]);
	OnVoiceLinePlayedDelegate.Broadcast(ToString(VoiceLineDTO.State), RandomIndex - StateIndices[VoiceLineDTO.State].X);
}

void UVoiceLineComponent::PlaySpecificVoiceLine(const FVoiceLineDTO& VoiceLineDTO)
{
	DisplaySubtitle(VoiceLineTableRows[VoiceLineDTO.IndexState]);
	OnVoiceLinePlayedDelegate.Broadcast(ToString(VoiceLineDTO.State), VoiceLineDTO.IndexState - StateIndices[VoiceLineDTO.State].X);
}

float UVoiceLineComponent::GetDurationByIndex(const FVoiceLineInGameDTO& VoiceLineInGameDTO)
{
	const EVoiceLineState State = VoiceLineInGameDTO.VoiceLineDTO.State;
	int32 StateIndex = GetIndexByActor(VoiceLineInGameDTO);
	const int32 RealIndex = StateIndices[State].X + StateIndex;
	return VoiceLineTableRows[RealIndex].Duration;
}

void UVoiceLineComponent::QueueVoiceLine(const FVoiceLineDTO& VoiceLineDTO)
{
	if (!IsValid(VoiceLineManager) || !CheckVoiceLineHasBeenFound(VoiceLineDTO.State))
	{
		return;
	}
	
	VoiceLineManager->TryEnqueueRequest(VoiceLineDTO);
}

int32 UVoiceLineComponent::GetIndexByActor(const FVoiceLineInGameDTO& VoiceLineInGameDTO)
{
	switch (VoiceLineInGameDTO.VoiceLineDTO.Actor)
	{
	case EVoiceLineActor::PLAYER: return VoiceLineInGameDTO.PlayerIndex; 
	case EVoiceLineActor::MARCO: return VoiceLineInGameDTO.MarcoIndex;
	case EVoiceLineActor::VINCENZO: return VoiceLineInGameDTO.VincenzoIndex;
	case EVoiceLineActor::AGENCY: return VoiceLineInGameDTO.AgencyIndex;
	default: return 0;
	}
}

void UVoiceLineComponent::SpeakSpecificVoiceLine(const FVoiceLineInGameDTO& VoiceLineInGameDTO)
{
	if (!CheckVoiceLineHasBeenFound(VoiceLineInGameDTO.VoiceLineDTO.State))
		return;

	const EVoiceLineState State = VoiceLineInGameDTO.VoiceLineDTO.State;

	
	int32 StateIndex = GetIndexByActor(VoiceLineInGameDTO);

	const int32 RealIndex = StateIndices[State].X + StateIndex;

	if (StateIndices[State].X > RealIndex || StateIndices[State].Y < RealIndex)
	{
		FLogger::ErrorLog("Invalid StateIndex in VoiceLineComponent.SpeakSpecificVoiceLine()");
		return;
	}

	float Duration = VoiceLineTableRows[RealIndex].Duration;
	
	DisplaySubtitle(VoiceLineTableRows[RealIndex],Duration);
	
	OnVoiceLinePlayedDelegate.Broadcast(ToString(State), StateIndex);
}

void UVoiceLineComponent::DisplaySubtitle(const FVoiceLineDatatableRow& Row,const float& Duration)
{
	FString FilteredLanguage = FilterByLanguageCode(Row);
	
	if (IsValid(FGlobalPointers::MainController) && IsValid(FGlobalPointers::MainController->GetMainHUD()))
	{
		if (FGlobalPointers::MainController->IsSubtitlesEnabled())
			FGlobalPointers::MainController->GetMainHUD()->AddSubtitle(OwnerName, FilteredLanguage,Duration);
	}
}

void UVoiceLineComponent::ClearSubtitle() const
{
	if (IsValid(FGlobalPointers::MainController) && IsValid(FGlobalPointers::MainController->GetMainHUD()))
	{
		FGlobalPointers::MainController->GetMainHUD()->RemoveSubtitle(OwnerName);
	}
}

FString UVoiceLineComponent::FilterByLanguageCode(const FVoiceLineDatatableRow& Row)
{
	if (IsValid(FGlobalPointers::GameInstance))
	{
		switch (FGlobalPointers::GameInstance->GetLanguageCode())
		{
		case ELanguageCode::EN_US:
			return Row.EN_US;
		case ELanguageCode::ES_ES:
			return Row.ES_ES;
		case ELanguageCode::DE_DE:
			return Row.DE_DE;
		case ELanguageCode::SV_SE:
			return Row.SV_SE;
		case ELanguageCode::EU_ES:
			return Row.EU_ES;
		case ELanguageCode::CA_ES:
			return Row.CA_ES;
		}
	}

	return Row.EN_US;
}

uint8 UVoiceLineComponent::FilterByRepetition(const EVoiceLineState State)
{
	if (!StateIndices.Contains(State))
	{
		return -1;
	}
	
	if (StateIndices[State].X == StateIndices[State].Y)
	{
		return StateIndices[State].X;
	}
	
	uint8 RandomIndex = FMath::RandRange(StateIndices[State].X, StateIndices[State].Y);
	
	if (LastUsedStateIndex[State] == RandomIndex)
	{
		const uint8 VoiceLineCount = StateIndices[State].X + StateIndices[State].Y + 1;
		RandomIndex = (RandomIndex + 1) % VoiceLineCount;
	}

	LastUsedStateIndex[State] = RandomIndex;

	return RandomIndex;
}

void UVoiceLineComponent::FindVoiceLineIndices()
{
	int32 Index = 0;
	EVoiceLineState LastState = EVoiceLineState::OTHER;
	
	for (const FVoiceLineDatatableRow Row : VoiceLineTableRows)
	{
		if (!StateIndices.Contains(Row.VoiceLineState))
		{
			LastUsedStateIndex.Emplace(Row.VoiceLineState, VOICELINE_STATE_INVALID_INDEX);
			StateIndices.Emplace(Row.VoiceLineState, FVector2d(Index));
		}

		if ((LastState != Row.VoiceLineState) || (Index == VoiceLineTableRows.Num()))
		{
			StateIndices[LastState].Y = Index - 1;
			StateIndices[Row.VoiceLineState].X = Index;

			LastState = Row.VoiceLineState;
		}

		Index++;
	}

	StateIndices[LastState].Y = Index - 1;
}

void UVoiceLineComponent::SortVoiceLineRows()
{
	for (int32 Index = 0; Index < VoiceLineTableRows.Num() - 1; Index++)
	{
		bool bSwapped = false;
		
		for (int32 SubIndex = 0; SubIndex < VoiceLineTableRows.Num() - Index - 1; SubIndex++)
		{
			if (VoiceLineTableRows[SubIndex].VoiceLineState > VoiceLineTableRows[SubIndex + 1].VoiceLineState)
			{
				FVoiceLineDatatableRow Temp = VoiceLineTableRows[SubIndex];
				VoiceLineTableRows[SubIndex] = VoiceLineTableRows[SubIndex + 1];
				VoiceLineTableRows[SubIndex + 1] = Temp;
				bSwapped = true;
			}
		}

		if (!bSwapped)
		{
			break;
		}
	}
}

bool UVoiceLineComponent::CheckVoiceLineHasBeenFound(const EVoiceLineState State) const
{
	if (VoiceLineTableRows.IsEmpty())
	{
		//FLogger::ErrorLog("Must call InitializeVoiceLine before speaking a VoiceLine.");
		return false;
	}

	if (!StateIndices.Contains(State))
	{
		//FLogger::ErrorLog("No valid Voice Line found!");
		return false;
	}

	return true;
}

bool UVoiceLineComponent::CheckCanSpeakVoiceLine(const FVoiceLineDTO& VoiceLineRequest)
{
	if (!CheckVoiceLineHasBeenFound(VoiceLineRequest.State))
	{
		return false;		
	}

	if (!IsValid(VoiceLineManager))
	{
		return false;
	}
	
	if (!VoiceLineManager->IsSequenceCooldownFinished())
	{
		return false;
	}
	
	return true;
}
