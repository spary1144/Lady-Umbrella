// 
// TubeElement.h
// 
// Defines the Tube Puzzle class 
// 
// Copyright Zulo Interactive. All Rights Reserved.
// 

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Environment/ReactiveElements/ReactiveElement.h"
#include "LadyUmbrella/Interfaces/HookInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "LadyUmbrella/Environment/TubeSpawnType.h"
#include "TubeElement.generated.h"

class UParticleSpawnerComponent;
class AActor;
class UNiagaraSystem;
class UAudioComponent;
class UCameraShakeSourceComponent;



UCLASS()
class LADYUMBRELLA_API ATubeElement : public AReactiveElement, public ISaveInterface, public IHookInterface
{
	GENERATED_BODY()

public:
	ATubeElement();

	virtual void Tick(float DeltaTime) override;
	virtual void Reaction(HitSource Source) override;

	UFUNCTION() virtual void LoadGame() override {};
	UFUNCTION() virtual void SaveGame() override {};
	UFUNCTION() virtual void SaveDataCheckPoint() override {};
	UFUNCTION() virtual void LoadDataCheckPoint() override;

	virtual void ContactWithCable(FVector& Direction, UPrimitiveComponent* ComponentHit) override{};
	virtual void ContactWithCable(FVector& Direction) override;
	virtual GrabType GetGrabType() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (ClampMin = 0.f, ClampMax = 60.f, UIMin = 0.f, UIMax = 60.f))
	float EnemyStunDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (ClampMin = 1.f, ClampMax = 1000.f, UIMin = 1.f, UIMax = 1000.f))
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (ClampMin = 1.f, ClampMax = 100.f, UIMin = 1.f, UIMax = 100.f))
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour")
	UNiagaraSystem* NS_Explosion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour")
	UAudioComponent* ExplosionAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour", meta = (EditConditionHides))
	bool bUseCustomShakeParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (EditConditionHides))
	bool CanRespawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour", meta = (EditCondition = "bUseCustomShakeParams"))
	FVector2D InOutRadiusParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSpawnerComponent* ParticleSpawnerComponent;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour")
	ESpawnType SpawnType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour")
	TSubclassOf<AActor> BlueprintActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Config")
	FRotator MeshRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Config")
	FVector CollisionOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Config")
	FVector MeshScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
	FVector BoxExtent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smoke Config")
	float SmokeTraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smoke Config")
	float SmokeSpawnRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Smoke Config")
	int32 SmokeParticleCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puddle Config")
	float PuddleTraceDepth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puddle Config")
	float PuddleFallbackZOffset;

	UFUNCTION()
	void SpawnSmoke();

	UFUNCTION()
	void SpawnPuddle();
	
	virtual void OnConstruction(const FTransform& Transform) override;
};
