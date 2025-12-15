// 
// LeonardoCharacter.h
// 
// Francesca's father
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "LadyUmbrella/Characters/GenericCharacter.h"
#include "LeonardoCharacter.generated.h"

#define LEONARDO_DEFAULT_NAME FString("Leonardo")

class UFModComponentInterface;

UCLASS()
class LADYUMBRELLA_API ALeonardoCharacter final : public AGenericCharacter
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UFModComponentInterface> VoicesComponent;
	
public:

	// +-------------------------------------------------+
	// |                                                 |
	// |                    FUNCTIONS                    |
	// |                                                 |
	// +-------------------------------------------------+
	ALeonardoCharacter();
};
