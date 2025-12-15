
#pragma once

UENUM(BlueprintType)
enum class ESteamAchievement : uint8
{

	NONE                = 255 UMETA(DisplayName = "NONE"),
	LADY_UMBRELLA       = 0 UMETA(DisplayName = "Lady Umbrella"),
	THE_SHINING         = 1 UMETA(DisplayName = "The Shining"),
	GOTTA_PICK_EM_ALL   = 2 UMETA(DisplayName = "Gotta Pick'em All"),
	MINIMAX             = 3 UMETA(DisplayName = "Minimax"),
	MAXIMIN             = 4 UMETA(DisplayName = "Maximin"),
	SMASH_GOONS_MELEE   = 5 UMETA(DisplayName = "Smash Goons. Melee"),
	KICKSTARTING        = 6 UMETA(DisplayName = "Kickstarting"),
	THIS_IS_ITALIA      = 7 UMETA(DisplayName = "THIS. IS. ITALIAAAAAA"),
	RESEND              = 8 UMETA(DisplayName = "Resend"),
	ZUPA_DI_PESCE       = 9 UMETA(DisplayName = "Zupa di Pesce"),
	FISHING_ROUTINE     = 10 UMETA(DisplayName = "Fishing Routine"),
	MAN_IN_BLUE         = 11 UMETA(DisplayName = "Man in Blue"),
	ZIP_ZAP_ZUP         = 12 UMETA(DisplayName = "Zip Zap Zup"),
	MAKE_EACH_BULLET_COUNT = 13 UMETA(DisplayName = "Make Each Bullet Count"),
	MAFIA_CITY_ESCAPE  = 14 UMETA(DisplayName = "Mafia City Escape"),
	THE_FLOOR_IS_LAVA  = 15 UMETA(DisplayName = "The Floor is Lava"),
	NIGHT_AT_THE_ART_GALLERY = 16 UMETA(DisplayName = "Night at the Art Gallery"),
	MORY_POPPIN        = 17 UMETA(DisplayName = "Mory Poppin"),
	ANTENNA_UMBROLICA  = 18 UMETA(DisplayName = "Antenna Umbrolica"),
	SHIELD_BREAKER     = 19 UMETA(DisplayName = "Shield Breaker"),
	SALTARE_SUI_TETTI  = 20 UMETA(DisplayName = "Salt sui Tetti"),
	UNEXPECTED_REUNION = 21 UMETA(DisplayName = "Unexpected Reunion")
};

static const char* ToString(const ESteamAchievement SteamAchievement)
{
	switch (SteamAchievement)
	{
	default:
	case ESteamAchievement::NONE:
		return "NONE";
	case ESteamAchievement::LADY_UMBRELLA:
		return "LADY_UMBRELLA";
	case ESteamAchievement::THE_SHINING:
		return "THE_SHINING";
	case ESteamAchievement::GOTTA_PICK_EM_ALL:
		return "GOTTA_PICK_EM_ALL";
	case ESteamAchievement::MINIMAX:
		return "MINIMAX";
	case ESteamAchievement::MAXIMIN:
		return "MAXIMIN";
	case ESteamAchievement::SMASH_GOONS_MELEE:
		return "SMASH_GOONS_MELEE";
	case ESteamAchievement::KICKSTARTING:
		return "KICKSTARTING";
	case ESteamAchievement::THIS_IS_ITALIA:
		return "THIS_IS_ITALIA";
	case ESteamAchievement::RESEND:
		return "RESEND";
	case ESteamAchievement::ZUPA_DI_PESCE:
		return "ZUPA_DI_PESCE";
	case ESteamAchievement::FISHING_ROUTINE:
		return "FISHING_ROUTINE";
	case ESteamAchievement::MAN_IN_BLUE:
		return "MAN_IN_BLUE";
	case ESteamAchievement::ZIP_ZAP_ZUP:
		return "ZIP_ZAP_ZUP";
	case ESteamAchievement::MAKE_EACH_BULLET_COUNT:
		return "MAKE_EACH_BULLET_COUNT";
	case ESteamAchievement::MAFIA_CITY_ESCAPE:
		return "MAFIA_CITY_ESCAPE";
	case ESteamAchievement::THE_FLOOR_IS_LAVA:
		return "THE_FLOOR_IS_LAVA";
	case ESteamAchievement::NIGHT_AT_THE_ART_GALLERY:
		return "NIGHT_AT_THE_ART_GALLERY";
	case ESteamAchievement::MORY_POPPIN:
		return "MORY_POPPIN";
	case ESteamAchievement::ANTENNA_UMBROLICA:
		return "ANTENNA_UMBROLICA";
	case ESteamAchievement::SHIELD_BREAKER:
		return "SHIELD_BREAKER";
	case ESteamAchievement::SALTARE_SUI_TETTI:
		return "SALTARE_SUI_TETTI";
	case ESteamAchievement::UNEXPECTED_REUNION:
		return "UNEXPECTED_REUNION";
	}
}
