// 
// ABossCharacter.cpp
// 
// Implementation of the BossCharacter class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 


#include "BossCharacter.h"

#include "LadyUmbrella/Assets/DatatableAssets.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Components/VoiceLineComponent/VoiceLineComponent.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

ABossCharacter::ABossCharacter()
{
	VoicelineFmodComponent = CreateDefaultSubobject<UFModComponentInterface>("Voiceline Fmod Component");
	UFMODEvent* BossVoiceline = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/BOSS_SEQUENCER_CHAPTER_0.BOSS_SEQUENCER_CHAPTER_0'");
	FmodUtils::TryAddEvent(BossVoiceline, VoicelineFmodComponent);
	VoicelineFmodComponent->SetVoiceLineIdentifier("BOSS_");
}

void ABossCharacter::PostInitProperties()
{
	Super::PostInitProperties();
	GetVoiceLineComponent()->InitializeVoiceLine(FDatatableAssets::BossVoiceLines);
	GetVoiceLineComponent()->SetOwnerName(BOSS_DEFAULT_NAME);
}
