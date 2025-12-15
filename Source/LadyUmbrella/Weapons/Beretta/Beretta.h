// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Weapons/GenericWeapon.h"
#include "Beretta.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API ABeretta : public AGenericWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int32 InitialAmmoVariance;
public:
	ABeretta();

	virtual void LoadWeaponFromDataTable() override;
	virtual void BeginPlay() override;
};
