// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyZonesComponent.h"

// Sets default values for this component's properties
UEnemyZonesComponent::UEnemyZonesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

int UEnemyZonesComponent::GetHiguestZone()
{
	int MaxKey = INT_MIN;

	for (const auto& Elem : ZonesPool)
	{
		if (Elem.Key > MaxKey)
		{
			MaxKey = Elem.Key;
		}
	}

	return MaxKey;
}

void UEnemyZonesComponent::Initialize()
{
	for (TPair<int, FZoneStruct>& Elem : ZonesPool)
	{
		Elem.Value.StoreInitialValues();
	}
}

void UEnemyZonesComponent::Reset()
{
	for (TPair<int, FZoneStruct>& Elem : ZonesPool)
	{
		Elem.Value.ResetToInitialValues();
	}
}

void UEnemyZonesComponent::ClearFreeCoversInZone(const int NumZone)
{
	if (ZonesPool.Find(NumZone))
	{
		ZonesPool.Find(NumZone)->FreeCovers = 0;
	}
}

void UEnemyZonesComponent::ClearEnemiesInZone(const int NumZone)
{
	if (ZonesPool.Find(NumZone))
	{
		ZonesPool.Find(NumZone)->Enemiescount = 0;
	}
}

void UEnemyZonesComponent::ClearZonesPool()
{
	ZonesPool.Empty();
}

bool UEnemyZonesComponent::IsZoneCreated(const int NumZone) const
{
	return ZonesPool.Contains(NumZone);
}


// Called when the game starts
void UEnemyZonesComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEnemyZonesComponent::AddEnemyToZone(const int NumZone)
{
	//Si no existe creo nueva zona
	if (!IsZoneCreated(NumZone))
	{
		ZonesPool.Emplace(NumZone);
	}
	ZonesPool.Find(NumZone)->Enemiescount++;
}

void UEnemyZonesComponent::RemoveEnemyFromZone(const int NumZone)
{
	if (IsZoneCreated(NumZone))
	{
		ZonesPool.Find(NumZone)->Enemiescount--;
	}
}

void UEnemyZonesComponent::AddFreeCoverToZone(const int NumZone)
{
	if (!IsZoneCreated(NumZone))
	{
		ZonesPool.Emplace(NumZone);
	}
	ZonesPool.Find(NumZone)->FreeCovers++;
}

void UEnemyZonesComponent::RemoveFreeCoverFromZone(const int NumZone)
{
	if (IsZoneCreated(NumZone))
	{
		ZonesPool.Find(NumZone)->FreeCovers--;
	}
}

TMap<int,FZoneStruct>* UEnemyZonesComponent::GetZonesPool()
{
	return &ZonesPool;
}

int UEnemyZonesComponent::GetFreeCoversInZone(const int NumZone) const
{
	if (!IsZoneCreated(NumZone)) return BadState;
	return ZonesPool.Find(NumZone)->FreeCovers;
}

int UEnemyZonesComponent::GetEnemiesInZone(const int NumZone) const
{
	if (!IsZoneCreated(NumZone)) return BadState;
	return ZonesPool.Find(NumZone)->Enemiescount;
}

// Called every frame
// void UEnemyZonesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	// ...
// }

