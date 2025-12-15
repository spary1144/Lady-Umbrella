#pragma once

UENUM(BlueprintType)
enum class EVoiceLineActor : uint8
{
	AGENCY   UMETA(DisplayName = "AGENCY"),
	BOSS     UMETA(DisplayName = "BOSS"),
	ENEMY    UMETA(DisplayName = "ENEMY"),
	LEONARDO UMETA(DisplayName = "LEONARDO"),
	MAFIA    UMETA(DisplayName = "MAFIA"),
	MARCO    UMETA(DisplayName = "MARCO"),
	PLAYER   UMETA(DisplayName = "PLAYER"),
	VINCENZO UMETA(DisplayName = "VINCENZO")
};
