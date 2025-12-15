#pragma once

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "ParticleSpawnerEnum.h"

#include "ParticleSpawnerStructs.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FNiagaraUserParameter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ParamName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor ColorValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VectorValue;
};

USTRUCT(BlueprintType)
struct FParticleParamOverride
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EParticleEffectType TargetEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FNiagaraUserParameter> Parameters;
};

USTRUCT(BlueprintType)
struct FNamedParticle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EParticleEffectType Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* System;
	
	 bool operator==(const FNamedParticle& Other) const
	    {
	        return Name == Other.Name;
	    }
};