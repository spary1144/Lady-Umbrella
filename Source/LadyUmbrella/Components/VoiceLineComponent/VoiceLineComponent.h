// 
// VoiceLineComponent.h
// 
// Component in charge of synchronizing audio and localized subtitles.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once
#include "GameFramework/CharacterMovementComponent.h"

#include "VoiceLineComponent.generated.h"

class AGenericCharacter;
class UVoiceLineManager;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVoiceLinePlayedDelegate, const FString&, const double);
DECLARE_MULTICAST_DELEGATE(FOnVoiceLineInterruptDelegate);

class AMainController;

enum class EVoiceLineState : uint8;
struct FVoiceLineDatatableRow;
struct FVoiceLineDTO;

#define VOICELINE_STATE_INVALID_INDEX 255
#define VOICELINE_CONTEXT TEXT("VoiceLineContext")

UCLASS()
class LADYUMBRELLA_API UVoiceLineComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UVoiceLineManager* VoiceLineManager;
	
	UPROPERTY()
	FString OwnerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TMap<EVoiceLineState, FVector2D> StateIndices;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TMap<EVoiceLineState, uint8> LastUsedStateIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<FVoiceLineDatatableRow> VoiceLineTableRows;
	float SaveSpeed;

public:

	FOnVoiceLinePlayedDelegate OnVoiceLinePlayedDelegate;
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	UVoiceLineComponent();
	virtual void BeginPlay() override;
	
	void InitializeVoiceLine(const TObjectPtr<UDataTable>& Value);
	UFUNCTION(BlueprintCallable) void SpeakSequentialVoiceLine(const EVoiceLineState State);
	UFUNCTION(BlueprintCallable) void QueueVoiceLine(const FVoiceLineDTO& VoiceLineDTO);
	int32 GetIndexByActor(const FVoiceLineInGameDTO& VoiceLineInGameDTO);
	UFUNCTION(BlueprintCallable) void SpeakSpecificVoiceLine(const FVoiceLineInGameDTO& VoiceLineInGameDTO);
	//UFUNCTION() void RestoreStateAndVelocity();
	
	void PlayRandomVoiceLine(const FVoiceLineDTO& VoiceLineDTO);
	void PlaySpecificVoiceLine(const FVoiceLineDTO& VoiceLineDTO);

	float GetDurationByIndex(const FVoiceLineInGameDTO& VoiceLineInGameDTO);

	void ClearSubtitle() const;
private:

	void DisplaySubtitle(const FVoiceLineDatatableRow& Row,const float& Duration = 3.f);
	
	FString FilterByLanguageCode(const FVoiceLineDatatableRow& Row);
	uint8   FilterByRepetition(const EVoiceLineState State);

	void FindVoiceLineIndices();
	void SortVoiceLineRows();

	bool CheckCanSpeakVoiceLine(const FVoiceLineDTO& VoiceLineRequest);
	bool CheckVoiceLineHasBeenFound(const EVoiceLineState State) const;
	// +-------------------------------------------------+
	// |                                                 |
	// |                GETTERS & SETTERS                |
	// |                                                 |
	// +-------------------------------------------------+
public:

	FORCEINLINE void SetOwnerName(const FString& Value) { OwnerName = Value; }

	FORCEINLINE TMap<EVoiceLineState, FVector2D> GetStateIndices() const { return StateIndices; }

};
