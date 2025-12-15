// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UmbrellaUpgradeInterface.generated.h"

class UUpgradeComponent;
UINTERFACE(MinimalAPI)
class UUmbrellaUpgradeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LADYUMBRELLA_API IUmbrellaUpgradeInterface
{
	GENERATED_BODY()

public:

	virtual UUpgradeComponent* GetUmbrellaUpgradeComponent() = 0;
	
};