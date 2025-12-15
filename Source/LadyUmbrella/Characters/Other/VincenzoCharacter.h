// 
// VincenzoCharacter.h
// 
// The bad guy.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "VincenzoCharacter.generated.h"

#define VINCENZO_DEFAULT_NAME FString("Vincenzo")

class UFModComponentInterface;

UCLASS()
class LADYUMBRELLA_API AVincenzoCharacter final : public AGenericCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Sequencer Voices Fmod Component Interface", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodSequencerVoicesComponent;

public:
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	AVincenzoCharacter();

	virtual void PostInitProperties() override;

};
