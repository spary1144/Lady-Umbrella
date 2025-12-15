// 
// VFXAssets.h
// 
// Collection of pre-cached Visual Effects (VFX) uassets.
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

class UNiagaraSystem;
class UParticleSystem;

namespace FVFXAssets
{
	inline TObjectPtr<UNiagaraSystem>  BulletCasingNiagaraSystem  = nullptr;
	inline TObjectPtr<UNiagaraSystem>  DartEquipNiagaraSystem  = nullptr;
	inline TObjectPtr<UNiagaraSystem>  DustImpactNiagaraSystem = nullptr;
	inline TObjectPtr<UNiagaraSystem>  ElectricNiagaraSystem   = nullptr;
	inline TObjectPtr<UNiagaraSystem>  ElectricEnemiesNiagaraSystem   = nullptr;
	inline TObjectPtr<UNiagaraSystem>  HookEquipNiagaraSystem  = nullptr;
	inline TObjectPtr<UNiagaraSystem>  HookHitNiagaraSystem    = nullptr;
	inline TObjectPtr<UNiagaraSystem>  HookShootNiagaraSystem  = nullptr;
	inline TObjectPtr<UNiagaraSystem>  LethalAmmoHitNiagaraSystem    = nullptr;
	inline TObjectPtr<UNiagaraSystem>  NonLethalAmmoHitNiagaraSystem = nullptr;
	inline TObjectPtr<UNiagaraSystem>  SmokeExplosionNiagaraSystem   = nullptr;
	inline TObjectPtr<UNiagaraSystem>  MuzzleflashUmbrella			 = nullptr;
	inline TObjectPtr<UParticleSystem> UmbrellaMeleeNiagaraSystem    = nullptr;
}