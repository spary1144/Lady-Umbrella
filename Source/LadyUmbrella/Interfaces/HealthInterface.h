// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LadyUmbrella/Weapons/WeaponTypes.h"
#include "HealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LADYUMBRELLA_API IHealthInterface
{
	GENERATED_BODY()

	// Interface to communicate with the Health Component of the GenericCharacter
public:
	virtual void SubtractHealth(const float Amount, const EWeaponType DamageType) = 0;
	virtual void SubtractHealth(const float Amount, const EWeaponType DamageType, const FVector& LastDamageSourceLocation) 
	{
		SubtractHealth(Amount, DamageType);
	}

	virtual void SetHealth(const float) 	 = 0;
	virtual const float GetHealth() const	 = 0;
};
