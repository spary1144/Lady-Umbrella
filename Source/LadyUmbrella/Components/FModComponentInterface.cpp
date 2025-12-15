
#include "FModComponentInterface.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "VoiceLineComponent/VoiceLineComponent.h"

UFModComponentInterface::UFModComponentInterface()
{
	FmodComponent = CreateDefaultSubobject<UFMODAudioComponent>("FMODAudioComponent");
	FmodComponent->SetAutoActivate(false);
	bAllowConcurrentTick = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UFModComponentInterface::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		FmodComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		InitializeMap();
		UVoiceLineComponent* VoiceLineComponent = Owner->FindComponentByClass<UVoiceLineComponent>();
		if (IsValid(VoiceLineComponent))
		{
			VoiceLineComponent->OnVoiceLinePlayedDelegate.AddUObject(this, &UFModComponentInterface::VoiceLineComponentCallback);
		}
	}
}

void UFModComponentInterface::InitializeMap()
{
	if (!FMODAudioEventsMap.IsEmpty() && IsValid(FmodComponent))
	{
		TrySetFmodComponentEvent(FMODAudioEventsMap.begin().Value());
	}
}

bool UFModComponentInterface::SetCurrentEvent(const FString& EventToChange)
{
	if (!FMODAudioEventsMap.Contains(EventToChange))
	{
		return false;
	}

	TObjectPtr<UFMODEvent> CurrentEvent = GetFmodEvent();

	if (!IsValid(CurrentEvent))
	{
		FLogger::ErrorLog("Invalid FmodEvent pointer in FModComponentInterface.SetCurrentEvent()");
		return false;
	}
	
	// We don't have to change the event. It's assigned already
	if (CurrentEvent->GetName().Equals(EventToChange))
	{
		return true;
	}
	
	UFMODEvent* NewCurrentEvent = *FMODAudioEventsMap.Find(EventToChange);
	return TrySetFmodComponentEvent(NewCurrentEvent); 
}

void UFModComponentInterface::VoiceLineComponentCallback(const FString& EventToPlay, const double EventParameter)
{
	if (!IsValid(GetFModAudioComponent()))
	{
		return;
	}
	
	if (!IsValid(GetFmodEvent()))
	{
		//FLogger::ErrorLog("UFModComponentInterface::VoiceLineComponentCallback Event Not Valid");
		return;
	}
	
	const FString Temporal = GetVoiceLineIdentifier() + EventToPlay;

	const bool bIsEventInMap = FMODAudioEventsMap.Contains(Temporal);
	if (!bIsEventInMap)
	{
		return;
	}

	if (!GetFmodEvent()->GetName().Equals(Temporal))
	{
		if (!SetCurrentEvent(Temporal))
		{
			return;
		}
	}
	
	SetParameter(*Temporal, EventParameter);
	PlayEvent();
}

bool UFModComponentInterface::ValidateState() const
{
	if (!FmodComponent || !FmodComponent->Event)
	{
		if (!IsValid(GetOwner()))
		{
			FLogger::ErrorLog("Error FModComponentInterface::ValidateState");
			FLogger::ErrorLog("FmodComponentInterface IsValid: %i", IsValid(FmodComponent));
			FLogger::ErrorLog("FmodComponent: %i", IsValid(GetFmodEvent()));
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "[ERROR]: " + GetOwner()->GetName());
			return false;
		}
	}
	return true;
}

void UFModComponentInterface::PlayEvent() const 
{
	if (!ValidateState())
	{
		return;
	}
	FmodComponent->Play();
}

void UFModComponentInterface::PlayEventAtLocation(const FTransform& Location) const 
{
	if (!ValidateState())
	{
		return;
	}
	UFMODBlueprintStatics::PlayEventAtLocation(GetWorld(), GetFmodEvent(), Location, false);
}

void UFModComponentInterface::StopEvent() const 
{
	if (!ValidateState())
	{
		return;
	}
	FmodComponent->Stop();
}

void UFModComponentInterface::SetParameter(const FName& ParameterName, float ParameterValue) const
{
	if (!ValidateState() || !ParameterName.IsValid() || ParameterName.IsNone())
	{
		return;
	}
	FmodComponent->SetParameter(ParameterName, ParameterValue);
}

bool UFModComponentInterface::AddNewEvent(UFMODEvent* EventToAdd)
{
	if (!IsValid(EventToAdd) || EventToAdd->GetName().IsEmpty())
	{
		return false;
	}
	FMODAudioEventsMap.Emplace(EventToAdd->GetName(), EventToAdd);
	return true;
}

bool UFModComponentInterface::TrySetFmodComponentEvent(UFMODEvent* NewEvent) const
{
	if (IsValid(NewEvent))
	{
		SetFmodComponentEvent(NewEvent);
		return true;
	}
	return false;
}

