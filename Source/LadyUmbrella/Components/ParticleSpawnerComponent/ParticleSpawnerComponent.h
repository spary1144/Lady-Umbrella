#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "ParticleSpawnerStructs.h"
#include "ParticleSpawnerComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LADYUMBRELLA_API UParticleSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

	const int C_FORBIDDEN_INT = -1;
	const float C_FORBIDDEN_FLOAT = -1.0f;
public:
	UParticleSpawnerComponent();

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable)
	UNiagaraComponent* SpawnParticleByName(EParticleEffectType ParticleName, FVector Location, FRotator Rotation, bool bAutoDestroy = true);

	UFUNCTION(BlueprintCallable)
	UNiagaraComponent* SpawnParticle(UNiagaraSystem* System, FVector Location, FRotator Rotation, bool bAutoDestroy = true, EParticleEffectType ParticleType = EParticleEffectType::None);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	TArray<FNamedParticle> ParticleSystems;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	TArray<FParticleParamOverride> ParameterOverrides;

	UPROPERTY(EditAnywhere, Category = "Particles")
	float CasingVelocityFactor;
	
	UPROPERTY(EditAnywhere, Category = "Particles")
	FVector CasingVelocityVerticalOffset;
};
