#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "FMODEvent.h"
#include "Components/ActorComponent.h"
#include "FModComponentInterface.generated.h"

class UFMODEvent;
class UFMODAudioComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UFModComponentInterface : public UActorComponent
{
	GENERATED_BODY()

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	UPROPERTY()
	UFMODAudioComponent* FmodComponent;

	FString VoiceLineIdentifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	TMap<FString, UFMODEvent*> FMODAudioEventsMap;
	
	bool ValidateState() const;

public:
	
	UFModComponentInterface();

	virtual void BeginPlay() override;
	
	/**
	 * We do this to set the FmodComponent->Event to the first event in the map
	 */
	void InitializeMap();
	
	void PlayEvent() const;
	
	void PlayEventAtLocation(const FTransform& Location) const;

	void StopEvent() const;
	
	void SetParameter(const FName& ParameterName, const float ParameterValue) const;

	FORCEINLINE bool IsEventPlaying() const noexcept { return GetFModAudioComponent()->IsPlaying(); }
	
	UFUNCTION()
	void VoiceLineComponentCallback(const FString& EventToPlay, const double EventParameter = -1);
	
	UFUNCTION(BlueprintCallable) [[nodiscard]] FORCEINLINE UFMODAudioComponent* GetFModAudioComponent() const noexcept { return FmodComponent; }
	
	[[nodiscard]] FORCEINLINE UFMODEvent* GetFmodEvent() const noexcept { return GetFModAudioComponent()->Event; }

	[[nodiscard]] FORCEINLINE const FString& GetVoiceLineIdentifier() const noexcept { return VoiceLineIdentifier; }
	
	FORCEINLINE void SetVoiceLineIdentifier(const FString& NewVoiceLineIdentifier) noexcept { VoiceLineIdentifier = NewVoiceLineIdentifier; }

	[[nodiscard]] FORCEINLINE const TMap<FString, UFMODEvent*>& GetFMODAudioEventsMap() const noexcept { return FMODAudioEventsMap; }
		
private:

	/**
	 * We don't want to set the FMod event by mistake to nullptr value.
	 * TrySetFmodComponentEvent calls this func if IsValid(NewEvent)
	 */
	FORCEINLINE void SetFmodComponentEvent(UFMODEvent* NewEvent) const { GetFModAudioComponent()->SetEvent(NewEvent); }

public:
	
	/**
	 <para name="EventToChange"> EventToChange The new Event to set into the FmodComponent->Event</para>
	 * This function searches in the map if EventToChange exists.
	 *
	 * If FALSE: return false;
	 * Then if EventToChange == FModComponentEvent->GetName()
	 *
	 * If TRUE: return true; (Event is already set)
	 * Then searches in the map for the EventToChange and TrySetFmodComponentEvent()
	 <returns> Returns true if the Event was already set or correctly set. False otherwise </returns>
	 */
	[[nodiscard]] bool SetCurrentEvent(const FString& EventToChange);
	
	bool TrySetFmodComponentEvent(UFMODEvent* NewEvent) const;
	
	FORCEINLINE void SetFMODAudioComponent(UFMODAudioComponent* NewFMODComponent) noexcept { FmodComponent = NewFMODComponent; }
	
	[[nodiscard]] bool AddNewEvent(UFMODEvent* EventToAdd);
	
};
