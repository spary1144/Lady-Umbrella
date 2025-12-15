#pragma once

enum class ESteamAchievement : uint8;

#define STEAM_ENABLED 1

namespace Steam
{
	void Initialize();
	bool IsSteamAvailable();
	bool UnlockAchievement(const ESteamAchievement SteamAchievement);
};
