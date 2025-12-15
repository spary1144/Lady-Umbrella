// Fill out your copyright notice in the Description page of Project Settings.


#include "TommyGun.h"

ATommyGun::ATommyGun()
{
	static const wchar_t* SkeletalMeshAsset = TEXT("/Script/Engine.SkeletalMesh'/Game/Meshes/SkeletalMeshes/SK_TommyGun.SK_TommyGun'");
	
	SetAmmoCurrent(20);
	SetAmmoCurrentMax(20);
	SetAmmoReserve(40);
	SetAmmoReserveMax(40);
	SetWeaponCadence(0.2f);

	GetSkeletalMeshComponent()->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(SkeletalMeshAsset).Object);
}

void ATommyGun::LoadWeaponFromDataTable()
{
	
}
