// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyZonesComponent.generated.h"

#pragma region Structs
USTRUCT()
struct FZoneStruct
{
	GENERATED_BODY()
public:
	//num of free covers in zone
	UPROPERTY(VisibleInstanceOnly)
	int Enemiescount = 0;
	//num of free covers in zone
	UPROPERTY(VisibleInstanceOnly)
	int FreeCovers = 0;

	//Initial Values
	int InitialEnemiescount = 0;
	int InitialFreeCovers = 0;

	//Set Initial values
	void StoreInitialValues()
	{
		InitialEnemiescount = Enemiescount;
		InitialFreeCovers = FreeCovers;
	}

	//Return to Initial Values
	void ResetToInitialValues()
	{
		Enemiescount = InitialEnemiescount;
		FreeCovers = InitialFreeCovers;
	}
};
#pragma endregion 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API UEnemyZonesComponent : public UActorComponent
{
	GENERATED_BODY()

	//int -> num zone
	//struct -> numenemies in zone and numfreecovers
	UPROPERTY(VisibleAnywhere,Category="Configuration")
	TMap<int,FZoneStruct> ZonesPool;

public:	
	//global constant
	static constexpr  int BadState = -1;
	// Sets default values for this component's properties
	UEnemyZonesComponent();

	int GetHiguestZone();

#pragma region ZonePoolMethods
	
	void Initialize();
	void Reset();
	
	void ClearFreeCoversInZone(const int NumZone);
	void ClearEnemiesInZone(const int NumZone);
	//void ReduceEnemyInZone(const int NumZone);

	bool IsZoneCreated(const int NumZone) const;
	
	UFUNCTION()
	void AddEnemyToZone(const int NumZone);

	UFUNCTION()
	void RemoveEnemyFromZone(const int NumZone);

	UFUNCTION()
	void AddFreeCoverToZone(const int NumZone);

	UFUNCTION()
	void RemoveFreeCoverFromZone(const int NumZone);
	
	TMap<int,FZoneStruct>* GetZonesPool();
	int GetFreeCoversInZone(const int NumZone) const;
	int GetEnemiesInZone(const int NumZone) const;
#pragma endregion
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

//public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	UFUNCTION(CallInEditor, Category="Configuration")
	void ClearZonesPool();

		
};
