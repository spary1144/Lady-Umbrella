// 
// LadyGamemode.cpp
// 
// Implementation of the 'LadyGamemode' class.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "LadyGamemode.h"

#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"
#include "LadyUmbrella/Controllers/MainController.h"
#include "LadyUmbrella/Util/AssetUtil.h"

// +-------------------------------------------------+
// |                                                 |
// |                    FUNCTIONS                    |
// |                                                 |
// +-------------------------------------------------+
ALadyGamemode::ALadyGamemode()
{
	DefaultPawnClass = nullptr;

	DefaultPawnClass      = APlayerCharacter::StaticClass();
	PlayerControllerClass = AMainController::StaticClass();
}
