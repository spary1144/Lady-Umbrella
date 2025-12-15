// 
// BossCharacter.h
// 
// Mafia boss that last for literally one freaking second.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "BossCharacter.generated.h"

#define BOSS_DEFAULT_NAME FString("Boss")

class UFModComponentInterface;

UCLASS()
class LADYUMBRELLA_API ABossCharacter final : public AGenericCharacter
{
	GENERATED_BODY()

	UPROPERTY() TObjectPtr<UFModComponentInterface> VoicelineFmodComponent;
	
	
public:

	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	ABossCharacter();
	virtual void PostInitProperties() override;
};
