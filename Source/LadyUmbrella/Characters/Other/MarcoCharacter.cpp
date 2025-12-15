// 
// MarcoCharacter.cpp
// 
// Implementation of the MarcoCharacter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "MarcoCharacter.h"

#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

AMarcoCharacter::AMarcoCharacter()
{
	// Cinematic triggered in antenna cinematic
	FmodSequencerVoicesComponent = CreateDefaultSubobject<UFModComponentInterface>("Sequencer Voicelines Fmod Component Interface");
	static UFMODEvent* FmodSecondSequencer = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/Sequencer/MARCO_SEQUENCER_CHAPTER_3.MARCO_SEQUENCER_CHAPTER_3'");
	FmodUtils::TryAddEvent(FmodSecondSequencer,FmodSequencerVoicesComponent);
	
	// Cinematic triggered in final level
	static UFMODEvent* FmodThirdSequencer = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/Sequencer/MARCO_SEQUENCER_CHAPTER_5.MARCO_SEQUENCER_CHAPTER_5'");
	FmodUtils::TryAddEvent(FmodThirdSequencer,FmodSequencerVoicesComponent);

	FmodCinematicVoicesComponent = CreateDefaultSubobject<UFModComponentInterface>("Cinematic Voicelines Fmod Component Interface");

	// Cinematic triggered in final level
	static UFMODEvent* FmodFirstCinematic = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/Cinematic/MARCO_CINEMATIC_CHAPTER_2.MARCO_CINEMATIC_CHAPTER_2'");
	FmodUtils::TryAddEvent(FmodFirstCinematic,FmodCinematicVoicesComponent);

	static UFMODEvent* FmodSecondCinematic = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/Cinematic/MARCO_CINEMATIC_CHAPTER_3.MARCO_CINEMATIC_CHAPTER_3'");
	FmodUtils::TryAddEvent(FmodSecondCinematic,FmodCinematicVoicesComponent);

	static UFMODEvent* FmodFifthCinematic = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/MarcoVoices/Cinematic/MARCO_CINEMATIC_CHAPTER_5.MARCO_CINEMATIC_CHAPTER_5'");
	FmodUtils::TryAddEvent(FmodFifthCinematic,FmodCinematicVoicesComponent);
	
	FmodSequencerVoicesComponent->SetVoiceLineIdentifier("MARCO_");
	FmodCinematicVoicesComponent->SetVoiceLineIdentifier("MARCO_");
}

void AMarcoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FmodSequencerVoicesComponent->InitializeMap();
	FmodCinematicVoicesComponent->InitializeMap();
}

void AMarcoCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::MarcoVoiceLines);
	GetVoiceLineComponent()->SetOwnerName(MARCO_DEFAULT_NAME);
}
