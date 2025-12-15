// Fill out your copyright notice in the Description page of Project Settings.

#include "Beretta.h"

#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FWeaponDataDriven.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

class UFMODEvent;

ABeretta::ABeretta()
{
	static const wchar_t* SkeletalMeshAsset = TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_Beretta.SK_Beretta'");
	
	SetAmmoCurrent(6);
	SetAmmoCurrentMax(6);
	SetAmmoReserve(12);
	SetAmmoReserveMax(12);
	SetWeaponCadence(1.0f);
	InitialAmmoVariance = 2;

	GetSkeletalMeshComponent()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SkeletalMeshAsset).Object);
	DataTableRowHandler.RowName = "BP_Beretta";
	UFMODEvent* FmodShootEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Agency/PISTOL.PISTOL'");
	if (!IsValid(FmodShootEvent))
	{
		return;
	}
	FmodUtils::TryAddEvent(FmodShootEvent, GetFmodComponentInterface());

	UFMODEvent* FmodElectricEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Agency/ELECTRIC_DART_SHOOT.ELECTRIC_DART_SHOOT'");
	if (!IsValid(FmodElectricEvent))
	{
		return;
	}
	FmodUtils::TryAddEvent(FmodElectricEvent, GetFmodComponentInterface());

	UFMODEvent* FmodElectricCharge = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Agency/ELECTRIC_DART_CHARGE.ELECTRIC_DART_CHARGE'");
	if (!IsValid(FmodElectricCharge))
	{
		return;
	}
	FmodUtils::TryAddEvent(FmodElectricCharge, GetFmodComponentInterface());	
}

void ABeretta::BeginPlay()
{
	Super::BeginPlay();
	LoadWeaponFromDataTable();
	if (IsValid(GetFmodComponentInterface()))
	{
		GetFmodComponentInterface()->GetFModAudioComponent()->SetupAttachment(GetRootComponent());
		GetFmodComponentInterface()->InitializeMap();
	}
}

void ABeretta::LoadWeaponFromDataTable()
{
	const FWeaponDataDriven* BerettaData = FDataTableHelpers::ValidateTableAndGetData<FWeaponDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);
	if (!BerettaData)
	{
		FLogger::ErrorLog("Beretta Data not loaded from Table [BerettaData == nullptr]");
		return;
	}
	
	SetWeaponCadence(BerettaData->FireRate);
	SetAmmoCurrent(BerettaData->MagazineCapacity - FMath::RandRange(0, InitialAmmoVariance));
	SetAmmoCurrentMax(BerettaData->MagazineCapacity);
	SetAmmoReserve(BerettaData->StoredAmmoCapacity);
	SetAmmoReserveMax(BerettaData->StoredAmmoCapacity);
	SetWeaponRange(BerettaData->MaximumRange);
	SetCurveMiddlePoint(BerettaData->MaximumRange / 2);
	SetElectricBulletChargeTime(BerettaData->DartChargeTime);
	SetMaxWeaponDamage(BerettaData->DamagePerProjectileMax);
	SetMinWeaponDamage(BerettaData->DamagePerProjectileMin);
	SetDamageCurveLeftSlope(BerettaData->PendienteFalloffDmgIZQ);
	SetDamageCurveRightSlope(BerettaData->PendienteFalloffDmgDCH);
	
	//BerettaData->ProjectileCount;
	//BerettaData->DPS;
	//BerettaData->TotalHitDMG;
	//BerettaData->DartCooldown;
	//BerettaData->ReloadingTime;
}


