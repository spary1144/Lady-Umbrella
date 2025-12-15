// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "TommyGun.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class LADYUMBRELLA_API ATommyGun : public AGenericWeapon
{
	GENERATED_BODY()

public:
	ATommyGun();
	virtual void LoadWeaponFromDataTable() override;
};
