// 
// BlueprintAssets.h
// 
// Collection of pre-cached blueprint uassets.
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once
#include "LadyUmbrella/Weapons/Umbrella/Umbrella.h"

class AAgencyCharacter;
class AMarcoCharacter;
class AVincenzoCharacter;

namespace FBlueprintAssets
{
	inline TSubclassOf<AMarcoCharacter> MarcoCharacterBlueprint = nullptr;
	inline TSubclassOf<AUmbrella> UmbrellaBlueprint = nullptr;
	inline TSubclassOf<AVincenzoCharacter> VincenzoCharacterBlueprint = nullptr;
	inline TSubclassOf<AAgencyCharacter> AgencyCharacterBlueprint = nullptr;
}
