// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GetWeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGetWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LADYUMBRELLA_API IGetWeaponInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual class AUmbrella* GetWeapon() const = 0;
};