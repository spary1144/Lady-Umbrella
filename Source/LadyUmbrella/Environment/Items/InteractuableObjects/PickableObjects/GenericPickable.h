// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LadyUmbrella/Environment/Items/InteractuableObjects/GenericInteractable.h"
#include "LadyUmbrella/Components/FModComponentInterface.h"
#include "GenericPickable.generated.h"

/**
 * 
 */
UCLASS()
class LADYUMBRELLA_API AGenericPickable : public AGenericInteractable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	int32 AmountToPickup;

	UPROPERTY(EditAnywhere, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* OverlayMaterialInstance;
	

public:
	
	AGenericPickable();

	virtual void BeginPlay() override;

	virtual int32 Interacting() override;

	virtual int32 Interacting(APlayerCharacter* Interactor) override;

	UFUNCTION()
	void DestroyInteractable();
	
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; }

	FORCEINLINE int32 GetAmountToPickup() const { return AmountToPickup; }
	
	FORCEINLINE void SetAmountToPickup(const int Value) { AmountToPickup = Value; }

	FORCEINLINE TObjectPtr<UFModComponentInterface> GetPickUpSoundComponent() const { return PickUpSoundComponent; }

private:
	
	void SetOverlayMaterialForMeshes(UMaterialInstance* OverlayMaterial) const;

protected:

	FTimerHandle PickUpTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	UFModComponentInterface* PickUpSoundComponent;
	
	const FName PickUpEventParameterName = "PICKABLE_TYPE";

	bool TypeOfPickable;

};

