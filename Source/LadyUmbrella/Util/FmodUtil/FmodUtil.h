#pragma once

#include "FMODBus.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"

class UFModComponentInterface;
class UFMODEvent;

namespace FmodUtils
{

	inline FMOD::Studio::System* StudioSystem = nullptr;
	inline TMap<FString, FMOD::Studio::Bus*> BusMap;

	inline TArray<FString> BusNames
	{
		"bus:/",
		"bus:/Voices",
		"bus:/SFX",
		"bus:/Music"
	};

	inline void InitializeFmodUtil()
	{
		StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);

		for (const FString& BusName : BusNames)
		{
			FMOD::Studio::Bus* Bus = nullptr;
			StudioSystem->getBus(TCHAR_TO_UTF8(*BusName), &Bus);
			BusMap.Add(BusName, Bus);
		}
	}
		
	inline bool ValidateAsset(const UObject* Asset)
	{
		return IsValid(Asset);
	}
	
	inline void TryAddEvent(UFMODEvent* EventToAdd, UFModComponentInterface* ComponentToAddTo)
	{
		if (!ValidateAsset(EventToAdd))
		{
			FLogger::ErrorLog("TryAddEvent EventToAdd Invalid to Component: " + ComponentToAddTo->GetName());
			return;
		}

		if (!ComponentToAddTo->AddNewEvent(EventToAdd))
		{
			FLogger::ErrorLog("TryAddEvent ComponentToAdd->AddNewEvent Failed");
		}
	}

	inline bool AttachFmodComponentToComponent(const UFModComponentInterface* Component, USceneComponent* ComponentToAttachTo)
	{
		if (!ValidateAsset(Component) || !ValidateAsset(Component->GetFmodEvent())) return false;

		if (!ValidateAsset(ComponentToAttachTo) && ValidateAsset(ComponentToAttachTo->GetOwner())) ComponentToAttachTo = ComponentToAttachTo->GetOwner()->GetRootComponent();

		Component->GetFModAudioComponent()->AttachToComponent(ComponentToAttachTo, FAttachmentTransformRules::KeepRelativeTransform);

		return true;
	}

	inline FMOD::Studio::Bus* ValidateBus(const FString& BusToValidate)
	{
		FMOD::Studio::Bus* NativeBus = nullptr;
		if (!StudioSystem->isValid())
		{
			return nullptr;
		}
		if (BusToValidate.IsEmpty())
		{
			return nullptr;
		}
		
		StudioSystem->getBus(TCHAR_TO_UTF8(*BusToValidate), &NativeBus);
		return NativeBus;
	}

	inline void SetBusVolume(const FString& BusToSetVolume, float Volume)
	{
		FMOD::Studio::Bus* Bus = ValidateBus(BusToSetVolume);
		if (Bus == nullptr) return;
		
		if (Bus->setVolume(Volume) != FMOD_OK)
		{
			FLogger::ErrorLog("SetBusVolume Failed");
		}
	}
	
	inline float GetBusCurrentVolume(const FString& BusToFindVolume)
	{
		const FMOD::Studio::Bus* NativeBus = ValidateBus(BusToFindVolume);
		
		if (NativeBus == nullptr) return 0.f;
		
		float CurrentVolume = 0.f;
		NativeBus->getVolume(&CurrentVolume);
		return CurrentVolume * 100.f;
	}
}
