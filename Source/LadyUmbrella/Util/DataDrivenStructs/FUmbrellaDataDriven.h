#pragma once

#include "CoreMinimal.h"
#include "FUmbrellaDataDriven.generated.h"

USTRUCT(BlueprintType)
struct FUmbrellaDataDriven : public FTableRowBase
{
	GENERATED_BODY()

	/** Time between each bullet shot */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireRate = 0.f;

	/** Reloading Time for the Shotgun, time between each slug */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ReloadingTime = 0;

	/** Amount of bullets inside the Umbrella to shoot */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MagazineCapacity = 0;

	/** Amount of bullets inside the reserve */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 StoredAmmoCapacity = 0;

	/** Max range in which the umbrella can do damage */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShotgunMaxRange = 0;
	
	/** Minimum damage an impact can produce */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DamagePerProjectileMin = 0;

	/** Maximum damage an impact can produce */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DamagePerProjectileMax = 0;

	/** Number of projectiles the shotgun shoots per shot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ProjectileCount = 0;

	/** Minimum damage an impact can produce */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DPS = 0;
	
	/** Max Damage a shot of the umbrella can make */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxShotDMG = 0;

	/** Amount of Damage Multiplier a shot can make if hits inside the Cylinder */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DMGMultiplierCylinderRadius = 0;
	
	/** Max range the hook can impact */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HookMaxRange = 0;
	
	/** Cooldown for the dart */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DartCooldown = 0;

	/** Shield initial health */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShieldEndurance = 0;

	/** Shield regeneration speed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShieldRegenerationSpeed = 0;

	/** Time it takes for the shield to start regenerating */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ShieldTimeToRegenerate = 0;

	/** Time the player gets stunned when the shield is broke */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StunTimePostShieldBrake = 0;

	/** Damage the player does attacking melee */
	// Done
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MeleeDMG = 0;

	/** Max range within the player can hit a melee */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeleeMaxRange = 0;

	/** Time between melee attacks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeleeAttackCadence = 0;

	/** Honestly I don't remember what this is */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeleeAttackCooldown = 0;

	/** What */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MeleeDPS = 0;
	
};
