// 
// AVincenzoCharacter.cpp
// 
// Implementation of the VincenzoCharacter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "VincenzoCharacter.h"

#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

AVincenzoCharacter::AVincenzoCharacter()
{
	// GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::VincenzoVoiceLines);
	// GetVoiceLineComponent()->SetOwnerName(VINCENZO_DEFAULT_NAME);
	
	FmodSequencerVoicesComponent = CreateDefaultSubobject<UFModComponentInterface>("First Cinematic Fmod Component Interface");
	
	UFMODEvent* FmodFirstCinematicEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/VincenzoVoices/VINCENZO_SEQUENCER_CHAPTER_0.VINCENZO_SEQUENCER_CHAPTER_0'");
	FmodUtils::TryAddEvent(FmodFirstCinematicEvent,FmodSequencerVoicesComponent);

	UFMODEvent* FmodThirdCinematic = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/VincenzoVoices/VINCENZO_SEQUENCER_CHAPTER_5.VINCENZO_SEQUENCER_CHAPTER_5'");
	FmodUtils::TryAddEvent(FmodThirdCinematic,FmodSequencerVoicesComponent);

	FmodSequencerVoicesComponent->SetVoiceLineIdentifier("VINCENZO_");

}

void AVincenzoCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::VincenzoVoiceLines);
	GetVoiceLineComponent()->SetOwnerName(VINCENZO_DEFAULT_NAME);
}