// 
// Upgrade.cpp
// 
// Implementation of the Upgrade class. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#include "Upgrade.h"
#include "LadyUmbrella/Characters/Player/PlayerCharacter.h"

FUpgrade::FUpgrade(const FString Name, const FString Summary, const uint8 MaxLevel, const TArray<uint8> PriceList) 
{
	Level		   = 0;
	this->MaxLevel = MaxLevel;
	this->Prices   = PriceList;
	this->Name	   = FText::FromString(Name);
	this->Summary  = FText::FromString(Summary);
}

bool FUpgrade::IsAvailable()
{
	if(!IsValid(FGlobalPointers::PlayerCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("Global Pointer error at UUpgradeSlot.SetupUpgradeSlots()"));
		return false;
	}

	if (!Prices.IsValidIndex(Level))
	{
		UE_LOG(LogTemp, Error, TEXT("Prices index out of range"));
		return false;
	}
	
	return !IsMaxedOut() && GetPrice() <= FGlobalPointers::PlayerCharacter->GetCurrentPiecesForUpgrades();
}

void FUpgrade::SetLevel(const uint8 NewLevel) { Level = FMath::Clamp(NewLevel, 0, MaxLevel); }
void FUpgrade::AddPrice(const uint8 NewPrice) { Prices.Add(NewPrice); }
void FUpgrade::RemovePrice(const uint8 Index) { Prices.RemoveAt(Index); }


