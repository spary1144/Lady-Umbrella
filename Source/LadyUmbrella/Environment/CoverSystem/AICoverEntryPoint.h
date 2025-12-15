// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AICoverEntryPoint.generated.h"

class UCoverSpline;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LADYUMBRELLA_API AAICoverEntryPoint : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AAICoverEntryPoint();


#pragma region CONSTANTS

	
	
#pragma endregion 
	
#pragma region COMPONENTS
	
	UPROPERTY()
	USceneComponent* Root;
	
	UPROPERTY()
	UStaticMeshComponent* StaticMesh;

#pragma endregion

#pragma region VARIABLES
	
private:
	UPROPERTY()
	UCoverSpline* OwningSpline;

	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority=0))
	float Section;

#pragma endregion

#pragma region EDITOR ONLY FUNCTIONS

private:
	UFUNCTION(CallInEditor, Category="Cover Configuration", DisplayName=UpdateCoverSpline)
	void UpdateCoverPointBasedOnSection();

public:
	UFUNCTION()
	UCoverSpline* EnsureOwningSplineIsSet();

#pragma endregion

#pragma region FUNCTIONS
	
public:
	UFUNCTION()
	FORCEINLINE void SetOwningSpline(UCoverSpline* _NewSpline) { OwningSpline = _NewSpline; }
	
	UFUNCTION()
	FORCEINLINE UCoverSpline* GetOwningSpline() const { return OwningSpline; }

	UFUNCTION()
	FORCEINLINE void SetSection(float _NewSection) { Section = FMath::Clamp(_NewSection, 0.f, 1.f); }

	UFUNCTION()
	FORCEINLINE float GetSection() const { return Section; }

#pragma endregion 
};
