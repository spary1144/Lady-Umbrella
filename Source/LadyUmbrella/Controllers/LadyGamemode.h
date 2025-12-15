// 
// LadyGamemode.h
// 
// Overrides the default gamemode script to provide our own.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "GameFramework/GameMode.h"
#include "LadyGamemode.generated.h"

class AMainController;

UCLASS()
class LADYUMBRELLA_API ALadyGamemode : public AGameMode
{
	GENERATED_BODY()

public:

	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	ALadyGamemode();
};
