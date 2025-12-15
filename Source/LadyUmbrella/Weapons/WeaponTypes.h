#pragma once
//Weapons Types
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Umbrella    UMETA(DisplayName = "Umbrella"),
	Rifle  UMETA(DisplayName = "Rifle"),
	Pistol UMETA(DisplayName = "Pistol"),
	Shield UMETA(DisplayName = "Shield"),
	Grenade UMETA(DisplayName = "Grenade"),
	ElectricProjectil UMETA(DisplayName = "ElectricProjectil"),
	Melee UMETA(DisplayName = "Melee"),
	Laser UMETA(DisplayName = "LaserDamage"),
	ExplosiveBarrier UMETA(DisplayName = "ExplosiveBarrier"),
	FallingObject UMETA(DisplayName = "FallingObject")
};

