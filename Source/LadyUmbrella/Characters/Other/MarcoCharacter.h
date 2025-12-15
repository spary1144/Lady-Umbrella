// 
// MarcoCharacter.h
// 
// Francesca's brother.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "MarcoCharacter.generated.h"

#define MARCO_DEFAULT_NAME FString("Marco")

class UFModComponentInterface;

UCLASS()
class LADYUMBRELLA_API AMarcoCharacter final : public AGenericCharacter
{
	GENERATED_BODY()
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                   COMPONENTS                    |
	// |                                                 |
	// +-------------------------------------------------+
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Sequencer Fmod Component Interface", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodSequencerVoicesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Cinematic Fmod Component Interface", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodCinematicVoicesComponent;

public:
	
	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	AMarcoCharacter();


	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;
};
