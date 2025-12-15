// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Workbench.generated.h"

class UFModComponentInterface;
class UWorkBenchMenu;
class USphereComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USceneComponent;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorkBenchInteract, EUmbrellaLocation, UmbrellaLocation, bool, bIsImmediate, bool, bPlayAnim);

UCLASS()
class LADYUMBRELLA_API AWorkbench : public AGenericInteractable
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere)
	USceneComponent* PlayerPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UWorkBenchMenu> WorkbenchWidgetReference;

	UPROPERTY()
	UWorkBenchMenu* WorkbenchCreatedWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess ="true"))
	TObjectPtr<UFModComponentInterface> FmodComponent; 

	virtual void BeginPlay() override;

	void PlayFModEvent(const FString& EventToPlay) const;

	// virtual void OnConstruction(const FTransform& Transform) override;
	
	void DisableInputCustom();
	
	UFUNCTION()
	void SetUpWidget();
	
	UFUNCTION()
	void WorkbenchExit(EUmbrellaLocation UmbrellaLocation, bool bIsImmediate, bool bPlayAnim);

	virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* PlayerCharacternInteractor) override;
	// TODO: change this to PLAYER_CHARACTER and have that ptr do all
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; };

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	uint32 PiecesForUpgrade;

public:
	static FOnWorkBenchInteract OnWorkBenchInteract;
	AWorkbench();

};
