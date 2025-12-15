
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineInGameDTO.h"

#include "TriggerCinematic.generated.h"

class UBoxComponent;
class ULevelSequence;
class AGenericCharacter;
struct FVoiceLineInGameDTO;
enum class EVoiceLineState : uint8;
enum class EVoiceLineActor : uint8;
UCLASS()
class LADYUMBRELLA_API ATriggerCinematic : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsCommunicationCinematic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EVoiceLineState VoiceLineState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 VoiceLineStateIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> TriggerBox;

	bool Activated;
	
	const float VELOCITY_IN_GAME_CINEMATIC = 350.f;
	float SaveSpeed;
	
	FVoiceLineInGameDTO VoiceLineInGameDTO;
	
	void TriggerCinematic();
	void CalculatePlayerIndex(const int32 StateIndex, TArray<TArray<EVoiceLineActor>> CinematicQueue);
	void SpawnSecondaryActors();
	void ThrowCinematicVoiceLine();

	int32* GetCinematicCharacterIndex();
	TObjectPtr<AGenericCharacter> GetCinematicCharacter();

	UFUNCTION()
	void UpdateCinematicIndex();
public:

	ATriggerCinematic();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};