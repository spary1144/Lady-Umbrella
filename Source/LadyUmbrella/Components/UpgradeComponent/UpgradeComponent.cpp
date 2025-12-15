// 
// UpgradeComponent.cpp
// 
// Implementation of the UmbrellaComponent class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#include "UpgradeComponent.h"

UUpgradeComponent::UUpgradeComponent()
{
	UpgradeCost.Emplace(EUpgradeType::AmmoCapacity, 1);
	UpgradeCost.Emplace(EUpgradeType::Handling, 3);
	UpgradeCost.Emplace(EUpgradeType::FireRate, 1);
	UpgradeCost.Emplace(EUpgradeType::ReloadSpeed, 2);

	// UpgradeCost.Emplace(EUpgradeType::Bounciness, 1);
	UpgradeCost.Emplace(EUpgradeType::Endurance, 4);
	UpgradeCost.Emplace(EUpgradeType::ShieldRegen, 1);
	// UpgradeCost.Emplace(EUpgradeType::ShieldMotion, 1);
}

void UUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Shotgun upgrades.
	Upgrades.Emplace(EUpgradeType::AmmoCapacity, FUpgrade("AmmoCapacity_Title", "AmmoCapacity_Description", 1,
	                                                      TArray{UpgradeCost[EUpgradeType::AmmoCapacity]}));
	Upgrades.Emplace(EUpgradeType::Handling, FUpgrade("Handling_Title", "Handling_Description", 1,
	                                                  TArray{UpgradeCost[EUpgradeType::Handling]}));
	Upgrades.Emplace(EUpgradeType::FireRate, FUpgrade("FireRate_Title", "FireRate_Description", 1,
	                                                  TArray{UpgradeCost[EUpgradeType::FireRate]}));
	Upgrades.Emplace(EUpgradeType::ReloadSpeed, FUpgrade("ReloadSpeed_Title", "ReloadSpeed_Description", 1,
	                                                     TArray{UpgradeCost[EUpgradeType::ReloadSpeed]}));

	// Shield upgrades. 
	// Upgrades.Emplace(EUpgradeType::Bounciness, FUpgrade("Bounciness","Increase the bounciness", 1,
	// 	TArray<uint8>({UpgradeCost[EUpgradeType::Bounciness]})));
	Upgrades.Emplace(EUpgradeType::Endurance, FUpgrade("Endurance_Title", "Endurance_Description", 1,
	                                                   TArray<uint8>({UpgradeCost[EUpgradeType::Endurance]})));
	Upgrades.Emplace(EUpgradeType::ShieldRegen, FUpgrade("ShieldRegen_Title", "ShieldRegen_Description", 1,
												   TArray<uint8>({UpgradeCost[EUpgradeType::ShieldRegen]})));
	// Upgrades.Emplace(EUpgradeType::ShieldMotion, FUpgrade("Shield Motion","Increase the shield agility", 1,
	// 	TArray<uint8>({UpgradeCost[EUpgradeType::ShieldMotion]})));
}

FUpgrade* UUpgradeComponent::Get(const EUpgradeType Type)
{
	return Upgrades.Find(Type);
}
