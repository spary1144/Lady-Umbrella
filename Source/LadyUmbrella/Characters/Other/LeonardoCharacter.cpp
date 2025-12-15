// 
// ALeonardoCharacter.cpp
// 
// Implementation of the LeonardoCharacter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "LeonardoCharacter.h"

#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

ALeonardoCharacter::ALeonardoCharacter()
{
	VoicesComponent = CreateDefaultSubobject<UFModComponentInterface>("Voices Component Interface");
	VoicesComponent->SetVoiceLineIdentifier("LEONARDO");
	
	UFMODEvent* VoicesEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/LEONARDO_SEQUENCER_CHAPTER_5.LEONARDO_SEQUENCER_CHAPTER_5'");
	FmodUtils::TryAddEvent(VoicesEvent,VoicesComponent);
	
	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::LeonardoVoiceLines);
	GetVoiceLineComponent()->SetOwnerName(LEONARDO_DEFAULT_NAME);
	VoicesComponent->SetVoiceLineIdentifier("LEONARDO_");
}
