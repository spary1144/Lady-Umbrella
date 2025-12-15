#pragma once

class AMainController;
class AMarcoCharacter;
class APlayerCharacter;
class AUmbrella;
class AVincenzoCharacter;
class ULU_GameInstance;

namespace FGlobalPointers
{
	inline TObjectPtr<AMainController>    MainController    = nullptr;
	inline TObjectPtr<APlayerCharacter>   PlayerCharacter   = nullptr;
	inline TObjectPtr<AUmbrella>          Umbrella		    = nullptr;
	inline TObjectPtr<ULU_GameInstance>   GameInstance	    = nullptr;

	static void ResetGlobalPointers()
	{
		GameInstance	  = nullptr;
		MainController    = nullptr;
		PlayerCharacter   = nullptr;
		Umbrella		  = nullptr;
	}
};
