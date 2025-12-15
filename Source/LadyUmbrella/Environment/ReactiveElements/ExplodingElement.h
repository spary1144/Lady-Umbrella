// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReactiveElement.h"
#include "NiagaraFunctionLibrary.h"
#include "LadyUmbrella/Util/SaveSystem/SaveInterface.h"
#include "LadyUmbrella/Util/EditorShapeVis/ShapeVisualizer.h"
#include "ExplodingElement.generated.h"

class UNiagaraSystem;
class UAudioComponent;
class UCameraShakeSourceComponent;

UCLASS()
class LADYUMBRELLA_API AExplodingElement : public AReactiveElement, public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplodingElement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true", ClampMin = 1.f, ClampMax = 1000.f, UIMin = 1.f, UIMax = 1000.f))
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true", ClampMin = 1.f, ClampMax = 100.f, UIMin = 1.f, UIMax = 100.f))
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* NS_Explosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ExplosionAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShapeVisualizer> Visualizer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour", meta = (EditConditionHides, AllowPrivateAccess = "true"))
	bool bUseCustomShakeParams = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reactive Behaviour", meta = (EditConditionHides, AllowPrivateAccess = "true"))
	bool CanRespawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reactive Behaviour", meta = (EditCondition = "bUseCustomShakeParams", AllowPrivateAccess = "true"))
	FVector2D InOutRadiusParams = FVector2D(0.f, 10000.f);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Reaction(HitSource Source) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditUndo() override;
#endif
	
	UFUNCTION()
	virtual void LoadGame() override{};
	
	UFUNCTION()
	virtual void SaveGame() override{};
	
	UFUNCTION()
	virtual void SaveDataCheckPoint() override{};
	
	UFUNCTION()
	virtual void LoadDataCheckPoint() override;

};
