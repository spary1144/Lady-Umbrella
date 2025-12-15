// 
// VoiceLineState.h
// 
// Defines the different actions that are associated with voice lines.  
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once

UENUM(BlueprintType)
enum class EVoiceLineState : uint8
{
	IDLING    UMETA(DisplayName = "Idling"),
	MOVING    UMETA(DisplayName = "Moving"),
	JUMPING   UMETA(DisplayName = "Jumping"),
	VAULTING  UMETA(DisplayName = "Vaulting"),
	SHOOTING  UMETA(DisplayName = "Shooting"),
	THROWING  UMETA(DisplayName = "Throwing"),
	BOUNCING  UMETA(DisplayName = "Bouncing"),
	RELOADING UMETA(DisplayName = "Reloading"),
	SHIELDING UMETA(DisplayName = "Shielding"),
	IN_COVER  UMETA(DisplayName = "In Cover"),
	TAKING_COVER UMETA(DisplayName = "Taking Cover"),
	DEFEAT_ENEMY UMETA(DisplayName = "Defeating Enemy"),
	GADGET_HOOK     UMETA(DisplayName = "Using Hook"),
	GADGET_ELECTRIC UMETA(DisplayName = "Using Electric Dart"),
	SHIELD_DEPLETED UMETA(DisplayName = "Shield Depleted"),
	ENEMY_IN_COVER  UMETA(DisplayName = "Enemy in Cover"),
	DISTRACTING UMETA(DisplayName = "Distracting"),
	FLANKING    UMETA(DisplayName = "Flanking"),
	NONCOMBAT UMETA(DisplayName = "Non Combat"),
	FIRST_COMBAT_AGENCY UMETA(DisplayName = "First Combat Agency"),

	CINEMATIC_CHAPTER1 UMETA(DisplayName = "Cinematic (Chapter 1)"),
	CINEMATIC_CHAPTER2 UMETA(DisplayName = "Cinematic (Chapter 2)"),
	CINEMATIC_CHAPTER3 UMETA(DisplayName = "Cinematic (Chapter 3)"),
	CINEMATIC_CHAPTER4 UMETA(DisplayName = "Cinematic (Chapter 4)"),
	CINEMATIC_CHAPTER5 UMETA(DisplayName = "Cinematic (Chapter 5)"),
	
	SEQUENCER_CHAPTER0 UMETA(DisplayName = "Sequencer (Chapter 0)"),
	SEQUENCER_CHAPTER1 UMETA(DisplayName = "Sequencer (Chapter 1)"),
	SEQUENCER_CHAPTER2 UMETA(DisplayName = "Sequencer (Chapter 2)"),
	SEQUENCER_CHAPTER3 UMETA(DisplayName = "Sequencer (Chapter 3)"),
	SEQUENCER_CHAPTER4 UMETA(DisplayName = "Sequencer (Chapter 4)"),
	SEQUENCER_CHAPTER5 UMETA(DisplayName = "Sequencer (Chapter 5)"),

	OTHER UMETA(DisplayName = "Other")
};

ENUM_RANGE_BY_FIRST_AND_LAST(EVoiceLineState, EVoiceLineState::IDLING, EVoiceLineState::OTHER)

static int32 GetVoiceLineStateNum()
{
	int32 SizeOfVoiceLineState = 0;
	for (EVoiceLineState State : TEnumRange<EVoiceLineState>())
	{
		SizeOfVoiceLineState++;
	}
	return SizeOfVoiceLineState;
}

static FString ToString(const EVoiceLineState VoiceLineState)
{
	switch (VoiceLineState)
	{
		case EVoiceLineState::IDLING:
			return FString("IDLING");
		case EVoiceLineState::MOVING:
			return FString("MOVING");
		case EVoiceLineState::JUMPING:
			return FString("JUMPING");
		case EVoiceLineState::VAULTING:
			return FString("VAULTING");
		case EVoiceLineState::SHOOTING:
			return FString("SHOOTING");
		case EVoiceLineState::THROWING:
			return FString("THROWING");
		case EVoiceLineState::BOUNCING:
			return FString("BOUNCING");
		case EVoiceLineState::RELOADING:
			return FString("RELOADING");
		case EVoiceLineState::SHIELDING:
			return FString("SHIELDING");
		case EVoiceLineState::IN_COVER:
			return FString("IN_COVER");
		case EVoiceLineState::TAKING_COVER:
			return FString("TAKING_COVER");
		case EVoiceLineState::DEFEAT_ENEMY:
			return FString("DEFEAT_ENEMY");
		case EVoiceLineState::GADGET_HOOK:
			return FString("GADGET_HOOK");
		case EVoiceLineState::GADGET_ELECTRIC:
			return FString("GADGET_ELECTRIC");
		case EVoiceLineState::SHIELD_DEPLETED:
			return FString("SHIELD_DEPLETED");
		case EVoiceLineState::ENEMY_IN_COVER:
			return FString("ENEMY_IN_COVER");
		case EVoiceLineState::DISTRACTING:
			return FString("DISTRACTING");
		case EVoiceLineState::FLANKING:
			return FString("FLANKING");
		
		case EVoiceLineState::CINEMATIC_CHAPTER1:
			return FString("CINEMATIC_CHAPTER_1");
		case EVoiceLineState::CINEMATIC_CHAPTER2:
			return FString("CINEMATIC_CHAPTER_2");
		case EVoiceLineState::CINEMATIC_CHAPTER3:
			return FString("CINEMATIC_CHAPTER_3");
		case EVoiceLineState::CINEMATIC_CHAPTER4:
			return FString("CINEMATIC_CHAPTER_4");
		case EVoiceLineState::CINEMATIC_CHAPTER5:
			return FString("CINEMATIC_CHAPTER_5");
		
		case EVoiceLineState::SEQUENCER_CHAPTER0:
			return FString("SEQUENCER_CHAPTER_0");
		case EVoiceLineState::SEQUENCER_CHAPTER1:
			return FString("SEQUENCER_CHAPTER_1");
		case EVoiceLineState::SEQUENCER_CHAPTER2:
			return FString("SEQUENCER_CHAPTER_2");
		case EVoiceLineState::SEQUENCER_CHAPTER3:
			return FString("SEQUENCER_CHAPTER_3");
		case EVoiceLineState::SEQUENCER_CHAPTER4:
			return FString("SEQUENCER_CHAPTER_4");
		case EVoiceLineState::SEQUENCER_CHAPTER5:
			return FString("SEQUENCER_CHAPTER_5");
		
		default:
		case EVoiceLineState::OTHER:
			return FString("INVALID_STATE");
	}
}
