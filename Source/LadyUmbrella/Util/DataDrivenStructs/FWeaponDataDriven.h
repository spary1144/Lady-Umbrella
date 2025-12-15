#pragma once

#include "CoreMinimal.h"
#include "FWeaponDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataDriven : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireRate = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadingTime = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MagazineCapacity = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StoredAmmoCapacity = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaximumRange = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DamagePerProjectileMin = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DamagePerProjectileMax = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ProjectileCount = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DPS = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 TotalHitDMG = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DartCooldown = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DartChargeTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DistanciaDisparoMin = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DistanciaDisparoMax = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbFalloMin = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbFalloMax = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PendienteFalloffDmgIZQ = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PendienteFalloffDmgDCH = 0.f;
	
};
