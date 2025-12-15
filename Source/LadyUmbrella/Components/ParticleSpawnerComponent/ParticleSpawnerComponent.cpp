#include "ParticleSpawnerComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Assets/VFXAssets.h"


UParticleSpawnerComponent::UParticleSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	if (IsValid(FVFXAssets::BulletCasingNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::BulletCasing, FVFXAssets::BulletCasingNiagaraSystem });
	if (IsValid(FVFXAssets::DartEquipNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::DartEquip, FVFXAssets::DartEquipNiagaraSystem });
	if (IsValid(FVFXAssets::HookEquipNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::HookEquip, FVFXAssets::HookEquipNiagaraSystem });
	if (IsValid(FVFXAssets::HookHitNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::HookHit,   FVFXAssets::HookHitNiagaraSystem });
	if (IsValid(FVFXAssets::HookShootNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::HookShoot, FVFXAssets::HookShootNiagaraSystem });
	if (IsValid(FVFXAssets::LethalAmmoHitNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::LethalAmmoHit,    FVFXAssets::LethalAmmoHitNiagaraSystem });
	if (IsValid(FVFXAssets::NonLethalAmmoHitNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::NonLethalAmmoHit, FVFXAssets::NonLethalAmmoHitNiagaraSystem });
	if (IsValid(FVFXAssets::SmokeExplosionNiagaraSystem)) ParticleSystems.Add({ EParticleEffectType::SmokeExplosion,   FVFXAssets::SmokeExplosionNiagaraSystem });
	if (IsValid(FVFXAssets::MuzzleflashUmbrella)) ParticleSystems.Add({ EParticleEffectType::MuzzleflashUmbrella,   FVFXAssets::MuzzleflashUmbrella });
	CasingVelocityFactor = -300.0f;
	CasingVelocityVerticalOffset = FVector(0.0f,0.0f, 500.0f);
}

void UParticleSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	// yes I know it's bad, TODO: @PALEXUTAD
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::BulletCasing, FVFXAssets::BulletCasingNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::BulletCasing, FVFXAssets::BulletCasingNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::DartEquip, FVFXAssets::DartEquipNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::DartEquip, FVFXAssets::DartEquipNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::HookEquip, FVFXAssets::HookEquipNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::HookEquip, FVFXAssets::HookEquipNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::HookHit, FVFXAssets::HookHitNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::HookHit, FVFXAssets::HookHitNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::HookShoot, FVFXAssets::HookShootNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::HookShoot, FVFXAssets::HookShootNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::LethalAmmoHit, FVFXAssets::LethalAmmoHitNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::LethalAmmoHit, FVFXAssets::LethalAmmoHitNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::NonLethalAmmoHit, FVFXAssets::NonLethalAmmoHitNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::NonLethalAmmoHit, FVFXAssets::NonLethalAmmoHitNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::SmokeExplosion, FVFXAssets::SmokeExplosionNiagaraSystem })) ParticleSystems.Add({ EParticleEffectType::SmokeExplosion, FVFXAssets::SmokeExplosionNiagaraSystem });
	if (!ParticleSystems.Contains(FNamedParticle{ EParticleEffectType::MuzzleflashUmbrella, FVFXAssets::MuzzleflashUmbrella })) ParticleSystems.Add({ EParticleEffectType::MuzzleflashUmbrella, FVFXAssets::MuzzleflashUmbrella });


}


UNiagaraComponent* UParticleSpawnerComponent::SpawnParticleByName(EParticleEffectType ParticleName, FVector Location, FRotator Rotation, bool bAutoDestroy)
{
	for (const FNamedParticle& Entry : ParticleSystems)
	{
		if (Entry.Name == ParticleName && Entry.System)
		{
			return SpawnParticle(Entry.System, Location, Rotation, bAutoDestroy, ParticleName);
		}
	}
	return nullptr;
}

UNiagaraComponent* UParticleSpawnerComponent::SpawnParticle(UNiagaraSystem* System, FVector Location, FRotator Rotation, bool bAutoDestroy, EParticleEffectType ParticleType)
{
	if (!System) return nullptr;

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		System,
		Location,
		Rotation,
		FVector(1.0f),
		bAutoDestroy,
		true,
		ENCPoolMethod::AutoRelease,
		true
	);

	// Yes I know it's a lot of ifs, leave me alone
	if (NiagaraComp && ParticleType != EParticleEffectType::None)
	{
		for (const FParticleParamOverride& Override : ParameterOverrides)
		{
			if (Override.TargetEffect == ParticleType)
			{
				for (const FNiagaraUserParameter& Param : Override.Parameters)
				{
					// Could be done differently probably, maybe an enum that let's you choose the type of the variable?
					if (!Param.ParamName.IsNone())
					{
						if (Param.ColorValue != FLinearColor::Transparent)
						{
							NiagaraComp->SetVariableLinearColor(Param.ParamName, Param.ColorValue);
						}
						if (Param.IntValue != C_FORBIDDEN_INT)
						{
							NiagaraComp->SetVariableInt(Param.ParamName, Param.FloatValue);
						}
						if (Param.FloatValue != C_FORBIDDEN_FLOAT)
						{
							NiagaraComp->SetVariableFloat(Param.ParamName, Param.FloatValue);
						}
						if (!Param.VectorValue.IsNearlyZero())
						{
							NiagaraComp->SetVariableVec3(Param.ParamName, Param.VectorValue);
						}
					}
				}
			}
		}
	}
	
	// Custom hardcoded velocity for casing ejection because no idea how to make it pretty right now
	if (ParticleType == EParticleEffectType::BulletCasing)
	{
		if (AActor* Owner = GetOwner())
		{
			FVector Velocity = CasingVelocityFactor * Owner->GetActorForwardVector() + CasingVelocityVerticalOffset;
			NiagaraComp->SetVariableVec3(FName("SpawnVelocity"), Velocity);
		}
	}

	return NiagaraComp;
}

