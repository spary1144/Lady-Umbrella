

#pragma once

#include "CoreMinimal.h"
#include "ParticleSpawnerEnum.generated.h"

UENUM(BlueprintType)
enum class EParticleEffectType : uint8
{
	None					UMETA(DisplayName = "None"),
	NonLethalAmmoHit        UMETA(DisplayName = "NonLethalAmmoHit"),
	LethalAmmoHit           UMETA(DisplayName = "LethalAmmoHit"),
	UmbrellaBroken          UMETA(DisplayName = "UmbrellaBroken"),
	DartEquip				UMETA(DisplayName = "DartEquip"),
	HookEquip				UMETA(DisplayName = "HookEquip"),
	HookHit					UMETA(DisplayName = "HookHit"),
	HookShoot				UMETA(DisplayName = "HookShoot"),
	BulletCasing			UMETA(DisplayName = "BulletCasing"),
	SmokeExplosion			UMETA(DisplayName = "SmokeExplosion"),
	MuzzleflashUmbrella		UMETA(DisplayName = "MuzzleflashUmbrella")
};
