// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HookableParent.generated.h"

class UFModComponentInterface;
class AHookImpactPoint;

UCLASS()
class LADYUMBRELLA_API AHookableParent : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFModComponentInterface> FmodSoundComponentInterface; 

protected:
	
#if WITH_EDITORONLY_DATA
	
	UPROPERTY()
	TArray<AHookImpactPoint*> ImpactPointsAux;
	
#endif WITH_EDITORONLY_DATA
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rotating params", meta = (AllowPrivateAccess = "true"))
	TArray<AHookImpactPoint*> ImpactPoints;

	TSubclassOf<AHookImpactPoint> HookImpactPointSubClass;

	bool bIsSolved;
	
	UFUNCTION(BlueprintCallable)
	virtual void RotateFromChildDynamic(const float DegreesOfRotation, const float SpeedOfRotation, const FRotator& RotatorForParent){}
	
	UFUNCTION(CallInEditor, DisplayName = "Add Impact Point", Category = "Settings", meta = (AllowPrivateAccess = "true"))
	void AddNewImpactPoint();
	
public:	
	
	AHookableParent();

	virtual void BeginPlay() override;

	FORCEINLINE TObjectPtr<UFModComponentInterface> GetFmodSoundComponentInterface() { return FmodSoundComponentInterface;}
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif WITH_EDITOR
	
};
