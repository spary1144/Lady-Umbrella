// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "Fnab.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class LADYUMBRELLA_API AFnab : public AGenericWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int32 InitialAmmoVariance;
	
public:
	AFnab();
	virtual void BeginPlay() override;
	virtual void LoadWeaponFromDataTable() override;
};
