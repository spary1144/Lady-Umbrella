// 
// Upgrade.h
// 
// Contains all the information regarding an Upgrade. 
// 
// Copyright Zulo Interactive. All Rights Reserved.
//

#pragma once
#include "Upgrade.generated.h"

USTRUCT(BlueprintType)
struct FUpgrade
{
	GENERATED_BODY()
	
private:
	
	uint8 Level;
	uint8 MaxLevel;
	TArray<uint8> Prices; // Dejo de momento una solución basada en un array que debe tener tamaño MaxLevel
	FText Name;
	FText Summary;
	
public:
	FUpgrade(){};
	FUpgrade(const FString Name, const FString Summary, const uint8 MaxLevel, const TArray<uint8> PriceList);

	FORCEINLINE uint8 GetLevel() const { return Level; }
	FORCEINLINE uint8 GetMaxLevel() const { return MaxLevel; }
	FORCEINLINE uint8 GetPrice() const { return Prices[Level]; }
	FORCEINLINE TArray<uint8> GetAllPrices() const { return Prices; }
	FORCEINLINE const FText& GetName() const { return Name;	}
	FORCEINLINE const FText& GetSummary() const { return Summary; }
	
	FORCEINLINE void SetMaxLevel(const uint8 NewMax) { MaxLevel = NewMax; }
	FORCEINLINE void SetName(const FText& NewText) { Name = NewText; }
	FORCEINLINE void SetSummary(const FText& NewText) { Summary = NewText; }
	FORCEINLINE void SetAllPrices(const TArray<uint8>& PriceList) { Prices = PriceList; }

	FORCEINLINE bool IsMaxedOut() const { return Level >= MaxLevel; }
	bool IsAvailable();
	
	void SetLevel(const uint8 NewLevel);
	void AddPrice(const uint8 NewPrice);
	void RemovePrice(const uint8 Index);
};
