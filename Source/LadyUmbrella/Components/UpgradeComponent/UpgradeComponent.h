// 
// UpgradeComponent.h
// 
// Information about the possible umbrella upgrades go here. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "LadyUmbrella/Components/UpgradeComponent/Upgrade.h"
#include "LadyUmbrella/Components/UpgradeComponent/UpgradeType.h"

#include "UpgradeComponent.generated.h"

UCLASS()
class LADYUMBRELLA_API UUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TMap<EUpgradeType, uint8> UpgradeCost;

	TMap<EUpgradeType, FUpgrade> Upgrades;

public:
	
	UUpgradeComponent();

	virtual void BeginPlay() override;

	FUpgrade* Get(const EUpgradeType Type);

	FORCEINLINE TMap<EUpgradeType, FUpgrade>* GetUpgrades() { return &Upgrades; }

	FORCEINLINE void SetUpgrades(const TMap<EUpgradeType, FUpgrade>& NewUpgrades) { Upgrades = NewUpgrades; }
	
};
