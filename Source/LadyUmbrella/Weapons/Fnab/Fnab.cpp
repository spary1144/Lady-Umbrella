// Fill out your copyright notice in the Description page of Project Settings.

#include "Fnab.h"

#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "LadyUmbrella/Util/AssetUtil.h"
#include "LadyUmbrella/Util/LoggerUtil.h"
#include "LadyUmbrella/Util/DataDrivenStructs/DataTableHelpers.h"
#include "LadyUmbrella/Util/DataDrivenStructs/FWeaponDataDriven.h"
#include "LadyUmbrella/Util/FmodUtil/FmodUtil.h"

AFnab::AFnab()
{
	static const wchar_t* SkeletalMeshAsset = TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_Fnab.SK_Fnab'");
	
	SetAmmoCurrent(6);
	SetAmmoCurrentMax(6);
	SetAmmoReserve(12);
	SetAmmoReserveMax(12);
	
	SetWeaponCadence(1.0f);
	InitialAmmoVariance = 7;

	GetSkeletalMeshComponent()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SkeletalMeshAsset).Object);
	
	DataTableRowHandler.RowName = "BP_Fnab";
	UFMODEvent* FmodReloadEvent = AssetUtils::FindObject<UFMODEvent>("/Script/FMODStudio.FMODEvent'/Game/FMOD/Events/EnemyWeapons/Mafia/BERETTA_VAR.BERETTA_VAR'");
	FmodUtils::TryAddEvent(FmodReloadEvent, GetFmodComponentInterface());	
}

void AFnab::BeginPlay()
{
	Super::BeginPlay();
	LoadWeaponFromDataTable();

	if (IsValid(GetFmodComponentInterface()))
	{
		GetFmodComponentInterface()->GetFModAudioComponent()->SetupAttachment(GetRootComponent());
		GetFmodComponentInterface()->InitializeMap();
	}
}

void AFnab::LoadWeaponFromDataTable()
{
	// Same functionallity as in Fnab, but diff values. Smelly
	const FWeaponDataDriven* FnabData = FDataTableHelpers::ValidateTableAndGetData<FWeaponDataDriven>(DataTableRowHandler, DataTableRowHandler.RowName);

	if (!FnabData)
	{
		FLogger::ErrorLog("Fnab Data not loaded from Table [Fnab == nullptr]");
		return;
	}

	SetWeaponCadence(FnabData->FireRate);
	//float ReloadingTime;
	SetAmmoCurrent(FnabData->MagazineCapacity - FMath::RandRange(0, InitialAmmoVariance));
	SetAmmoCurrentMax(FnabData->MagazineCapacity);
	SetAmmoReserve(FnabData->StoredAmmoCapacity);
	SetAmmoReserveMax(FnabData->StoredAmmoCapacity);
	SetWeaponRange(FnabData->MaximumRange);
	SetCurveMiddlePoint(FnabData->MaximumRange / 2);
	SetMinWeaponDamage(FnabData->DamagePerProjectileMin);
	SetMaxWeaponDamage(FnabData->DamagePerProjectileMax);
	SetDamageCurveLeftSlope(FnabData->PendienteFalloffDmgIZQ);
	SetDamageCurveRightSlope(FnabData->PendienteFalloffDmgDCH);
	//int32 ProjectileCount;
	//int32 DPS;
	//int32 TotalHitDMG;
	//float DartCooldown;
	//float DartChargeTime;
}