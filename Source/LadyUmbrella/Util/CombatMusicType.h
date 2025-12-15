#pragma once

UENUM(BlueprintType)
enum class ECombatMusicType: uint8
{
	DEFAULT UMETA(DisplayName = "Default Music"),
	MAFIA   UMETA(DisplayName = "Mafia Music"),
	AGENCY  UMETA(DisplayName = "Agency Music")
};

static FString ToString(const ECombatMusicType& CombatMusicType)
{
	switch (CombatMusicType)
	{
	case ECombatMusicType::DEFAULT:
		return FString("DEFAULT_COMBAT_MUSIC");
	case ECombatMusicType::MAFIA:
		return FString("MAFIA_COMBAT_MUSIC");
	case ECombatMusicType::AGENCY:
		return FString("AGENCY_COMBAT_MUSIC");
	}
	return FString("INVALID_STATE");
}
