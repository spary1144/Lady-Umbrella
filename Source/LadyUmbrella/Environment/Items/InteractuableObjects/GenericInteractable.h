// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LadyUmbrella/Interfaces/InteractInterface.h"
#include "GenericInteractable.generated.h"

class AMainController;
class UArrowComponent;
class UInteractiveIcon;
class USphereComponent;
class UWidgetComponent;

UCLASS()
class LADYUMBRELLA_API AGenericInteractable : public AActor, public IInteractInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Logic", meta = (AllowPrivateAccess = "true"))
	bool bCanInteract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> WidgetRootLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInteractiveIcon> InteractiveIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FVector WidgetOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float InnerIconSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float InnerSphereTriggerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float OuterIconSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float OuterSphereTriggerRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float AnimationSpeed;

public:

	AGenericInteractable();
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable) virtual int32 Interacting() override;
	virtual int32 Interacting(APlayerCharacter* Interactor) override;
	virtual TYPE_OF_INTERACTION GetTypeOfInteraction() const override { return NO_PARAMS; }
	
	USphereComponent* GetSphereComponent() const;
	UWidgetComponent* GetWidgetComponent() const;
	UInteractiveIcon* GetInteractiveIcon() const;

	FORCEINLINE FVector GetWidgetOffset() const { return WidgetOffset; };
	FORCEINLINE float GetInnerRadius() const { return InnerSphereTriggerRadius; };
	FORCEINLINE float GetOuterRadius() const { return OuterSphereTriggerRadius; };
	
	void SetWidgetClass(TSubclassOf<UInteractiveIcon> WidgetClass);

protected:

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;
	
	UPROPERTY()
	TWeakObjectPtr<AMainController> MainController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;
	
	virtual void BeginPlay() override;

	FORCEINLINE bool CanInteract() const { return bCanInteract; }
	FORCEINLINE void SetCanInteract(const bool Value) { bCanInteract = Value; }
	FORCEINLINE UArrowComponent* GetWidgetRootLocation() const { return WidgetRootLocation; }
	
};
