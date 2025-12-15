// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

class APlayerCharacter;

enum TYPE_OF_INTERACTION : uint8
{
	NO_PARAMS,
	PLAYER_CHARACTER,
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LADYUMBRELLA_API IInteractInterface
{
	GENERATED_BODY()

	TYPE_OF_INTERACTION InteractType = NO_PARAMS;
	
public:
	virtual int32 Interacting() = 0;
	virtual int32 Interacting(APlayerCharacter* Interactor) = 0;
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const = 0;
};
