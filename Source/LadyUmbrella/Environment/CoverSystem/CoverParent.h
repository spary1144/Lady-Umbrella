// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Components/HealthComponent/HealthComponent.h"
#include "LadyUmbrella/Interfaces/HealthInterface.h"
#include "CoverParent.generated.h"

class UTextRenderComponent;
class AAICoverEntryPoint;
class UNumericProgressBar;
class UCoverSpline;
class UBillboardComponent;
class UWidgetComponent;

UCLASS()
class LADYUMBRELLA_API ACoverParent : public AActor, public IHealthInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACoverParent();

#pragma region COMPONENTS
	
protected:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* BillboardRoot;
	
	UPROPERTY()
	UBillboardComponent* Billboard;

	UPROPERTY()
	UTextRenderComponent* ZoneTextRender;

	UPROPERTY(EditDefaultsOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* HealthWidgetRoot;

	UPROPERTY()
	UWidgetComponent* HealthWidget;

#pragma	endregion 

#pragma region PARAMETERS

protected:
	UPROPERTY(EditInstanceOnly, Category="Arena config")
	int NumZone;

	UPROPERTY(EditAnywhere, Category="Cover Configuration", meta=(DisplayPriority = 1))
	bool bCanBeDestroyed;

	UPROPERTY(EditDefaultsOnly, Category="Cover Configuration", meta=(DisplayPriority = 2))
	TSubclassOf<UNumericProgressBar> HealthWidgetBP;
	
	UPROPERTY()
	bool bNeedsOnConstructReset;
	
	UPROPERTY()
	bool bNeedsOnConstructUpdate;

#pragma endregion

#pragma region PUBLIC FUNCTIONS

public:
	UFUNCTION(BlueprintCallable)
	void DestroyCover();

	UFUNCTION()
	void SetZone(const int NewZone);

	UFUNCTION()
	FORCEINLINE int GetZone() const { return NumZone; };

	UFUNCTION()
	void ResetIaEntryPointsOwningSplines() const;

#pragma endregion

#pragma region EDITOR ONLY FUNCTIONS
	
protected:
	/**
	* Safe-Checks the Spline's Points, respawns the IA Entry Points and repositions them along the Spline.
	* Will NOT override the Number of IA Entry Points and their custom Section values.
	*/
	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = -3), DisplayName=UpdateAllSplines)
	void ForceUpdateAllSplines();
	
	/**
	* Safe-Checks the Spline's Points, resets the number of IA Entry Points, respawns them with new section values, and repositions them along the Spline.
	* Will override the Number of IA Entry Points and their custom Section values.
	*/
	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = 3), DisplayName=ResetAllSplines)
	void ResetAllSplines();

	UFUNCTION()
	void ShowAllPlayerEntryPoints() const;

	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = 4))
	void HideAllPlayerEntryPoints() const;

	UFUNCTION(CallInEditor, Category="Cover Configuration", meta=(DisplayPriority = 5))
	void DeleteAllAiEntryPoints() const;

	UFUNCTION()
	void CleanUnusedAttachedIaEntryPoints();

public:
	UFUNCTION()
	void FindConnectableEntryPoints(TArray<AAICoverEntryPoint*>& BufferArray) const;

#pragma endregion

#pragma region VIRTUAL FUNCTIONS
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	// We have EndPlay AND Destroyed cause one is used in Editor and the other is called first at runtime.
	// We were advised to use EndPlay over Destroyed, but EndPlay does not seem to work well in Editor.
	// Since both functions check if the Components they use are Valid, duplicating their functionality should not be a problem.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Destroyed() override;

#pragma endregion

#pragma region HEALTH INTERFACE

public:
	virtual void SubtractHealth(const float HealthToSubstract, EWeaponType DamageType) override;
	
	virtual void SetHealth(const float NewHealth) override;

	FORCEINLINE virtual const float GetHealth() const override { return IsValid(HealthComponent) ? HealthComponent->GetHealth() : 0; };

#pragma endregion 
	
};
