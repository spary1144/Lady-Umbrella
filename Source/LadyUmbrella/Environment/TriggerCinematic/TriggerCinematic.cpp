
#include "TriggerCinematic.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LadyUmbrella/Assets/BlueprintAssets.h"
#include "LadyUmbrella/Characters/Enemies/Agency/AgencyCharacter.h"
#include "LadyUmbrella/Characters/Other/MarcoCharacter.h"
#include "LadyUmbrella/Characters/Other/VincenzoCharacter.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Components/StateComponent/PlayerStateEnum.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineActor.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineInGameDTO.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineManager.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineState.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/SaveSystem/LU_GameInstance.h"

class UVoiceLineManager;

ATriggerCinematic::ATriggerCinematic()
{
	IsCommunicationCinematic = false;

	VoiceLineState = EVoiceLineState::OTHER;
	VoiceLineStateIndex = 0;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>("CollisionBoxComponent");
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	TriggerBox->SetBoxExtent(FVector(256.0f, 256.0f, 64.0f));
	
	Activated = false;
}

// NOTE: Cannot be made const (regardless of what the IDE says).
void ATriggerCinematic::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(APlayerCharacter::StaticClass()) || Activated)
	{
		return;
	}

	Activated = true;
	
	VoiceLineInGameDTO.StateIndex = VoiceLineStateIndex;
	VoiceLineInGameDTO.MarcoIndex = 0;
	VoiceLineInGameDTO.PlayerIndex = 0;
	VoiceLineInGameDTO.PlayerCharacter = FGlobalPointers::PlayerCharacter;
	VoiceLineInGameDTO.VincenzoIndex = 0;
	
	VoiceLineInGameDTO.AgencyIndex = 0;
	
	VoiceLineInGameDTO.OrderArrayIndex = 0;
	VoiceLineInGameDTO.VoiceLineDTO.State = VoiceLineState;
	VoiceLineInGameDTO.VoiceLineDTO.Actor = EVoiceLineActor::PLAYER;
	VoiceLineInGameDTO.VoiceLineDTO.RequestingActor = FGlobalPointers::PlayerCharacter;
	
	TriggerCinematic();
}

void ATriggerCinematic::CalculatePlayerIndex(const int32 StateIndex, TArray<TArray<EVoiceLineActor>> CinematicQueue)
{
	if (StateIndex > 0)
	{
		for (int i = 0; i < StateIndex;i++)
		{
			for (EVoiceLineActor& actor : CinematicQueue[i])
			{
				if (actor != EVoiceLineActor::PLAYER)
				{
					continue;
				}
				VoiceLineInGameDTO.PlayerIndex++;
			}
		}
	}
}

void ATriggerCinematic::SpawnSecondaryActors()
{
	if (VoiceLineInGameDTO.OrderArray.Contains(EVoiceLineActor::MARCO))
	{
		VoiceLineInGameDTO.MarcoCharacter = GetWorld()->SpawnActor<AMarcoCharacter>(FBlueprintAssets::MarcoCharacterBlueprint);
	}
	if (VoiceLineInGameDTO.OrderArray.Contains(EVoiceLineActor::VINCENZO))
	{
		VoiceLineInGameDTO.VincenzoCharacter = GetWorld()->SpawnActor<AVincenzoCharacter>(FBlueprintAssets::VincenzoCharacterBlueprint);
	}
	if (VoiceLineInGameDTO.OrderArray.Contains(EVoiceLineActor::AGENCY))
	{
		ULU_GameInstance* GameInstance = Cast<ULU_GameInstance>(GetWorld()->GetGameInstance());
		FVector SpawnLocation = GameInstance->GetLastArenaPosition();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		VoiceLineInGameDTO.AgencyCharacter = GetWorld()->SpawnActor<AAgencyCharacter>(
			FBlueprintAssets::AgencyCharacterBlueprint.Get(),
			SpawnLocation,
			SpawnRotation
		);
	}
}

void ATriggerCinematic::TriggerCinematic()
{
	if (!IsValid(FGlobalPointers::PlayerCharacter))
	{
		return;
	}
	
	const EVoiceLineState State = VoiceLineInGameDTO.VoiceLineDTO.State;
	const int32 StateIndex = VoiceLineInGameDTO.StateIndex;
	
	const TMap<EVoiceLineState, TArray<TArray<EVoiceLineActor>>>& CinematicQueues =
		FGlobalPointers::GameInstance->GetSubsystem<UVoiceLineManager>()->GetCinematicQueues();

	if (!CinematicQueues.Contains(State))
	{
		FLogger::ErrorLog("Invalid CinematicQueue in PlayerCharacter.OnTriggerCinematic()");
		return;
	}
	
	TArray<TArray<EVoiceLineActor>> CinematicQueue = CinematicQueues[State];
	
	if ((StateIndex < 0) || (StateIndex >= CinematicQueue.Num()))
	{
		FLogger::ErrorLog("Invalid StateIndex in PlayerCharacter.OnTriggerCinematic()");
		return;
	}
	
	VoiceLineInGameDTO.OrderArray = CinematicQueue[StateIndex];

	SpawnSecondaryActors();

	CalculatePlayerIndex(StateIndex, CinematicQueue);

	//Stop movement and activate animation
	FGlobalPointers::PlayerCharacter->ActivateInComsMode(IsCommunicationCinematic);

	ThrowCinematicVoiceLine();
}

void ATriggerCinematic::ThrowCinematicVoiceLine()
{
	if (VoiceLineInGameDTO.OrderArrayIndex >= VoiceLineInGameDTO.OrderArray.Num())
	{
		FGlobalPointers::PlayerCharacter->DeActivateInComsMode(IsCommunicationCinematic);
		Destroy();
		return;
	}

	//Update actor
	VoiceLineInGameDTO.VoiceLineDTO.Actor = VoiceLineInGameDTO.OrderArray[VoiceLineInGameDTO.OrderArrayIndex];
	
	int32* VoiceLineCharacterIndex = GetCinematicCharacterIndex();
	AGenericCharacter* Character = GetCinematicCharacter();
	if (!IsValid(Character))
	{
		FLogger::ErrorLog("Character not valid");
		return;
	}
	
	const TObjectPtr<UVoiceLineComponent> CharacterVoiceLineComponent = Character->GetVoiceLineComponent();
	
	CharacterVoiceLineComponent->SpeakSpecificVoiceLine(VoiceLineInGameDTO);
	float VoiceLineDuration = CharacterVoiceLineComponent->GetDurationByIndex(VoiceLineInGameDTO);

	//DoWork(VoiceLineInGameDTO, VoiceLineCharacterIndex);
	
	FTimerHandle NextVoiceLineTimerHandle;
	FTimerDelegate NextVoiceLineDelegate;
	NextVoiceLineDelegate.BindUObject(this,&ATriggerCinematic::UpdateCinematicIndex);

	GetWorld()->GetTimerManager().SetTimer(
		NextVoiceLineTimerHandle,
		NextVoiceLineDelegate,
		VoiceLineDuration, //FGlobalPointers::GameInstance->GetSubsystem<UVoiceLineManager>()->GetVoiceLineCooldown()
		false
	);
}

int32* ATriggerCinematic::GetCinematicCharacterIndex()
{
	const EVoiceLineActor VoiceLineActor = VoiceLineInGameDTO.OrderArray[VoiceLineInGameDTO.OrderArrayIndex];

	if (VoiceLineActor == EVoiceLineActor::MARCO)
		return &VoiceLineInGameDTO.MarcoIndex;

	if (VoiceLineActor == EVoiceLineActor::VINCENZO)
		return &VoiceLineInGameDTO.VincenzoIndex;

	if (VoiceLineActor == EVoiceLineActor::AGENCY)
		return &VoiceLineInGameDTO.AgencyIndex;
	
	return &VoiceLineInGameDTO.PlayerIndex;
}

TObjectPtr<AGenericCharacter> ATriggerCinematic::GetCinematicCharacter()
{
	const EVoiceLineActor VoiceLineActor = VoiceLineInGameDTO.OrderArray[VoiceLineInGameDTO.OrderArrayIndex];

	if (VoiceLineActor == EVoiceLineActor::MARCO)
		return VoiceLineInGameDTO.MarcoCharacter;

	if (VoiceLineActor == EVoiceLineActor::VINCENZO)
		return VoiceLineInGameDTO.VincenzoCharacter;

	if (VoiceLineActor == EVoiceLineActor::AGENCY)
		return VoiceLineInGameDTO.AgencyCharacter;
	
	return VoiceLineInGameDTO.PlayerCharacter;
}

void ATriggerCinematic::UpdateCinematicIndex()
{
	int32* VoiceLineCharacterIndex = GetCinematicCharacterIndex();
	
	if (VoiceLineCharacterIndex == nullptr)
	{
		return;
	}
	
	(*VoiceLineCharacterIndex)++;
	
	VoiceLineInGameDTO.OrderArrayIndex++;
	VoiceLineInGameDTO.StateIndex = VoiceLineInGameDTO.OrderArrayIndex;
	
	ThrowCinematicVoiceLine();
}
